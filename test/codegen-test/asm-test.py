import sys
from pathlib import Path
import enum
from typing import TextIO
import re
from dataclasses import dataclass
import itertools

TARGET_ASM_PREFIX = "//'"

cxx_compiler_id: 'CxxCompilerId | None' = None

def main() -> None:
    global cxx_compiler_id
    raw_compiler_id, raw_frontend_compiler_id, compiler_version, stdlib_id, raw_stdlib_version, build_config, target_os = sys.argv[1].strip().split(';')

    print(f'''Info:
    Build Configuration: {build_config}
    Compiler: {raw_compiler_id}
    Frontend Compiler: {raw_frontend_compiler_id}
    Compiler Version: {compiler_version}
    C++ Standard Library: {stdlib_id}
    C++ Standard Library Version: {raw_stdlib_version}
    Target OS: {target_os}''', flush=True)

    if build_config != 'Release':
        print(f'Skipping codegen test in {build_config} configuration...')
        return

    cxx_compiler_id = get_cxx_compiler_id(raw_compiler_id, raw_frontend_compiler_id)
    obj_files: list[str] = sys.argv[2].split(';')
    src_files: list[str] = sys.argv[3].split(';')
    build_dir: Path = Path(sys.argv[4].strip())

    normalized_asm: dict[str, list[str]] = {}
    for obj_file in obj_files:
        asm_file: Path = get_asm_file(obj_file, build_dir)
        with asm_file.open() as f:
            parsed_asm: dict[str, list[str]] = parse_asm(f)
        normalized_asm.update({ func_name: normalize_asm(insts) for func_name, insts in parsed_asm.items() })

    expr_symbols: dict[str, Symbol] = get_expression_symbols(compiler_version, stdlib_id, raw_stdlib_version, target_os)

    has_errors: bool = False
    total_functions: int = 0
    for src_file_str in src_files:
        src_file: Path = Path(src_file_str.strip())
        with src_file.open() as f:
            parsed_target_asm: dict[SourceLine, list[SourceLine]] = parse_target_asm(f)
        
        preprocessed_target_asm: dict[SourceLine, list[SourceLine]] = {}
        has_preprocessor_error = False
        for func_name, insts in parsed_target_asm.items():
            try:
                preprocessed_target_asm[func_name] = preprocess_target_asm(insts, expr_symbols)
            except PreprocessorError as e:
                print(f'At {src_file.name}:{func_name.number}, "{func_name.text}":\n  {e}', file=sys.stderr)
                has_errors = True
                has_preprocessor_error = True
        if has_preprocessor_error:
            continue

        for func_name, expected_lines in preprocessed_target_asm.items():
            actual_lines: list[str] | None = normalized_asm.get(func_name.text)
            if actual_lines is None:
                print(f'Error: function "{func_name}" expected but not found in generated assembly', file=sys.stderr)
                has_errors = True
                continue
            if not compare_disassembly(func_name, src_file, expected_lines, actual_lines):
                has_errors = True
            total_functions += 1

    if has_errors:
        print(f'Codegen test failed', file=sys.stderr)
        sys.exit(1)
    else:
        print(f'Codegen test successful; {total_functions} functions checked in {len(src_files)} files...')

class CxxCompilerId(enum.Enum):
    MSVC = 1
    CLANG = 2
    GCC = 3
    CLANG_CL = 4

@dataclass(frozen=True)
class SourceLine:
    number: int
    text: str

Version = tuple[int, int, int, int]
Symbol = Version | bool | None

class Lexer:
    def __init__(self, text: str):
        self._tokens: list[str] = re.findall(r'([!|&\(\)]|true|false|\$[\w\-]+|\[[^\]]+\])', text)
        self._current: int = 0
    def next(self) -> str | None:
        if self._current >= len(self._tokens):
            return None
        token: str = self._tokens[self._current]
        self._current += 1
        return token
    def peek(self) -> str | None:
        if self._current >= len(self._tokens):
            return None
        return self._tokens[self._current]
    def remaining(self) -> list[str]:
        return self._tokens[self._current:]

def strip_asm_comments(line: str) -> str:
    if cxx_compiler_id == CxxCompilerId.MSVC:
        return line.partition(';')[0].strip()
    else:
        return line.partition('#')[0].strip()

