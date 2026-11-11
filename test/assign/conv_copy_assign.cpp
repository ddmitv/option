#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>
#include <type_traits>
#include <utility>

// From LWG2451:
// template<typename U>
// option& operator=(const option<U>& other);

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/const_optional_U.pass.cpp

namespace {

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
struct X {
    static inline bool throw_now = false;

    X(int) {
        if (throw_now) {
            throw 6;
        }
    }
};
#endif

template<typename T>
struct AssignableFrom {
    [[maybe_unused]] inline static int type_constructed = 0;
    [[maybe_unused]] inline static int type_assigned = 0;
    [[maybe_unused]] inline static int int_constructed = 0;
    [[maybe_unused]] inline static int int_assigned = 0;

    static void reset() {
        type_constructed = 0;
        type_assigned = 0;
        int_constructed = 0;
        int_assigned = 0;
    }

    AssignableFrom() = default;

    explicit AssignableFrom(T) { ++type_constructed; }
    AssignableFrom& operator=(T) { ++type_assigned; return *this; }

    AssignableFrom(int) { ++int_constructed; }
    AssignableFrom& operator=(int) { ++int_assigned; return *this; }

private:
    AssignableFrom(AssignableFrom const&) = delete;
    AssignableFrom& operator=(AssignableFrom const&) = delete;
};

void test_with_test_type() {
    using T = test_types::test_type;
    T::reset();
    { // non-empty to empty
        T::reset_constructors();
        opt::option<T> opt;
        const opt::option<int> other(42);
        opt = other;
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::assigned == 0);
        check(T::destroyed == 0);
        check(other.has_value());
        check(*other == 42);
        check(opt.has_value());
        check(*opt == T(42));
    }
    check(T::alive == 0);
    { // non-empty to non-empty
        opt::option<T> opt(101);
        const opt::option<int> other(42);
        T::reset_constructors();
        opt = other;
        check(T::alive == 1);
        check(T::constructed == 0);
        check(T::assigned == 1);
        check(T::value_assigned == 1);
        check(T::destroyed == 0);
        check(other.has_value());
        check(*other == 42);
        check(opt.has_value());
        check(*opt == T(42));
    }
    check(T::alive == 0);
    { // empty to non-empty
        opt::option<T> opt(101);
        const opt::option<int> other;
        T::reset_constructors();
        opt = other;
        check(T::alive == 0);
        check(T::constructed == 0);
        check(T::assigned == 0);
        check(T::destroyed == 1);
        check(!other.has_value());
        check(!opt.has_value());
    }
    check(T::alive == 0);
    { // empty to empty
        opt::option<T> opt;
        const opt::option<int> other;
        T::reset_constructors();
        opt = other;
        check(T::alive == 0);
        check(T::constructed == 0);
        check(T::assigned == 0);
        check(T::destroyed == 0);
        check(!other.has_value());
        check(!opt.has_value());
    }
    check(T::alive == 0);
}

void test_ambiguous_assign() {
    using OptInt = opt::option<int>;
    {
        using T = AssignableFrom<OptInt const&>;
        const OptInt a(42);
        T::reset();
        {
            opt::option<T> t;
            t = a;
            check(T::type_constructed == 1);
            check(T::type_assigned == 0);
            check(T::int_constructed == 0);
            check(T::int_assigned == 0);
        }
        T::reset();
        {
            opt::option<T> t(42);
            t = a;
            check(T::type_constructed == 0);
            check(T::type_assigned == 1);
            check(T::int_constructed == 1);
            check(T::int_assigned == 0);
        }
        T::reset();
        {
            opt::option<T> t(42);
            t = std::move(a);
            check(T::type_constructed == 0);
            check(T::type_assigned == 1);
            check(T::int_constructed == 1);
            check(T::int_assigned == 0);
        }
    }
    {
        using T = AssignableFrom<OptInt&>;
        OptInt a(42);
        T::reset();
        {
            opt::option<T> t;
            t = a;
            check(T::type_constructed == 1);
            check(T::type_assigned == 0);
            check(T::int_constructed == 0);
            check(T::int_assigned == 0);
        }
        {
            using Opt = opt::option<T>;
            static_assert(!std::is_assignable_v<Opt&, OptInt const&>);
        }
    }
}

// TODO: test_ref

}

void conv_copy_assign_test_suite() {
    test_with_test_type();
    test_ambiguous_assign();
    {
        opt::option<int> opt;
        constexpr opt::option<short> opt2;
        opt = opt2;
        static_assert(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    {
        opt::option<int> opt;
        constexpr opt::option<short> opt2(short{2});
        opt = opt2;
        static_assert(opt2.has_value());
        static_assert(*opt2 == 2);
        check(opt.has_value() == opt2.has_value());
        check(*opt == *opt2);
    }
    {
        opt::option<int> opt(3);
        constexpr opt::option<short> opt2;
        opt = opt2;
        static_assert(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    {
        opt::option<int> opt(3);
        constexpr opt::option<short> opt2(short{2});
        opt = opt2;
        static_assert(opt2.has_value());
        static_assert(*opt2 == 2);
        check(opt.has_value() == opt2.has_value());
        check(*opt == *opt2);
    }
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    {
        opt::option<X> opt;
        opt::option<int> opt2(42);
        check(opt2.has_value());
        X::throw_now = true;
        check_throw_value(opt = opt2, int, 6);
        check(!opt.has_value());
        X::throw_now = false;
    }
#endif
}
