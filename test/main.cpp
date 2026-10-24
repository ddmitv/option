// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdio>
#include <cstdlib>

#include "tests.hpp"

void on_check_failed(const char* expr, const char* filename, unsigned int line, const char* function) {
    std::fprintf(stderr, "check failed: (%s)\n", expr);
    std::fprintf(stderr, "  in function: %s\n", function);
    std::fprintf(stderr, "  at %s:%u\n", filename, line);
    std::fflush(stderr);
}

#define run_test_suite(suite) \
    void suite(); \
    std::puts("> " #suite); \
    std::fflush(stdout); \
    suite()

int main() {
    run_test_suite(main_test_suite);
    run_test_suite(lifetime_test_suite);
    run_test_suite(none_test_suite);
    run_test_suite(comp_with_none_test_suite);
    run_test_suite(comp_with_t_test_suite);
    run_test_suite(relops_test_suite);

    std::puts("Tests successfully completed");
}
