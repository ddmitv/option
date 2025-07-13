// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <opt/option.hpp>
#include <doctest/doctest.h>

#pragma warning(push, 3)

namespace {

struct some_struct {
    constexpr some_struct() = default;
    constexpr some_struct(int a, float b, char c) : a{a}, b{b}, c{c} {}

    int a = 2;
    float b = 34.3f;
    char c = 'b';
};

TEST_CASE_TEMPLATE("generic opt::option", T, int, unsigned int, some_struct) {
    SUBCASE("unengaged constructored") {
        {
            opt::option<T> o;
            CHECK(!o.has_value());
        }
        {
            opt::option<T> o(opt::none);
            CHECK(!o.has_value());
        }
        {
            opt::option<T> o = opt::none;
            CHECK(!o.has_value());
        }
        {
            opt::option<const T> o;
            CHECK(!o.has_value());
        }
    }
    // SUBCASE("default constructed") {
    //     {
    //         opt::option<T> o{T{}};
    //         CHECK(o.has_value());
    //         CHECK(o.get() == T{});
    //     }
    //     {
    //         opt::option<T> o{{}};
    //         CHECK(o.has_value());
    //         CHECK(o.get() == T{});
    //     }
    //     {
    //         opt::option<const T> o{T{}};
    //         CHECK(o.has_value());
    //         CHECK(o.get() == T{});
    //     }
    // }

}

TEST_CASE("opt::option<some_struct>") {
    SUBCASE("std::in_place constructed") {
        {
            opt::option<some_struct> o{std::in_place};
            CHECK(o.has_value());
            CHECK(o.get().a == 2);
            CHECK(o.get().b == 34.3f);
            CHECK(o.get().c == 'b');
        }
        {
            opt::option<some_struct> o{std::in_place, 10, 40.f, 'c'};
            CHECK(o.has_value());
            CHECK(o.get().a == 10);
            CHECK(o.get().b == 40.f);
            CHECK(o.get().c == 'c');
        }
    }
}

}

#pragma warning(pop)
