# Copyright 2025 Dima Dmitriev.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import json
import subprocess
import logging
import os
import shutil
import time
import statistics
import argparse
import sys

CONTENT_NUM = 10_000
CMAKELISTS_TARGET_SETUP = r'''add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/option")
target_link_libraries(bench PRIVATE option)
target_compile_features(bench PRIVATE cxx_std_17)
'''

def main():
    arg_parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    arg_parser.add_argument('--compilers', type=str, default='compilers.json', metavar='PATH', help='Path to test compilers .json config')
    arg_parser.add_argument('--codegen', type=str, default='codegen.json', metavar='PATH', help='Path to test codegen .json config')
    arg_parser.add_argument('--cmake', type=str, default='cmake', metavar='PATH/COMMAND', help='Path to CMake executable/command')
    arg_parser.add_argument('--runs', type=int, default=5, metavar='NUM', help='Number of benchmark runs')
    arg_parser.add_argument('--warmup-runs', type=int, default=1, metavar='NUM', help='Number of benchmark warmup runs')
    arg_parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'], default='INFO', help='Select logging level')
    args = arg_parser.parse_args()

    logging.basicConfig(level=args.log_level, format='[%(levelname)s] %(message)s', handlers=[logging.FileHandler('output.log', mode='w'), logging.StreamHandler()])

    if args.runs <= 2:
        logging.critical('At least two benchmark runs are required')
        exit(1)

    logging.info(f'Reading compilers configuration: "{args.compilers}"')
    with open_or_exit(args.compilers, error_msg='Failed to open compilers configuration') as compilers_file:
        try:
            compilers_cfg = json.load(compilers_file)
        except json.JSONDecodeError as e:
            logging.critical(f'Failed to parse JSON (compilers configuration): {e}')
            exit(1)
    
    logging.info(f'Reading codegen configuration: "{args.codegen}"')
    with open_or_exit(args.codegen, error_msg='Failed to open codegen configuration') as codegen_file:
        try:
            codegen_cfg = json.load(codegen_file)
        except json.JSONDecodeError as e:
            logging.critical(f'Failed to parse JSON (codegen configuration): {e}')
            exit(1)

    cmake_exe = shutil.which(args.cmake)

    prepare_project(path='project')
    os.environ['CCACHE_DISABLE'] = '1'
    os.environ['CMAKE_BUILD_PARALLEL_LEVEL'] = '1'

    final_summary = []
    total_progress_step = 0
    total_progress_max_step = len(codegen_cfg) * len(compilers_cfg) * (args.warmup_runs + args.runs)
    script_start_time = time.monotonic()

    for codegen_idx, codegen in enumerate(codegen_cfg, start=1):
        logging.info(f'Current codegen config name: {codegen['name']} ({codegen_idx}/{len(codegen_cfg)})')
        filename = 'src.cpp'
        generate_source_file(name=filename, content_num=CONTENT_NUM,
            prologue=codegen.get('prologue', ''), content=codegen.get('content', ''),
            epilogue_prefix=codegen.get('epilogue_prefix', ''),
            epilogue_line_template=codegen.get('epilogue_line_template', ''),
            epilogue_suffix=codegen.get('epilogue_suffix', '')
        )

        create_cmake_lists(name='CMakeLists.txt', src=filename)

        for compiler_idx, compiler in enumerate(compilers_cfg, start=1):
            logging.info(f'Compiler: {compiler['name']} ({compiler_idx}/{len(compilers_cfg)})')
            cmake_gen_buildsystem(cmake_exe=cmake_exe, extra_args=compiler.get('cmake_gen_args', []))
            compiler_version = extract_compiler_version('build/CMakeCache.txt')
            cmake_build_args = compiler.get('cmake_build_args', [])

            for warmup_run_num in range(args.warmup_runs):
                total_progress_step += 1
                logging.info(f'Benchmark warmup run #{warmup_run_num + 1} (total progress: {total_progress_step}/{total_progress_max_step})')
                _ = run_benchmark_build(cmake_exe=cmake_exe, extra_args=cmake_build_args)

            compile_time_results = []
            for run_num in range(args.runs):
                total_progress_step += 1
                logging.info(f'Benchmark run #{run_num + 1} (total progress: {total_progress_step}/{total_progress_max_step})')

                elapsed = run_benchmark_build(cmake_exe=cmake_exe, extra_args=cmake_build_args)
                logging.info(f'Benchmark run #{run_num + 1} ended. Elapsed time: {elapsed} s')
                compile_time_results.append(elapsed)

            mean = statistics.mean(compile_time_results)
            std_dev = statistics.stdev(compile_time_results, mean)
            final_summary.append(f'{codegen['name']}, {compiler['name']} ({compiler_version}) | {mean:.4f} s \u00B1 {std_dev:.4f} s | {min(compile_time_results):.3f} s \u2026 {max(compile_time_results):.3f} s')

    logging.info(f'Script execution time: {script_start_time - time.monotonic()} s')

    print('\nBenchmark settings:\n'
          f'Compilers config: {args.compilers}\n'
          f'Codegen config: {args.codegen}\n'
          f'Benchmark runs: {args.runs}\n'
          f'Benchmark warmup runs: {args.warmup_runs}')
    print('\nBenchmark summary:')
    for summary_line in final_summary:
        print(summary_line)