def get_cxx_compiler_id(raw_id: str, raw_frontend_id: str) -> CxxCompilerId:
    # https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
    if raw_id == 'MSVC':
        return CxxCompilerId.MSVC
    elif 'Clang' in raw_id:
        if raw_frontend_id == 'MSVC':
            return CxxCompilerId.CLANG_CL
        return CxxCompilerId.CLANG
    elif raw_id == 'GNU':
        return CxxCompilerId.GCC
    else:
        assert False, f"Unknown C++ compiler ID: {raw_id} (frontend ID: {raw_frontend_id})"

def get_asm_file(obj_file: str, build_dir: Path) -> Path:
    if cxx_compiler_id == CxxCompilerId.MSVC or cxx_compiler_id == CxxCompilerId.CLANG_CL:
        return build_dir / Path(obj_file).with_suffix('').with_suffix('.asm').name
    elif cxx_compiler_id == CxxCompilerId.CLANG:
        return Path(obj_file).with_suffix('').with_suffix('.s')
    elif cxx_compiler_id == CxxCompilerId.GCC:
        return Path(obj_file).with_suffix('.s')
    else:
        assert False, f"Unexpected C++ compiler ID: {cxx_compiler_id}"

def demangle_msvc(line: str) -> str:
    assert line[0] == '?'
    rest = line[1:]
    return rest[:rest.index('@')]

def demangle_itanium(mangled: str) -> str:
    rest = mangled[2:]
    i = 0
    while i < len(rest) and rest[i].isdigit():
        i += 1
    assert i != 0, "expected length of symbol"
    name_start = i
    name_end = name_start + int(rest[:i])
    assert name_end <= len(rest), "symbol name end is out of range"

    return rest[name_start:name_end]

def parse_asm(file: TextIO) -> dict[str, list[str]]:
    parsed_functions: dict[str, list[str]] = {}
    current_instructions: list[str] | None = None
    for line in file:
        if cxx_compiler_id == CxxCompilerId.MSVC and line.startswith('?'):
            if current_instructions is None: # function beginning (PROC)
                current_instructions = []
                parsed_functions[demangle_msvc(line)] = current_instructions
            else: # function ending (ENDP)
                current_instructions = None
            continue
        if cxx_compiler_id == CxxCompilerId.CLANG or cxx_compiler_id == CxxCompilerId.CLANG_CL:
            POSSIBLE_FUNCTION_END_DIRECTIVES = ('.section', '.globl', '.def', '.addrsig', '.ident')
            if current_instructions is not None and any(line.lstrip().startswith(d) for d in POSSIBLE_FUNCTION_END_DIRECTIVES):
                current_instructions = None
                continue

            if cxx_compiler_id == CxxCompilerId.CLANG_CL:
                if line.startswith('"?'):
                    current_instructions = []
                    # skip "
                    parsed_functions[demangle_msvc(line[1:])] = current_instructions
                    continue
            elif cxx_compiler_id == CxxCompilerId.CLANG:
                if line.startswith('_Z'):
                    if len(line) >= 3 and line[2].isdigit():
                        current_instructions = []
                        parsed_functions[demangle_itanium(line)] = current_instructions
                    else:
                        current_instructions = None
                    continue
        if cxx_compiler_id == CxxCompilerId.GCC and line.startswith('"_Z') and line.endswith('":\n'):
            if len(line) >= 4 and line[3].isdigit():
                current_instructions = []
                parsed_functions[demangle_itanium(line[1:-2])] = current_instructions
            else:
                current_instructions = None
            continue
    
        if current_instructions is None:
            continue
            
        stripped_line = strip_asm_comments(line)
        if stripped_line:
            current_instructions.append(stripped_line)
    return parsed_functions

REMOVE_DIRECTIVES: set[str] = {
    '.cfi_startproc', '.cfi_endproc', '.size', '.ident', '.section', '.p2align', '.globl', '.type', '.addrsig',
    '.def', '.scl', '.endef', '.ascii', '.seh_proc', '.loc', '.seh_stackalloc', '.seh_endprologue', '.seh_startepilogue',
    '.seh_endepilogue', '.seh_endproc', '.seh_pushreg', '.file', '.asciz', '.short', '.long', '.byte',
    '.linkonce', '.align', '.cfi_def_cfa_offset'
}

