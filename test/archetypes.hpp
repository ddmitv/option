#pragma once

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/support/archetypes.h

#include <cstdlib>
#include <initializer_list>

#include <tests.hpp>

namespace archetype_bases {

    struct null_base {
    protected:
        null_base() = default;
        null_base(const null_base&) = default;
        null_base& operator=(const null_base&) = default;
        null_base(null_base&&) = default;
        null_base& operator=(null_base&&) = default;
    };

    template<typename Derived, bool IsExplicit = false>
    struct test_base {
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

        test_base() noexcept : value(0) {
            alive += 1;
            constructed += 1;
            default_constructed += 1;
        }
        explicit(IsExplicit) test_base(int x) noexcept : value(x) {
            alive += 1;
            constructed += 1;
            value_constructed += 1;
        }
        explicit(IsExplicit) test_base(int, int y) noexcept : value(y) {
            alive += 1;
            constructed += 1;
            value_constructed += 1;
        }
        explicit test_base(std::initializer_list<int>& il, int = 0) noexcept
            : value(static_cast<int>(il.size())) {
            alive += 1;
            constructed += 1;
            value_constructed += 1;
        }
        test_base& operator=(int xvalue) noexcept {
            value = xvalue;
            assigned += 1;
            value_assigned += 1;
            return *this;
        }
    protected:
        ~test_base() {
            check(value != -999);
            check(alive > 0);
            --alive;
            destroyed += 1;
            value = -999;
        }
        explicit test_base(const test_base& o) noexcept : value(o.value) {
            check(o.value != -1);
            check(o.value != -999);
            alive += 1;
            constructed += 1;
            copy_constructed += 1;
        }
        explicit test_base(test_base&& o) noexcept : value(o.value) {
            check(o.value != -1);
            check(o.value != -999);
            alive += 1;
            constructed += 1;
            move_constructed += 1;
            o.value = -1;
        }
        test_base& operator=(const test_base& o) noexcept {
            check(o.value != -1);
            check(o.value != -999);
            assigned += 1;
            copy_assigned += 1;
            value = o.value;
            return *this;
        }
        test_base& operator=(test_base&& o) noexcept {
            check(o.value != -1);
            check(o.value != -999);
            assigned += 1;
            move_assigned += 1;
            value = o.value;
            o.value = -1;
            return *this;
        }
    public:
        int value;
    };

    template<bool IsExplicit = false>
    struct value_base {
        explicit(IsExplicit) constexpr value_base(int x) : value(x) {}
        explicit(IsExplicit) constexpr value_base(int, int y) : value(y) {}
        explicit(IsExplicit) constexpr value_base(std::initializer_list<int>& il, int = 0)
            : value(static_cast<int>(il.size())) {}
        constexpr value_base& operator=(int xvalue) noexcept {
            value = xvalue;
            return *this;
        }
        int value;
    protected:
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
        constexpr value_base() noexcept : value(0) {}
        constexpr value_base(const value_base& o) noexcept : value(check_value(o.value)) {}
        constexpr value_base(value_base&& o) noexcept : value(check_value(o.value)) {}
        constexpr value_base& operator=(const value_base& o) noexcept {
            check(o.value != -1);
            check(o.value != -999);
            value = o.value;
            return *this;
        }
        constexpr value_base& operator=(value_base&& o) noexcept {
            check(o.value != -1);
            check(o.value != -999);
            value = o.value;
            o.value = -1;
            return *this;
        }
    };

    template<bool IsExplicit = false>
    struct trivial_value_base {
        explicit(IsExplicit) constexpr trivial_value_base(int x) : value(x) {}
        explicit(IsExplicit) constexpr trivial_value_base(int, int y) : value(y) {}
        explicit(IsExplicit) constexpr trivial_value_base(std::initializer_list<int>& il, int = 0)
            : value(static_cast<int>(il.size())) {}
        int value;
    protected:
        constexpr trivial_value_base() noexcept : value(0) {}
    };
}