def open_or_exit(file, mode='r', buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None, *, error_msg):
    try:
        return open(file, mode, buffering, encoding, errors, newline, closefd, opener)
    except IOError as e:
        sys.exit(f'{error_msg}: {e} (path: "{os.path.abspath(file)}")')


def rmdir_if_exists(path):
    while os.path.isdir(path):
        try:
            shutil.rmtree(path)
            logging.info(f'Removing "{path}" folder')
        except Exception as e:
            logging.warning(f'Failed to remove "{path}" folder: {repr(e)}. Retrying in 3 seconds...')
            time.sleep(3)

def prepare_project(path):
    rmdir_if_exists(path)
    
    if not os.path.isdir(path):
        logging.info(f'Creating "{path}" folder')
        os.makedirs(path)
    
    while not os.path.exists(path):
        time.sleep(seconds=0.1)

    logging.info(f'Changing current working directory to "{path}" folder')
    os.chdir(path)


def generate_source_file(name, content_num, prologue, content, epilogue_prefix, epilogue_line_template, epilogue_suffix):
    logging.info(f'Generating source file: "{name}")')
    with open_or_exit(name, 'w', error_msg=f'Failed to open benchmark source file "{name}"') as f:
        f.write(prologue + '\n')
        for i in range(content_num):
            f.write(content.replace('$', str(i)) + '\n')
        if epilogue_prefix:
            f.write(epilogue_prefix + '\n')
        if epilogue_line_template:
            for i in range(content_num):
                f.write(epilogue_line_template.replace('$', str(i)) + '\n')
        if epilogue_suffix:
            f.write(epilogue_suffix + '\n')

def create_cmake_lists(name, src):
    logging.info(f'Creating CMake configuration file: "{name}"')
    with open_or_exit(name, 'w', error_msg=f'Failed to open CMakeLists file') as f:
        f.write(
            'cmake_minimum_required(VERSION 3.24)\n'
            'project(bench)\n'
            f'add_executable(bench {src})\n'
            + CMAKELISTS_TARGET_SETUP +
            'set(_COMPTIME_BENCH_compiler_version ${CMAKE_CXX_COMPILER_VERSION} CACHE STRING "")\n'
        )

def cmake_gen_buildsystem(cmake_exe, extra_args):
    rmdir_if_exists('build')
    gen_args = [cmake_exe, '-B', 'build', '--fresh'] + extra_args
    logging.info(f'Running cmake project generation (args: {' '.join(gen_args)})')
    process = subprocess.run(gen_args)
    if process.returncode != 0:
        logging.critical(f'Failed to generate buildsystem. Exit code: {process.returncode}')
        exit(1)

def run_benchmark_build(cmake_exe, extra_args):
    logging.info('Cleaning build artifacts...')
    subprocess.run([cmake_exe, '--build', 'build', '--target', 'clean'])
    build_args = [cmake_exe, '--build', 'build'] + extra_args
    logging.info(f'Running benchmark build (args: {' '.join(build_args)})')

    start_time = time.monotonic()
    subprocess.run(build_args, check=True)
    end_time = time.monotonic()
    return end_time - start_time

def extract_compiler_version(cmake_cache_path):
    search_str_prefix = '_COMPTIME_BENCH_compiler_version:STRING='
    with open_or_exit(cmake_cache_path, 'r', error_msg='Failed to open CMake cache file') as file:
        for line in file:
            if line.startswith(search_str_prefix):
                return line[len(search_str_prefix):].strip()
    return ' [unavailable] '

if __name__ == '__main__':
    main()
