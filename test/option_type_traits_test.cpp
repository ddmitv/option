// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <opt/option.hpp>
#include <type_traits>
#include <memory>

OPTION_SUPPRESS_COMMON_WARNINGS_PUSH

OPTION_CLANG_SUPPRESS_WARNING("-Wunneeded-member-function")

namespace {

struct non_trivial_copy_ctor {
    non_trivial_copy_ctor(const non_trivial_copy_ctor&) {}
};
static_assert(!std::is_trivially_copy_constructible_v<opt::option<non_trivial_copy_ctor>>);
static_assert(std::is_copy_constructible_v<opt::option<non_trivial_copy_ctor>>);
static_assert(!std::is_nothrow_copy_constructible_v<opt::option<non_trivial_copy_ctor>>);

static_assert(!std::is_trivially_copy_assignable_v<opt::option<non_trivial_copy_ctor>>);
static_assert(std::is_copy_assignable_v<opt::option<non_trivial_copy_ctor>>);
static_assert(!std::is_nothrow_copy_assignable_v<opt::option<non_trivial_copy_ctor>>);

struct trivial_copy_ctor {
    trivial_copy_ctor(const trivial_copy_ctor&) = default;
};
static_assert(std::is_trivially_copy_constructible_v<opt::option<trivial_copy_ctor>>);
static_assert(std::is_copy_constructible_v<opt::option<trivial_copy_ctor>>);
static_assert(std::is_nothrow_copy_constructible_v<opt::option<trivial_copy_ctor>>);

static_assert(std::is_trivially_copy_assignable_v<opt::option<trivial_copy_ctor>>);
static_assert(std::is_copy_assignable_v<opt::option<trivial_copy_ctor>>);
static_assert(std::is_nothrow_copy_assignable_v<opt::option<trivial_copy_ctor>>);

struct deleted_copy_ctor {
    deleted_copy_ctor(const deleted_copy_ctor&) = delete;
};
static_assert(!std::is_trivially_copy_constructible_v<opt::option<deleted_copy_ctor>>);
static_assert(!std::is_copy_constructible_v<opt::option<deleted_copy_ctor>>);
static_assert(!std::is_nothrow_copy_constructible_v<opt::option<deleted_copy_ctor>>);

static_assert(!std::is_trivially_copy_assignable_v<opt::option<deleted_copy_ctor>>);
static_assert(!std::is_copy_assignable_v<opt::option<deleted_copy_ctor>>);
static_assert(!std::is_nothrow_copy_assignable_v<opt::option<deleted_copy_ctor>>);

struct non_trivial_move_ctor {
    non_trivial_move_ctor(non_trivial_move_ctor&&) {}
};
static_assert(!std::is_trivially_move_constructible_v<opt::option<non_trivial_move_ctor>>);
static_assert(std::is_move_constructible_v<opt::option<non_trivial_move_ctor>>);
static_assert(!std::is_nothrow_move_constructible_v<opt::option<non_trivial_move_ctor>>);
// move assignment operator is implicitly deleted if move ctor is declared
static_assert(!std::is_trivially_move_assignable_v<opt::option<non_trivial_move_ctor>>);
static_assert(!std::is_move_assignable_v<opt::option<non_trivial_move_ctor>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<non_trivial_move_ctor>>);

struct non_trivial_move_ctor_but_move_assignable {
    non_trivial_move_ctor_but_move_assignable(non_trivial_move_ctor_but_move_assignable&&) {}
    non_trivial_move_ctor_but_move_assignable& operator=(non_trivial_move_ctor_but_move_assignable&&) = default;
};
static_assert(!std::is_trivially_move_constructible_v<opt::option<non_trivial_move_ctor_but_move_assignable>>);
static_assert(std::is_move_constructible_v<opt::option<non_trivial_move_ctor_but_move_assignable>>);
static_assert(!std::is_nothrow_move_constructible_v<opt::option<non_trivial_move_ctor_but_move_assignable>>);
// move assignment operator is implicitly deleted if move ctor is declared
static_assert(!std::is_trivially_move_assignable_v<opt::option<non_trivial_move_ctor_but_move_assignable>>);
static_assert(std::is_move_assignable_v<opt::option<non_trivial_move_ctor_but_move_assignable>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<non_trivial_move_ctor_but_move_assignable>>);

struct non_trivial_nothrow_move_ctor {
    non_trivial_nothrow_move_ctor(non_trivial_nothrow_move_ctor&&) noexcept {}
};
static_assert(!std::is_trivially_move_constructible_v<opt::option<non_trivial_nothrow_move_ctor>>);
static_assert(std::is_move_constructible_v<opt::option<non_trivial_nothrow_move_ctor>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<non_trivial_nothrow_move_ctor>>);
// move assignment operator is implicitly deleted if move ctor is declared
static_assert(!std::is_trivially_move_assignable_v<opt::option<non_trivial_nothrow_move_ctor>>);
static_assert(!std::is_move_assignable_v<opt::option<non_trivial_nothrow_move_ctor>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<non_trivial_nothrow_move_ctor>>);

struct trivial_move_ctor {
    trivial_move_ctor(trivial_move_ctor&&) = default; // implicitly noexcept
};
static_assert(std::is_trivially_move_constructible_v<opt::option<trivial_move_ctor>>);
static_assert(std::is_move_constructible_v<opt::option<trivial_move_ctor>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<trivial_move_ctor>>);
// move assignment operator is implicitly deleted if move ctor is declared
static_assert(!std::is_trivially_move_assignable_v<opt::option<trivial_move_ctor>>);
static_assert(!std::is_move_assignable_v<opt::option<trivial_move_ctor>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<trivial_move_ctor>>);

struct trivial_throw_move_ctor {
    trivial_throw_move_ctor(trivial_throw_move_ctor&&) noexcept(false) = default;
};
static_assert(std::is_trivially_move_constructible_v<opt::option<trivial_throw_move_ctor>>);
static_assert(std::is_move_constructible_v<opt::option<trivial_throw_move_ctor>>);
static_assert(!std::is_nothrow_move_constructible_v<opt::option<trivial_throw_move_ctor>>);
// move assignment operator is implicitly deleted if move ctor is declared
static_assert(!std::is_trivially_move_assignable_v<opt::option<trivial_throw_move_ctor>>);
static_assert(!std::is_move_assignable_v<opt::option<trivial_throw_move_ctor>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<trivial_throw_move_ctor>>);

struct deleted_move_ctor {
    deleted_move_ctor(deleted_move_ctor&&) = delete;
};
static_assert(!std::is_trivially_move_constructible_v<opt::option<deleted_move_ctor>>);
static_assert(!std::is_move_constructible_v<opt::option<deleted_move_ctor>>);
static_assert(!std::is_nothrow_move_constructible_v<opt::option<deleted_move_ctor>>);

static_assert(!std::is_trivially_move_assignable_v<opt::option<deleted_move_ctor>>);
static_assert(!std::is_move_assignable_v<opt::option<deleted_move_ctor>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<deleted_move_ctor>>);

struct non_trivial_copy_assign {
    non_trivial_copy_assign& operator=(const non_trivial_copy_assign&) { return *this; }
};
static_assert(std::is_trivially_copy_constructible_v<opt::option<non_trivial_copy_assign>>);
static_assert(std::is_copy_constructible_v<opt::option<non_trivial_copy_assign>>);
static_assert(std::is_nothrow_copy_constructible_v<opt::option<non_trivial_copy_assign>>);

static_assert(!std::is_trivially_copy_assignable_v<opt::option<non_trivial_copy_assign>>);
static_assert(std::is_copy_assignable_v<opt::option<non_trivial_copy_assign>>);
static_assert(!std::is_nothrow_copy_assignable_v<opt::option<non_trivial_copy_assign>>);

struct trivial_copy_assign {
    trivial_copy_assign& operator=(const trivial_copy_assign&) = default;
};
static_assert(std::is_trivially_copy_constructible_v<opt::option<trivial_copy_assign>>);
static_assert(std::is_copy_constructible_v<opt::option<trivial_copy_assign>>);
static_assert(std::is_nothrow_copy_constructible_v<opt::option<trivial_copy_assign>>);

static_assert(std::is_trivially_copy_assignable_v<opt::option<trivial_copy_assign>>);
static_assert(std::is_copy_assignable_v<opt::option<trivial_copy_assign>>);
static_assert(std::is_nothrow_copy_assignable_v<opt::option<trivial_copy_assign>>);

struct deleted_copy_assign {
    deleted_copy_assign& operator=(const deleted_copy_assign&) = delete;
};
static_assert(std::is_trivially_copy_constructible_v<opt::option<deleted_copy_assign>>);
static_assert(std::is_copy_constructible_v<opt::option<deleted_copy_assign>>);
static_assert(std::is_nothrow_copy_constructible_v<opt::option<deleted_copy_assign>>);

static_assert(!std::is_trivially_copy_assignable_v<opt::option<deleted_copy_assign>>);
static_assert(!std::is_copy_assignable_v<opt::option<deleted_copy_assign>>);
static_assert(!std::is_nothrow_copy_assignable_v<opt::option<deleted_copy_assign>>);

struct non_trivial_move_assign {
    non_trivial_move_assign(non_trivial_move_assign&&) = default;
    non_trivial_move_assign& operator=(non_trivial_move_assign&&) { return *this; }
};
static_assert(std::is_trivially_move_constructible_v<opt::option<non_trivial_move_assign>>);
static_assert(std::is_move_constructible_v<opt::option<non_trivial_move_assign>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<non_trivial_move_assign>>);

static_assert(!std::is_trivially_move_assignable_v<opt::option<non_trivial_move_assign>>);
static_assert(std::is_move_assignable_v<opt::option<non_trivial_move_assign>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<non_trivial_move_assign>>);

struct non_trivial_nothrow_move_assign {
    non_trivial_nothrow_move_assign(non_trivial_nothrow_move_assign&&) = default;
    non_trivial_nothrow_move_assign& operator=(non_trivial_nothrow_move_assign&&) noexcept { return *this; }
};
static_assert(std::is_trivially_move_constructible_v<opt::option<non_trivial_nothrow_move_assign>>);
static_assert(std::is_move_constructible_v<opt::option<non_trivial_nothrow_move_assign>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<non_trivial_nothrow_move_assign>>);

static_assert(!std::is_trivially_move_assignable_v<opt::option<non_trivial_nothrow_move_assign>>);
static_assert(std::is_move_assignable_v<opt::option<non_trivial_nothrow_move_assign>>);
static_assert(std::is_nothrow_move_assignable_v<opt::option<non_trivial_nothrow_move_assign>>);

struct trivial_move_assign {
    trivial_move_assign(trivial_move_assign&&) = default;
    trivial_move_assign& operator=(trivial_move_assign&&) noexcept = default;
};
static_assert(std::is_trivially_move_constructible_v<opt::option<trivial_move_assign>>);
static_assert(std::is_move_constructible_v<opt::option<trivial_move_assign>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<trivial_move_assign>>);

static_assert(std::is_trivially_move_assignable_v<opt::option<trivial_move_assign>>);
static_assert(std::is_move_assignable_v<opt::option<trivial_move_assign>>);
static_assert(std::is_nothrow_move_assignable_v<opt::option<trivial_move_assign>>);

struct trivial_throw_move_assign {
    trivial_throw_move_assign(trivial_throw_move_assign&&) = default;
    trivial_throw_move_assign& operator=(trivial_throw_move_assign&&) noexcept(false) = default;
};
static_assert(std::is_trivially_move_constructible_v<opt::option<trivial_throw_move_assign>>);
static_assert(std::is_move_constructible_v<opt::option<trivial_throw_move_assign>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<trivial_throw_move_assign>>);

static_assert(std::is_trivially_move_assignable_v<opt::option<trivial_throw_move_assign>>);
static_assert(std::is_move_assignable_v<opt::option<trivial_throw_move_assign>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<trivial_throw_move_assign>>);

struct deleted_move_assign {
    deleted_move_assign(deleted_move_assign&&) = default;
    deleted_move_assign& operator=(deleted_move_assign&&) = delete;
};
static_assert(std::is_trivially_move_constructible_v<opt::option<deleted_move_assign>>);
static_assert(std::is_move_constructible_v<opt::option<deleted_move_assign>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<deleted_move_assign>>);

static_assert(!std::is_trivially_move_assignable_v<opt::option<deleted_move_assign>>);
static_assert(!std::is_move_assignable_v<opt::option<deleted_move_assign>>);
static_assert(!std::is_nothrow_move_assignable_v<opt::option<deleted_move_assign>>);

struct non_trivial_dtor {
    // copy/move ctors and copy/move assignment ops are always non-trivial if dtor is non-trivial
    non_trivial_dtor(const non_trivial_dtor&) = default;
    non_trivial_dtor(non_trivial_dtor&&) = default;
    non_trivial_dtor& operator=(const non_trivial_dtor&) = default;
    non_trivial_dtor& operator=(non_trivial_dtor&&) = default;
    ~non_trivial_dtor() {}
};
static_assert(!std::is_trivially_destructible_v<opt::option<non_trivial_dtor>>);

static_assert(!std::is_trivially_copy_constructible_v<opt::option<non_trivial_dtor>>);
static_assert(std::is_copy_constructible_v<opt::option<non_trivial_dtor>>);
static_assert(!std::is_nothrow_copy_constructible_v<opt::option<non_trivial_dtor>>);

static_assert(!std::is_trivially_copy_assignable_v<opt::option<non_trivial_dtor>>);
static_assert(std::is_copy_assignable_v<opt::option<non_trivial_dtor>>);
static_assert(!std::is_nothrow_copy_assignable_v<opt::option<non_trivial_dtor>>);

static_assert(!std::is_trivially_move_constructible_v<opt::option<non_trivial_dtor>>);
static_assert(std::is_move_constructible_v<opt::option<non_trivial_dtor>>);
static_assert(std::is_nothrow_move_constructible_v<opt::option<non_trivial_dtor>>);

static_assert(!std::is_trivially_move_assignable_v<opt::option<non_trivial_dtor>>);
static_assert(std::is_move_assignable_v<opt::option<non_trivial_dtor>>);
static_assert(std::is_nothrow_move_assignable_v<opt::option<non_trivial_dtor>>);

struct base {};
struct derived : base {};

struct multi_arg {
    multi_arg(int, double, char) {}
};

struct ilist_ctor {
    ilist_ctor(std::initializer_list<int>, int) {}
};

struct from_int_and_optional {
    from_int_and_optional(int) {}
    [[maybe_unused]] from_int_and_optional(opt::option<int>) {}
};

// [optional.ctor]/1
static_assert(std::is_nothrow_default_constructible_v<opt::option<int>>);
// [optional.ctor]/2
static_assert(std::is_nothrow_constructible_v<opt::option<int>, opt::none_t>);
static_assert(std::is_convertible_v<opt::none_t, opt::option<int>>);
// [optional.ctor]/6
static_assert(std::is_constructible_v<opt::option<int>, int>);
static_assert(std::is_convertible_v<int, opt::option<int>>);

static_assert(std::is_constructible_v<opt::option<long>, short>);
static_assert(std::is_convertible_v<short, opt::option<long>>);

static_assert(!std::is_constructible_v<opt::option<int>, void*>);
static_assert(std::is_constructible_v<opt::option<from_int_and_optional>, int>);
// [optional.ctor]/7
static_assert(std::is_constructible_v<opt::option<int>, const opt::option<short>&>);
static_assert(std::is_convertible_v<const opt::option<short>&, opt::option<int>>);

static_assert(std::is_constructible_v<opt::option<base>, const opt::option<derived>&>);
static_assert(std::is_convertible_v<const opt::option<derived>&, opt::option<base>>);

static_assert(!std::is_constructible_v<opt::option<int>, const opt::option<void*>&>);

static_assert(!std::is_constructible_v<opt::option<bool>, const opt::option<int>&>);
// [optional.ctor]/8
static_assert(std::is_constructible_v<opt::option<int>, opt::option<short>&&>);
static_assert(std::is_convertible_v<opt::option<short>&&, opt::option<int>>);

static_assert(std::is_constructible_v<opt::option<base>, opt::option<derived>&&>);
static_assert(std::is_convertible_v<opt::option<derived>&&, opt::option<base>>);

static_assert(!std::is_constructible_v<opt::option<int>, opt::option<void*>&&>);

static_assert(!std::is_constructible_v<opt::option<bool>, opt::option<int>&&>);
// [optional.ctor]/9-10
static_assert(std::is_constructible_v<opt::option<multi_arg>, std::in_place_t, int, double, char>);
static_assert(std::is_constructible_v<opt::option<ilist_ctor>, std::in_place_t, std::initializer_list<int>, int>);

static_assert(!std::is_convertible_v<std::in_place_t, opt::option<int>>);
// [optional.assign]/1-2
static_assert(std::is_nothrow_assignable_v<opt::option<int>&, opt::none_t>);
static_assert(std::is_assignable_v<opt::option<deleted_copy_assign>&, opt::none_t>);
// [optional.assign]/3
static_assert(std::is_assignable_v<opt::option<int>&, int>);
static_assert(std::is_assignable_v<opt::option<long>&, short>);
// [optional.assign]/4
static_assert(std::is_assignable_v<opt::option<int>&, const opt::option<short>&>);
static_assert(std::is_assignable_v<opt::option<base>&, const opt::option<derived>&>);
static_assert(!std::is_assignable_v<opt::option<int>&, const opt::option<void*>&>);
// [optional.assign]/5
static_assert(std::is_assignable_v<opt::option<int>&, opt::option<short>&&>);
static_assert(std::is_assignable_v<opt::option<base>&, opt::option<derived>&&>);
static_assert(!std::is_assignable_v<opt::option<int>&, opt::option<void*>&&>);


// std::unique_ptr
static_assert(std::is_constructible_v<opt::option<std::unique_ptr<int>>, std::unique_ptr<int>&&>);
static_assert(std::is_move_constructible_v<opt::option<std::unique_ptr<int>>>);

struct explicit_from_int { explicit explicit_from_int(int) {} };
static_assert(std::is_constructible_v<opt::option<explicit_from_int>, int>);
static_assert(!std::is_convertible_v<int, opt::option<explicit_from_int>>);

}

OPTION_SUPPRESS_COMMON_WARNINGS_POP
