#pragma once

#include <initializer_list>
#include <tests.hpp>

namespace test_types {

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/support/archetypes.h
// https://github.com/llvm/llvm-project/blob/main/libcxx/test/support/archetypes.ipp

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

struct non_triv_dtor_no_ctors {
    non_triv_dtor_no_ctors() = delete;
    non_triv_dtor_no_ctors(non_triv_dtor_no_ctors const&) = delete;
    non_triv_dtor_no_ctors& operator=(non_triv_dtor_no_ctors const&) = delete;
    ~non_triv_dtor_no_ctors() {}
};

struct triv_test_type {
    constexpr triv_test_type() noexcept : value(0) {}

    constexpr triv_test_type(int x) : value(x) {}
    constexpr triv_test_type(int, int y) : value(y) {}
    constexpr triv_test_type(std::initializer_list<int>& il, int = 0)
        : value(static_cast<int>(il.size())) {}

    int value;
};

struct test_type {
    inline static int alive = 0;
    inline static int constructed = 0;
    inline static int value_constructed = 0;
    inline static int default_constructed = 0;
    inline static int copy_constructed = 0;
    inline static int move_constructed = 0;
    inline static int assigned = 0;
    inline static int value_assigned = 0;
    inline static int copy_assigned = 0;
    inline static int move_assigned = 0;
    inline static int destroyed = 0;

    static void reset() {
        check(alive == 0);
        alive = 0;
        reset_constructors();
    }

    static void reset_constructors() {
        constructed = 0;
        value_constructed = 0;
        default_constructed = 0;
        copy_constructed = 0;
        move_constructed = 0;
        assigned = 0;
        value_assigned = 0;
        copy_assigned = 0;
        move_assigned = 0;
        destroyed = 0;
    }

    test_type() noexcept : value(0) {
        ++alive;
        ++constructed;
        ++default_constructed;
    }
    test_type(int x) noexcept : value(x) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    test_type(int, int y) noexcept : value(y) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    explicit test_type(std::initializer_list<int>& il, int = 0) noexcept : value(static_cast<int>(il.size())) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }

    test_type(const test_type& o) noexcept : value(o.value) {
        check(o.value != -1);
        check(o.value != -999);
        ++alive;
        ++constructed;
        ++copy_constructed;
    }
    test_type(test_type&& o) noexcept : value(o.value) {
        check(o.value != -1);
        check(o.value != -999);
        ++alive;
        ++constructed;
        ++move_constructed;
        o.value = -1;
    }

    test_type& operator=(int xvalue) noexcept {
        value = xvalue;
        ++assigned;
        ++value_assigned;
        return *this;
    }
    test_type& operator=(const test_type& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        ++assigned;
        ++copy_assigned;
        value = o.value;
        return *this;
    }
    test_type& operator=(test_type&& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        ++assigned;
        ++move_assigned;
        value = o.value;
        o.value = -1;
        return *this;
    }

    ~test_type() {
        check(value != -999);
        check(alive > 0);
        --alive;
        ++destroyed;
        value = -999;
    }

    int value;
};

struct explicit_test_type {
    inline static int alive = 0;
    inline static int constructed = 0;
    inline static int value_constructed = 0;
    inline static int default_constructed = 0;
    inline static int copy_constructed = 0;
    inline static int move_constructed = 0;
    inline static int assigned = 0;
    inline static int value_assigned = 0;
    inline static int copy_assigned = 0;
    inline static int move_assigned = 0;
    inline static int destroyed = 0;

    static void reset() {
        check(alive == 0);
        alive = 0;
        reset_constructors();
    }

    static void reset_constructors() {
        constructed = 0;
        value_constructed = 0;
        default_constructed = 0;
        copy_constructed = 0;
        move_constructed = 0;
        assigned = 0;
        value_assigned = 0;
        copy_assigned = 0;
        move_assigned = 0;
        destroyed = 0;
    }

