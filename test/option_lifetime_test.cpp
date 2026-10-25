// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <opt/option.hpp>
#include <utility>
#include <cstdio>

#include "tests.hpp"

OPTION_SUPPRESS_COMMON_WARNINGS_PUSH

namespace {

struct counted {
    static inline int ctor = 0;
    static inline int default_ctor = 0;
    static inline int copy_ctor = 0;
    static inline int move_ctor = 0;
    static inline int copy_assign = 0;
    static inline int move_assign = 0;
    static inline int dtor = 0;

    int value = 0;

    counted() {
        ++default_ctor;
    }
    counted(int v) : value{v} {
        ++ctor;
    }
    counted(const counted& o) : value{o.value} {
        ++copy_ctor;
    }
    counted(counted&& o) noexcept : value{o.value} {
        ++move_ctor;
    }
    ~counted() {
        ++dtor;
    }

    counted& operator=(const counted& o) {
        value = o.value;
        ++copy_assign;
        return *this;
    }
    counted& operator=(counted&& o) noexcept {
        value = o.value;
        ++move_assign;
        return *this;
    }

    static void reset() {
        ctor = 0;
        default_ctor = 0;
        copy_ctor = 0;
        move_ctor = 0;
        copy_assign = 0;
        move_assign = 0;
        dtor = 0;
    }
};

struct check_counted_guard {
    const char* file;
    unsigned int line;
    const char* function;

    int exp_ctor;
    int exp_default_ctor;
    int exp_copy_ctor;
    int exp_move_ctor;
    int exp_copy_assign;
    int exp_move_assign;
    int exp_dtor;

    check_counted_guard(const char* f OPTION_LIFETIMEBOUND, unsigned int l, const char* fn OPTION_LIFETIMEBOUND, int ec, int edc, int ecc, int emc, int eca, int ema, int ed) noexcept
        : file{f}, line{l}, function{fn}
        , exp_ctor{ec}, exp_default_ctor{edc}, exp_copy_ctor{ecc}, exp_move_ctor{emc}
        , exp_copy_assign{eca}, exp_move_assign{ema}, exp_dtor{ed}
    {
        counted::reset();
    }

    ~check_counted_guard() {
        if (counted::ctor != exp_ctor || counted::default_ctor != exp_default_ctor || counted::copy_ctor != exp_copy_ctor
            || counted::move_ctor != exp_move_ctor || counted::copy_assign != exp_copy_assign || counted::move_assign != exp_move_assign || counted::dtor != exp_dtor)
        {
            std::fprintf(stderr, "check_counted failed:\n");
            std::fprintf(stderr, "  ctor         expected %d, got %d\n", exp_ctor, counted::ctor);
            std::fprintf(stderr, "  default_ctor expected %d, got %d\n", exp_default_ctor, counted::default_ctor);
            std::fprintf(stderr, "  copy_ctor    expected %d, got %d\n", exp_copy_ctor, counted::copy_ctor);
            std::fprintf(stderr, "  move_ctor    expected %d, got %d\n", exp_move_ctor, counted::move_ctor);
            std::fprintf(stderr, "  copy_assign  expected %d, got %d\n", exp_copy_assign, counted::copy_assign);
            std::fprintf(stderr, "  move_assign  expected %d, got %d\n", exp_move_assign, counted::move_assign);
            std::fprintf(stderr, "  dtor         expected %d, got %d\n", exp_dtor, counted::dtor);
            std::fflush(stderr);
            ::on_check_failed("check_counted", file, line, function);
            OPTION_DEBUG_BREAK;
        }
        counted::reset();
    }

    check_counted_guard(const check_counted_guard&) = delete;
    check_counted_guard& operator=(const check_counted_guard&) = delete;
};

#define check_counted(ctor, default_ctor, copy_ctor, move_ctor, copy_assign, move_assign, dtor) \
    if (check_counted_guard _check_counted_guard_{ __FILE__, __LINE__, CURRENT_FUNCTION, ctor, default_ctor, copy_ctor, move_ctor, copy_assign, move_assign, dtor}; true)

void constructors() {
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        opt::option<counted> o;
    }
    check_counted(1, 0, 0, 0, 0, 0, 1) {
        opt::option<counted> o{1};
    }
    check_counted(1, 0, 0, 1, 0, 0, 2) {
        opt::option<counted> o{counted{2}};
    }
    check_counted(0, 1, 0, 1, 0, 0, 2) {
        opt::option<counted> o{counted{}};
    }
    check_counted(0, 1, 0, 0, 0, 0, 1) {
        opt::option<counted> o{std::in_place};
    }

    opt::option<counted> other;

    other.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        opt::option<counted> o{other};
    }
    other.emplace();
    check_counted(0, 0, 1, 0, 0, 0, 1) {
        opt::option<counted> o{other};
    }
    other.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        opt::option<counted> o{std::move(other)};
    }
    other.emplace();
    check_counted(0, 0, 0, 1, 0, 0, 1) {
        opt::option<counted> o{std::move(other)};
    }
}