def normalize_asm(insts: list[str]) -> list[str]:
    new_insts: list[str] = []
    for inst in insts:
        new_inst = inst.replace('\t', ' ')
        if new_inst.partition(' ')[0] in REMOVE_DIRECTIVES:
            continue

        if cxx_compiler_id == CxxCompilerId.MSVC:
            # jg SHORT .L0 -> jg .L0
            new_inst = re.sub(r'^(j[a-z]{1,2}) (?:SHORT|FAR|NEAR) (.+)', r'\1 \2', new_inst)
            # ret 0 -> ret
            if new_inst == 'ret 0': new_inst = 'ret' 
            if new_inst == 'int 3': new_inst = 'int3'
        if cxx_compiler_id in (CxxCompilerId.MSVC, CxxCompilerId.GCC):
            # [rcx+rax*4+8] -> [rcx + rax*4 + 8]
            new_inst = re.sub(r'\[(.+)\]', lambda m: f'[{m[1].replace("+", " + ").replace("-", " - ")}]', new_inst)
            # mov DWORD PTR [rcx+rax*4+8], 6 -> mov	dword ptr [rcx+rax*4+8], 6
            new_inst = re.sub(r'(BYTE|WORD|DWORD|QWORD|XMMWORD|YMMWORD|ZMMWORD) PTR', lambda m: m[0].lower(), new_inst)
        if cxx_compiler_id == CxxCompilerId.GCC:
            # 8[rdi + rdx*4] -> [rdi + rdx*4 + 8]
            new_inst = re.sub(r'(-)?(\d+)\[(.+)\]', lambda m: f'[{m[3]} {m[1] or "+"} {m[2]}]', new_inst)
        if cxx_compiler_id in (CxxCompilerId.CLANG, CxxCompilerId.CLANG_CL):
            # [rdi + 4*rax + 8] -> [rdi + rax*4 + 8]
            new_inst = re.sub(r'(\d)\*([a-z0-9]+)', r'\2*\1', new_inst)
        if cxx_compiler_id in (CxxCompilerId.GCC, CxxCompilerId.CLANG_CL):
            # call "function" -> call function
            new_inst = re.sub(r'^((?:call|jmp|j[a-z]{1,4})) "([^"]+)"$', r'\1 \2', new_inst)
        if cxx_compiler_id != CxxCompilerId.MSVC:
            # movabs rax, 12345 -> mov rax, 12345
            new_inst = f'mov {new_inst[len("movabs "):]}' if new_inst.startswith('movabs ') else new_inst
        # movzx eax, byte ptr [rdi] -> movzx eax, [rdi]
        new_inst = re.sub(r'^([a-z]\w* [a-z]\w*), (?:byte|word|dword|qword|xmmword|ymmword|zmmword) ptr (\[.+\])$', r'\1, \2', new_inst, flags=re.IGNORECASE)
        # mov byte ptr [rdi], al -> mov [rdi], al
        new_inst = re.sub(r'^([a-z]\w*) (?:byte|word|dword|qword|xmmword|ymmword|zmmword) ptr (\[.+\]), ([a-z]\w*)$', r'\1 \2, \3', new_inst, flags=re.IGNORECASE)

        new_insts.append(new_inst)

    label_pattern = r'\$L[LN][\w@]+' if cxx_compiler_id == CxxCompilerId.MSVC else r'\.L\w+'

    # remove leading label
    if len(new_insts) > 0 and re.fullmatch(f'{label_pattern}:', new_insts[0]):
        del new_insts[0]
    # remove trailing label
    if len(new_insts) > 0 and re.fullmatch(f'{label_pattern}:', new_insts[-1]):
        del new_insts[-1]

    label_map: dict[str, str] = {}
    label_idx: int = 0
    def label_repl(m: re.Match[str]) -> str:
        nonlocal label_idx

        old_label = m.group(0)
        if old_label not in label_map:
            label_map[old_label] = f'.L{label_idx}'
            label_idx += 1
        return label_map[old_label]

    new_insts = [re.sub(label_pattern, label_repl, inst) for inst in new_insts]
    return new_insts

def parse_target_asm(file: TextIO) -> dict[SourceLine, list[SourceLine]]:
    parsed_functions: dict[SourceLine, list[SourceLine]] = {}
    current_statements: list[SourceLine] | None = None
    for i, line in enumerate(file, start=1):
        if not line.startswith(TARGET_ASM_PREFIX):
            continue
        line = line[len(TARGET_ASM_PREFIX):].strip()
        func_match = re.fullmatch(r'\[(\w+)\]', line)
        if func_match is not None:
            current_statements = []
            parsed_functions[SourceLine(i, func_match[1])] = current_statements
        elif current_statements is not None:
            current_statements.append(SourceLine(i, line))

    return parsed_functions