    explicit explicit_test_type() noexcept : value(0) {
        ++alive;
        ++constructed;
        ++default_constructed;
    }
    explicit explicit_test_type(int x) noexcept : value(x) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    explicit explicit_test_type(int, int y) noexcept : value(y) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }
    explicit explicit_test_type(std::initializer_list<int>& il, int = 0) noexcept
        : value(static_cast<int>(il.size())) {
        ++alive;
        ++constructed;
        ++value_constructed;
    }

    explicit explicit_test_type(const explicit_test_type& o) noexcept : value(o.value) {
        check(o.value != -1);
        check(o.value != -999);
        ++alive;
        ++constructed;
        ++copy_constructed;
    }
    explicit explicit_test_type(explicit_test_type&& o) noexcept : value(o.value) {
        check(o.value != -1);
        check(o.value != -999);
        ++alive;
        ++constructed;
        ++move_constructed;
        o.value = -1;
    }

    explicit_test_type& operator=(int xvalue) noexcept {
        value = xvalue;
        ++assigned;
        ++value_assigned;
        return *this;
    }
    explicit_test_type& operator=(const explicit_test_type& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        ++assigned;
        ++copy_assigned;
        value = o.value;
        return *this;
    }
    explicit_test_type& operator=(explicit_test_type&& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        ++assigned;
        ++move_assigned;
        value = o.value;
        o.value = -1;
        return *this;
    }

    ~explicit_test_type() {
        check(value != -999);
        check(alive > 0);
        --alive;
        ++destroyed;
        value = -999;
    }

    int value;
};

struct explicit_triv_test_type {
    explicit constexpr explicit_triv_test_type() noexcept : value(0) {}

    explicit constexpr explicit_triv_test_type(int x) : value(x) {}
    explicit constexpr explicit_triv_test_type(int, int y) : value(y) {}
    explicit constexpr explicit_triv_test_type(std::initializer_list<int>& il, int = 0) : value(static_cast<int>(il.size())) {}

    explicit constexpr explicit_triv_test_type(const explicit_triv_test_type&) = default;
    explicit constexpr explicit_triv_test_type(explicit_triv_test_type&&) = default;

    constexpr explicit_triv_test_type& operator=(const explicit_triv_test_type&) = default;
    constexpr explicit_triv_test_type& operator=(explicit_triv_test_type&&) = default;

    int value;
};

struct constexpr_test_type {
private:
    constexpr static int check_value(const int& val) {
        check(val != -1);
        check(val != 999);
        return val;
    }

    constexpr static int check_value(int& val, int val_cp = 0) {
        check(val != -1);
        check(val != 999);
        val_cp = val;
        val = -1;
        return val_cp;
    }

public:
    constexpr constexpr_test_type() noexcept : value(0) {}

    constexpr constexpr_test_type(int x) : value(x) {}
    constexpr constexpr_test_type(int, int y) : value(y) {}
    constexpr constexpr_test_type(std::initializer_list<int>& il, int = 0) : value(static_cast<int>(il.size())) {}

    constexpr constexpr_test_type(const constexpr_test_type& o) noexcept
        : value(check_value(o.value)) {}

    constexpr constexpr_test_type(constexpr_test_type&& o) noexcept
        : value(check_value(o.value)) {}

    constexpr constexpr_test_type& operator=(int xvalue) noexcept {
        value = xvalue;
        return *this;
    }

    constexpr constexpr_test_type& operator=(const constexpr_test_type& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        value = o.value;
        return *this;
    }

    constexpr constexpr_test_type& operator=(constexpr_test_type&& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        value = o.value;
        o.value = -1;
        return *this;
    }

    int value;
};

struct explicit_constexpr_test_type {
private:
    constexpr static int check_value(const int& val) {
        check(val != -1);
        check(val != 999);
        return val;
    }

    constexpr static int check_value(int& val, int val_cp = 0) {
        check(val != -1);
        check(val != 999);
        val_cp = val;
        val = -1;
        return val_cp;
    }

public:
    explicit constexpr explicit_constexpr_test_type() noexcept : value(0) {}

    explicit constexpr explicit_constexpr_test_type(int x) : value(x) {}
    explicit constexpr explicit_constexpr_test_type(int, int y) : value(y) {}
    explicit constexpr explicit_constexpr_test_type(std::initializer_list<int>& il, int = 0) : value(static_cast<int>(il.size())) {}

    explicit constexpr explicit_constexpr_test_type(const explicit_constexpr_test_type& o) noexcept
        : value(check_value(o.value)) {}

    explicit constexpr explicit_constexpr_test_type(explicit_constexpr_test_type&& o) noexcept
        : value(check_value(o.value)) {}

    constexpr explicit_constexpr_test_type& operator=(int xvalue) noexcept {
        value = xvalue;
        return *this;
    }

    constexpr explicit_constexpr_test_type& operator=(const explicit_constexpr_test_type& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        value = o.value;
        return *this;
    }

    constexpr explicit_constexpr_test_type& operator=(explicit_constexpr_test_type&& o) noexcept {
        check(o.value != -1);
        check(o.value != -999);
        value = o.value;
        o.value = -1;
        return *this;
    }

    int value;
};

template<typename T>
constexpr bool operator==(const T& x, const T& y) noexcept {
    return x.value == y.value;
}
template<typename T>
constexpr bool operator!=(const T& x, const T& y) noexcept {
    return x.value != y.value;
}

}