void assignment_operators() {
    opt::option<counted> mut;

    mut.reset();
    check_counted(1, 0, 0, 1, 0, 0, 1) {
        mut = counted{1};
    }
    mut.reset();
    check_counted(1, 0, 0, 1, 0, 0, 1) {
        mut = counted{1};
    }
    mut.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        mut = opt::option<counted>{};
    }
    mut.reset();
    check_counted(1, 0, 0, 1, 0, 0, 1) {
        mut = opt::option<counted>{1};
    }
    mut.emplace();
    check_counted(0, 0, 0, 0, 0, 0, 1) {
        mut = opt::option<counted>{};
    }
    mut.emplace();
    check_counted(1, 0, 0, 0, 0, 1, 1) {
        mut = opt::option<counted>{1};
    }

    counted some_value;

    mut.reset();
    check_counted(0, 0, 1, 0, 0, 0, 0) {
        mut = some_value;
    }
    mut.emplace();
    check_counted(0, 0, 0, 0, 1, 0, 0) {
        mut = some_value;
    }
    mut.reset();
    check_counted(0, 0, 0, 1, 0, 0, 0) {
        mut = std::move(some_value);
    }
    mut.emplace();
    check_counted(0, 0, 0, 0, 0, 1, 0) {
        mut = std::move(some_value);
    }
}

void reset_method() {
    opt::option<counted> mut;

    mut.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        mut.reset();
    }
    mut.emplace();
    check_counted(0, 0, 0, 0, 0, 0, 1) {
        mut.reset();
    }
}

void emplace_method() {
    opt::option<counted> mut;

    mut.reset();
    check_counted(0, 1, 0, 0, 0, 0, 0) {
        mut.emplace();
    }
    mut.reset();
    check_counted(1, 0, 0, 0, 0, 0, 0) {
        mut.emplace(1);
    }
    mut.emplace();
    check_counted(0, 1, 0, 0, 0, 0, 1) {
        mut.emplace();
    }
    mut.emplace();
    check_counted(1, 0, 0, 0, 0, 0, 1) {
        mut.emplace(1);
    }
    counted some_value;

    mut.reset();
    check_counted(0, 0, 1, 0, 0, 0, 0) {
        mut.emplace(some_value);
    }
    mut.reset();
    check_counted(0, 0, 0, 1, 0, 0, 0) {
        mut.emplace(std::move(some_value));
    }
    mut.emplace();
    check_counted(0, 0, 1, 0, 0, 0, 1) {
        mut.emplace(some_value);
    }
    mut.emplace();
    check_counted(0, 0, 0, 1, 0, 0, 1) {
        mut.emplace(std::move(some_value));
    }
}

void std_exchange_with_option() {
    opt::option<counted> o1;
    opt::option<counted> o2;

    o1.reset();
    check_counted(1, 0, 0, 0, 0, 0, 0) {
        (void)std::exchange(o1, 1);
    }
    o1.emplace();
    check_counted(1, 0, 0, 1, 0, 1, 2) {
        (void)std::exchange(o1, 1);
    }
    o1.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        (void)std::exchange(o1, opt::option<counted>{});
    }
    o1.reset();
    check_counted(1, 0, 0, 1, 0, 0, 1) {
        (void)std::exchange(o1, opt::option<counted>{1});
    }
    o1.emplace();
    check_counted(0, 0, 0, 1, 0, 0, 2) {
        (void)std::exchange(o1, opt::option<counted>{});
    }

    o1.emplace();
    check_counted(1, 0, 0, 1, 0, 1, 2) {
        (void)std::exchange(o1, opt::option<counted>{1});
    }
    o2.reset(); o1.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        (void)std::exchange(o1, o2);
    }
    o2.emplace(1); o1.reset();
    check_counted(0, 0, 1, 0, 0, 0, 0) {
        (void)std::exchange(o1, o2);
    }
    o2.reset(); o1.emplace();
    check_counted(0, 0, 0, 1, 0, 0, 2) {
        (void)std::exchange(o1, o2);
    }
    o2.emplace(1); o1.emplace();
    check_counted(0, 0, 0, 1, 1, 0, 1) {
        (void)std::exchange(o1, o2);
    }
}

void swap_method() {
    opt::option<counted> o1;
    opt::option<counted> o2;

    o1.emplace(1); o2.emplace(2);
    check_counted(0, 0, 0, 1, 0, 2, 1) {
        o1.swap(o2);
    }
    o1.emplace(1); o2.reset();
    check_counted(0, 0, 0, 1, 0, 0, 1) {
        o1.swap(o2);
    }
    o1.reset(); o2.emplace(2);
    check_counted(0, 0, 0, 1, 0, 0, 1) {
        o1.swap(o2);
    }
    o1.reset(); o2.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        o1.swap(o2);
    }

    o1.emplace(1); // self-swap
    check_counted(0, 0, 0, 1, 0, 2, 1) {
        o1.swap(o1);
    }
    o1.reset(); // self-swap
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        o1.swap(o1);
    }

    o1.emplace(1); o2.emplace(2);
    check_counted(0, 0, 0, 1, 0, 2, 1) {
        std::swap(o1, o2);
    }
    o1.emplace(1); o2.reset();
    check_counted(0, 0, 0, 2, 0, 0, 2) {
        std::swap(o1, o2);
    }
    o1.reset(); o2.emplace(2);
    check_counted(0, 0, 0, 1, 0, 0, 1) {
        std::swap(o1, o2);
    }
    o1.reset(); o2.reset();
    check_counted(0, 0, 0, 0, 0, 0, 0) {
        std::swap(o1, o2);
    }

}

}

void lifetime_test_suite() {
    constructors();
    assignment_operators();

    reset_method();
    emplace_method();
    swap_method();

    std_exchange_with_option();
}

OPTION_SUPPRESS_COMMON_WARNINGS_POP