def parse_version_str(version_str: str) -> Version:
    if not version_str:
        sys.exit('Empty version in version constraint')
    parts = version_str.split('.')
    if len(parts) > 4:
        sys.exit(f'Too many version components in "{version_str}" (max 4)')
    result: list[int] = []
    for part in parts:
        if not part.isdigit():
            sys.exit(f'Invalid version component "{part}" in "{version_str}"')
        result.append(int(part))
    while len(result) < 4:
        result.append(0)
    return (result[0], result[1], result[2], result[3])

def check_version_constraint(version: Version, constraint_str: str) -> bool:
    constraint_str = constraint_str.strip()
    if not constraint_str:
        sys.exit('Empty version constraint')

    if '..' in constraint_str:
        start_str, _, end_str = constraint_str.partition('..')
        inclusive_end = end_str.startswith('=')
        if inclusive_end:
            end_str = end_str[1:]
        start_str = start_str.strip()
        end_str = end_str.strip()

        if not start_str and not end_str:
            sys.exit(f'Invalid version range: "{constraint_str}"')

        if start_str:
            if version < parse_version_str(start_str):
                return False
        if end_str:
            end = parse_version_str(end_str)
            if inclusive_end:
                if version > end:
                    return False
            else:
                if version >= end:
                    return False
        return True
    
    # legacy
    if constraint_str[:2] in ('==', '>=', '<='):
        operator = constraint_str[:2]
        version_part = constraint_str[2:]
    elif constraint_str[0] in ('<', '>'):
        operator = constraint_str[:1]
        version_part = constraint_str[1:]
    else:
        operator = '=='
        version_part = constraint_str

    for actual, expected_str in itertools.zip_longest(version, version_part.split('.'), fillvalue='0'):
        if expected_str == '*':
            break
        actual = int(actual)
        expected = int(expected_str)
        if operator == '==':
            if actual != expected: return False
        elif operator == '>':
            if actual > expected: return True
            if actual < expected: return False
        elif operator == '>=':
            if actual > expected: return True
            if actual < expected: return False
        elif operator == '<':
            if actual < expected: return True
            if actual > expected: return False
        elif operator == '<=':
            if actual < expected: return True
            if actual > expected: return False
    return operator in ('==', '>=', '<=')

def eval_expr(lexer: Lexer, symbols: dict[str, Symbol], *, min_bp: int = 0) -> bool:
    # uses Pratt parsing
    lhs_token = lexer.next()
    if lhs_token is None:
        sys.exit('Unexpected EOF')

    if lhs_token == '(':
        lhs = eval_expr(lexer, symbols, min_bp=0)
        if lexer.next() != ')':
            sys.exit('Expected closing parenthesis ")"')
    elif (prefix_bp := prefix_binding_power(lhs_token)) is not None:
        rhs = eval_expr(lexer, symbols, min_bp=prefix_bp)
        lhs = eval_prefix_op(lhs_token, rhs)
    elif lhs_token == 'true':
        lhs = True
    elif lhs_token == 'false':
        lhs = False
    elif lhs_token.startswith('$'):
        symbol = symbols[lhs_token]

        constraint_token = lexer.peek()
        has_constraint = constraint_token is not None and constraint_token.startswith('[') and constraint_token.endswith(']')
        if has_constraint:
            lexer.next() # advace past [..]

        if symbol is None:
            lhs = False
        elif isinstance(symbol, bool):
            lhs = symbol
        elif has_constraint:
            lhs = check_version_constraint(symbol, constraint_token[1:-1].strip())
        else:
            lhs = True
    else:
        sys.exit(f'Invalid token: "{lhs_token}"')

    while True:
        op = lexer.peek()
        if op is None: break

        if (binding_power := infix_binding_power(op)) is not None:
            left_bp, right_bp = binding_power
            if left_bp < min_bp:
                break
            lexer.next() # consume operator
            rhs = eval_expr(lexer, symbols, min_bp=right_bp)
            lhs = eval_op(lhs, rhs, op)
            continue
        break
    return lhs

def infix_binding_power(op: str) -> tuple[int, int] | None:
    if op == '&':
        return (3, 4)
    if op == '|':
        return (1, 2)
    return None
def eval_op(lhs: bool, rhs: bool, op: str) -> bool:
    if op == '&':
        return lhs and rhs
    if op == '|':
        return lhs or rhs
    sys.exit(f'Bad operator: "{op}"')
def prefix_binding_power(op: str) -> int | None:
    if op == '!':
        return 5
    return None