namespace implicit_types {
    #include "archetypes.ipp"
}

namespace explicit_types {
    #define SPEC_EXPLICIT explicit
    #include "archetypes.ipp"
}

namespace non_constexpr_types {
    #define SPEC_CONSTEXPR
    #include "archetypes.ipp"
}

namespace non_literal_types {
    #define SPEC_ASSIGN_CONSTEXPR
    #define DTOR_BODY(Name) ~Name() {}
    #include "archetypes.ipp"
}

namespace non_throwing_types {
    #define SPEC_NOEXCEPT noexcept
    #include "archetypes.ipp"
}

namespace non_trivial_types {
    #define CTOR_BODY {}
    #define ASSIGN_BODY { return *this; }
    #include "archetypes.ipp"
}

namespace test_types {
    #define SPEC_CONSTEXPR
    #define ARCHETYPE_BASE(Name) ::archetype_bases::test_base<Name>
    #include "archetypes.ipp"

    using test_type = all_ctors;

    template<typename Tp>
    constexpr bool operator==(const Tp& l, const Tp& r) noexcept {
        return l.value == r.value;
    }

    template<typename Tp>
    constexpr bool operator!=(const Tp& l, const Tp& r) noexcept {
        return l.value != r.value;
    }
}

namespace explicit_test_types {
    #define SPEC_CONSTEXPR
    #define SPEC_EXPLICIT explicit
    #define ARCHETYPE_BASE(Name) ::archetype_bases::test_base<Name, true>
    #include "archetypes.ipp"

    using test_type = all_ctors;

    template<typename Tp>
    constexpr bool operator==(const Tp& l, const Tp& r) noexcept {
        return l.value == r.value;
    }

    template<typename Tp>
    constexpr bool operator!=(const Tp& l, const Tp& r) noexcept {
        return l.value != r.value;
    }
}

namespace constexpr_test_types {
    #define ARCHETYPE_BASE(Name) ::archetype_bases::value_base<>
    #include "archetypes.ipp"

    using test_type = all_ctors;

    template<typename Tp>
    constexpr bool operator==(const Tp& l, const Tp& r) noexcept {
        return l.value == r.value;
    }
    template<typename Tp>
    constexpr bool operator!=(const Tp& l, const Tp& r) noexcept {
        return l.value != r.value;
    }
}

namespace explicit_constexpr_test_types {
    #define SPEC_EXPLICIT explicit
    #define ARCHETYPE_BASE(Name) ::archetype_bases::value_base<true>
    #include "archetypes.ipp"

    using test_type = all_ctors;

    template<typename Tp>
    constexpr bool operator==(const Tp& l, const Tp& r) noexcept {
        return l.value == r.value;
    }

    template<typename Tp>
    constexpr bool operator!=(const Tp& l, const Tp& r) noexcept {
        return l.value != r.value;
    }
}

namespace trivial_test_types {
    #define ARCHETYPE_BASE(Name) ::archetype_bases::trivial_value_base<false>
    #include "archetypes.ipp"

    using test_type = all_ctors;

    template<typename Tp>
    constexpr bool operator==(const Tp& l, const Tp& r) noexcept {
        return l.value == r.value;
    }

    template<typename Tp>
    constexpr bool operator!=(const Tp& l, const Tp& r) noexcept {
        return l.value != r.value;
    }
}

namespace explicit_trivial_test_types {
    #define SPEC_EXPLICIT explicit
    #define ARCHETYPE_BASE(Name) ::archetype_bases::trivial_value_base<true>
    #include "archetypes.ipp"

    using test_type = all_ctors;

    template<typename Tp>
    constexpr bool operator==(const Tp& l, const Tp& r) noexcept {
        return l.value == r.value;
    }

    template<typename Tp>
    constexpr bool operator!=(const Tp& l, const Tp& r) noexcept {
        return l.value != r.value;
    }
}
