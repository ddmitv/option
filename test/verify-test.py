from __future__ import annotations
import re
import subprocess
import sys
from pathlib import Path
import enum
from dataclasses import dataclass
import os

# clang / gcc: "path/file.cpp:12:34: error: message"  (column sometimes absent)
CLANG_GCC_DIAG = re.compile(
    r"^(?P<file>.+?):(?P<line>\d+)(?::\d+)?:\s*(?:fatal\s+)?error:\s*(?P<msg>.+)$"
)
# msvc: "path\file.cpp(12,5-9): error C2338: message"  (C-code optional)
MSVC_DIAG = re.compile(
    r"^(?P<file>.+)\((?P<line>\d+)(?:,\d+(?:-\d+)?)?\):\s*(?:fatal\s+)?error(?:\s+C\d+)?:\s*(?P<msg>.+)$"
)

EXPECTED_ERROR = re.compile(
    r'// error(?:\[(?P<compilers>[^\]]*)\])?(?P<dedup>\^)?(?:x(?P<count>\d+))?:(?P<pattern>.+)'
)

ANSI_SEQUENCE = re.compile(r'\x1b\[[0-9;]*[A-Za-z]')
EXPECTED_DIAG_REGEX_BLOCK = re.compile(r'\{\{(.*?)\}\}')

def main() -> None:
    raw_compiler_id, raw_frontend_compiler_id = sys.argv[1:3]
    compiler_args = sys.argv[3:]

    cxx_compiler_id = CxxCompilerId.from_cmake(raw_compiler_id, raw_frontend_compiler_id)

    print(f'''Info:
    Compiler: {raw_compiler_id}
    Frontend Compiler: {raw_frontend_compiler_id}''', flush=True)

    sources: list[Path] = extract_sources(compiler_args)

    print(f'running compiler with args: {compiler_args!r}', flush=True)

    compiler_env = os.environ | { 'VSLANG': '1033' } # force msvc to print english messages
    if cxx_compiler_id != CxxCompilerId.MSVC:
        compiler_args.append(f'-fmax-errors=0') # remove limit on the number of errors (msvc limit is hardcoded on 100)

    compiler_proc = subprocess.run(compiler_args, capture_output=True, text=True, env=compiler_env)
    compiler_diag: list[CompilerDiag] = parse_compiler_output(compiler_proc.stdout + compiler_proc.stderr, cxx_compiler_id)
    # print(compiler_proc.stdout + compiler_proc.stderr)

    mismatches: list[str] = []

    has_errors: bool = False
    total_diags: int = 0

    for source in sources:
        expected_diags = parse_expected_diags(source, cxx_compiler_id)
        for diag_pattern in expected_diags:
            for i, diag in enumerate(compiler_diag):
                if diag_pattern.pattern.search(diag.msg):
                    del compiler_diag[i]
                    total_diags += 1
                    break
            else:
                mismatches.append(f'unmatched error \"{diag_pattern.raw}\" at {source}:{diag_pattern.line}')
        mismatches.extend(f'uncaught error: \"{msg.msg}\" at {msg.file}:{msg.line}' for msg in compiler_diag)

        if len(mismatches) > 0:
            print(f"- FAIL {source}", file=sys.stderr)
            print("\n".join(f"  - {p}" for p in mismatches), file=sys.stderr)
            has_errors = True
        else:
            print(f'- PASS {source}')

    sys.stdout.flush()
    if has_errors:
        print(f'Compiler diagnostic verification failed', file=sys.stderr)
        sys.exit(1)
    else:
        print(f'Compiler diagnostic verification succeded; {total_diags} diagnostics checked in {len(sources)} files...')

class CxxCompilerId(enum.Enum):
    MSVC = 1
    CLANG = 2
    GCC = 3
    CLANG_CL = 4

    @classmethod
    def from_cmake(cls, id: str, frontend_id: str) -> CxxCompilerId:
        # https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
        # https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_FRONTEND_VARIANT.html
        match id:
            case 'MSVC': return CxxCompilerId.MSVC
            case _ if 'Clang' in id:
                return CxxCompilerId.CLANG_CL if frontend_id == 'MSVC' else CxxCompilerId.CLANG
            case 'GNU':
                return CxxCompilerId.GCC
            case _:
                raise ValueError(f"Unknown C++ compiler ID: {id} (frontend ID: {frontend_id})")

    def readable_name(self) -> str:
        match self:
            case CxxCompilerId.MSVC: return 'msvc'
            case CxxCompilerId.CLANG | CxxCompilerId.CLANG_CL: return 'clang'
            case CxxCompilerId.GCC: return 'gcc'

@dataclass(frozen=True)
class CompilerDiag:
    file: str
    line: str
    msg: str

@dataclass
class ExpectedDiag:
    raw: str
    pattern: re.Pattern[str]
    line: int

def check_compiler_guard(guard_compilers: str | None, compiler_id: CxxCompilerId) -> bool:
    if guard_compilers is None:
        return True
    guard_compilers = guard_compilers.strip()

    if guard_compilers.startswith('!'):
        return guard_compilers[1:] != compiler_id.readable_name()
    
    return compiler_id.readable_name() in map(str.strip, guard_compilers.split(','))

def compile_diag_pattern(pattern: str) -> re.Pattern[str]:
    parts = EXPECTED_DIAG_REGEX_BLOCK.split(pattern)
    # re.split with one capturing group yields
    # [literal, raw, literal, raw, ..., literal]
    return re.compile(''.join(part if i % 2 else re.escape(part) for i, part in enumerate(parts)))

def parse_expected_diags(src: Path, compiler_id: CxxCompilerId) -> list[ExpectedDiag]:
    error_patterns: list[ExpectedDiag] = []
    for i, line in enumerate(src.open(encoding='utf-8'), start=1):
        m = EXPECTED_ERROR.search(line)
        if m is None:
            continue
        compilers, pattern = m.group('compilers'), m.group('pattern').strip()
        merge_with_prev = m.group('dedup') is not None
        raw_count = m.group('count')
        count = int(raw_count) if raw_count is not None else 1

        if not check_compiler_guard(compilers, compiler_id):
            continue

        # for some reason, msvc suppresses repeated diagnostics so we need to also deduplicate them to match with msvc's output
        if compiler_id == CxxCompilerId.MSVC and merge_with_prev and len(error_patterns) > 0 and error_patterns[-1].raw == pattern:
            continue

        for _ in range(count):
            error_patterns.append(ExpectedDiag(raw=pattern, pattern=compile_diag_pattern(pattern), line=i))
    return error_patterns

def parse_compiler_output(output: str, compiler_id: CxxCompilerId) -> list[CompilerDiag]:
    diags: list[CompilerDiag] = []
    for raw_line in output.splitlines():
        line = ANSI_SEQUENCE.sub('', raw_line)
        if compiler_id == CxxCompilerId.MSVC:
            m = MSVC_DIAG.match(line)
        else:
            m = CLANG_GCC_DIAG.match(line)

        if m is None:
            continue
        diags.append(CompilerDiag(file=m.group('file'), line=m.group('line'), msg=m.group('msg')))
    return diags

def extract_sources(args: list[str]) -> list[Path]:
    sources: list[Path] = []
    for arg in args:
        if arg.endswith(('.cpp', '.cxx', '.cc', '.c')) and Path(arg).is_file():
            sources.append(Path(arg))
    return sources

if __name__ == '__main__':
    main()