def eval_prefix_op(op: str, operand: bool) -> bool:
    if op == '!':
        return not operand
    sys.exit(f'Bad prefix operator: "{op}"')

class PreprocessorError(Exception):
    pass

def preprocess_target_asm(insts: list[SourceLine], symbols: dict[str, Symbol]) -> list[SourceLine]:
    processed_lines: list[SourceLine] = []

    emit = True
    matched_if_lines: list[SourceLine] = []

    i = 0
    while i < len(insts):
        line = insts[i]
        text = line.text

        if text.startswith('@'):
            if text.startswith('@if '):
                expr_text = text[len('@if '):]
                emit = eval_expr(Lexer(expr_text), symbols)
                if emit:
                    matched_if_lines.append(line)
            elif text.startswith('@error'):
                if emit:
                    err_msg = text[len('@error '):].strip()
                    if len(err_msg) == 0:
                        raise PreprocessorError(f'Preprocessor error at line {line.number}')
                    else:
                        raise PreprocessorError(f'Preprocessor error (line {line.number}): {err_msg}')
            else:
                raise PreprocessorError(f'Unknown preprocessor directive at line {line.number}: "{text}"')
            i += 1
            continue

        if emit:
            processed_lines.append(line)
        i += 1

    if len(matched_if_lines) > 1:
        matched_desc = '\n'.join(f'    line {l.number}: {l.text}' for l in matched_if_lines)
        raise PreprocessorError(f'Multiple @if conditions are true. Only one @if block may be active. Matched @if conditions:\n{matched_desc}')

    return processed_lines

def parse_stdlib_version(stdlib_id: str, raw: str) -> Version:
    if stdlib_id == 'libc++':
        # _LIBCPP_VERSION = major*10000 + minor*100 + patch
        v = int(raw)
        return (v // 10000, (v // 100) % 100, v % 100, 0)

    if stdlib_id == 'libstdc++':
        # _GLIBCXX_RELEASE = major
        return (int(raw), 0, 0, 0)

    if stdlib_id == 'stl':
        # _MSVC_STL_VERSION = major*10 + minor
        v = int(raw)
        return (v // 10, v % 10, 0, 0)

    assert False, f'Unknown C++ standard library implementation: "{stdlib_id}"'

def get_expression_symbols(compiler_version_str: str, stdlib_id: str, raw_stdlib_version: str, target_os: str) -> dict[str, Symbol]:
    symbols: dict[str, Symbol] = {}

    ver_parts = compiler_version_str.split('.')
    comp_ver = (
        int(ver_parts[0]),
        int(ver_parts[1]) if len(ver_parts) > 1 else 0,
        int(ver_parts[2]) if len(ver_parts) > 2 else 0,
        int(ver_parts[3]) if len(ver_parts) > 3 else 0
    )

    symbols['$clang'] = comp_ver if cxx_compiler_id == CxxCompilerId.CLANG else None
    symbols['$msvc'] = comp_ver if cxx_compiler_id == CxxCompilerId.MSVC else None
    symbols['$clang-cl'] = comp_ver if cxx_compiler_id == CxxCompilerId.CLANG_CL else None
    symbols['$gcc'] = comp_ver if cxx_compiler_id == CxxCompilerId.GCC else None

    stdlib_version = parse_stdlib_version(stdlib_id, raw_stdlib_version)

    symbols['$stl'] = stdlib_version if stdlib_id == 'stl' else None
    symbols['$libstdcxx'] = stdlib_version if stdlib_id == 'libstdc++' else None
    symbols['$libcxx'] = stdlib_version if stdlib_id == 'libc++' else None

    symbols['$windows'] = target_os == 'Windows'
    symbols['$linux'] = target_os == 'Linux'
    symbols['$macos'] = target_os == 'Darwin'
    symbols['$android'] = target_os == 'Android'
    symbols['$ios'] = target_os == 'iOS'

    symbols['$ms-abi'] = target_os == 'Windows'
    symbols['$sysv-abi'] = target_os != 'Windows'

    return symbols

def compare_disassembly(func_name: SourceLine, src_file: Path, expected: list[SourceLine], actual: list[str]) -> bool:
    expected_texts = [line.text for line in expected]

    if expected_texts == actual:
        return True
    
    print(f'{func_name.text}: ({src_file.name}:{func_name.number})')
    
    for line in actual:
        print(f'+ {line}')
    print()
    for line in expected_texts:
        print(f'- {line}')

    if len(expected_texts) > 0:
        print()

    return False

if __name__ == '__main__':
    main()
