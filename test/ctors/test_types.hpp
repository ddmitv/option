#pragma once

namespace test_types {

struct no_ctors {
    no_ctors() = delete;
    no_ctors(no_ctors const&) = delete;
    no_ctors& operator=(no_ctors const&) = delete;
};

struct explicit_no_ctors {
    explicit explicit_no_ctors() = delete;
    explicit explicit_no_ctors(explicit_no_ctors const&) = delete;
    explicit_no_ctors& operator=(explicit_no_ctors const&) = delete;
};

struct noexcept_no_ctors {
    noexcept_no_ctors() noexcept = delete;
    noexcept_no_ctors(noexcept_no_ctors const&) noexcept = delete;
    noexcept_no_ctors& operator=(noexcept_no_ctors const&) noexcept = delete;
};

struct non_trivial_dtor_no_ctors {
    non_trivial_dtor_no_ctors() = delete;
    non_trivial_dtor_no_ctors(non_trivial_dtor_no_ctors const&) = delete;
    non_trivial_dtor_no_ctors& operator=(non_trivial_dtor_no_ctors const&) = delete;
    ~non_trivial_dtor_no_ctors() {}
};

}
