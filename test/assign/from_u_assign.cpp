#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>
#include <memory>
#include <type_traits>
#include <utility>

// template<typename U>
// option& operator=(U&& value);

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/assign_value.pass.cpp

namespace {

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
struct ThrowAssign {
    static inline int dtor_called = 0;

    [[maybe_unused]] ThrowAssign() = default;
    [[noreturn]] ThrowAssign(int) { throw 42; }
    [[noreturn]] ThrowAssign& operator=(int) { throw 42; }
    ~ThrowAssign() { ++dtor_called; }
};
#endif

template<typename T, typename Arg = T, bool Expect = true>
void assert_assignable() {
    static_assert(std::is_assignable_v<opt::option<T>&, Arg> == Expect, "unexpected assignability");
    static_assert(!std::is_assignable_v<const opt::option<T>&, Arg>, "const option must not be assignable");
}

struct MismatchType {
    [[maybe_unused]] explicit MismatchType(int) {}
    [[maybe_unused]] explicit MismatchType(char*) {}
    explicit MismatchType(int*) = delete;

    [[maybe_unused]] MismatchType& operator=(int) { return *this; }
    [[maybe_unused]] MismatchType& operator=(int*) { return *this; }
    MismatchType& operator=(char*) = delete;
};

struct FromOptionalType {
    using Opt = opt::option<FromOptionalType>;

    [[maybe_unused]] FromOptionalType() = default;
    FromOptionalType(FromOptionalType const&) = delete;

    template<typename Dummy = void>
    [[maybe_unused]] constexpr FromOptionalType(Opt&) { Dummy::BARK; }

    template<typename Dummy = void>
    [[maybe_unused]] constexpr FromOptionalType& operator=(Opt&) { Dummy::BARK; return *this; }
};

void test_sfinae() {
    using I = test_types::test_type;
    using E = explicit_test_types::all_ctors;

    assert_assignable<int>();
    assert_assignable<int, int&>();
    assert_assignable<int, int const&>();
    // Implicit test type
    assert_assignable<I, I const&>();
    assert_assignable<I, I&&>();
    assert_assignable<I, int>();
    assert_assignable<I, void*, false>();
    // Explicit test type
    assert_assignable<E, E const&>();
    assert_assignable<E, E&&>();
    assert_assignable<E, int>();
    assert_assignable<E, void*, false>();
    // Mismatch type
    assert_assignable<MismatchType, int>();
    assert_assignable<MismatchType, int*, false>();
    assert_assignable<MismatchType, char*, false>();
    // Type constructible from option
    assert_assignable<FromOptionalType, opt::option<FromOptionalType>&, false>();
}

void test_with_test_type() {
    using T = test_types::test_type;
    T::reset();
    { // to empty
        opt::option<T> opt;
        opt = 3;
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::assigned == 0);
        check(T::destroyed == 0);
        check(opt.has_value());
        check(*opt == T(3));
    }
    { // to existing
        opt::option<T> opt(42);
        T::reset_constructors();
        opt = 3;
        check(T::alive == 1);
        check(T::constructed == 0);
        check(T::assigned == 1);
        check(T::value_assigned == 1);
        check(T::destroyed == 0);
        check(opt.has_value());
        check(*opt == T(3));
    }
    { // test default argument
        opt::option<T> opt;
        T::reset_constructors();
        opt = {1, 2};
        check(T::alive == 1);
        check(T::constructed == 2);
        check(T::value_constructed == 1);
        check(T::move_constructed == 1);
        check(T::assigned == 0);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(1, 2));
    }
    { // test default argument
        opt::option<T> opt(42);
        T::reset_constructors();
        opt = {1, 2};
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::assigned == 1);
        check(T::move_assigned == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(1, 2));
    }
    { // test default argument
        opt::option<T> opt;
        T::reset_constructors();
        opt = {1};
        check(T::alive == 1);
        check(T::constructed == 2);
        check(T::value_constructed == 1);
        check(T::move_constructed == 1);
        check(T::assigned == 0);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(1));
    }
    { // test default argument
        opt::option<T> opt(42);
        T::reset_constructors();
        opt = {};
        check(!opt.has_value());
        check(T::alive == 0);
        check(T::constructed == 0);
        check(T::assigned == 0);
        check(T::destroyed == 1);
    }
}

template<typename T, typename Value = int>
void test_with_type() {
    { // to empty
        opt::option<T> opt;
        opt = Value(3);
        check(opt.has_value());
        check(*opt == T(3));
    }
    { // to existing
        opt::option<T> opt(Value(42));
        opt = Value(3);
        check(opt.has_value());
        check(*opt == T(3));
    }
    { // test const
        opt::option<T> opt(Value(42));
        const T t(Value(3));
        opt = t;
        check(opt.has_value());
        check(*opt == T(3));
    }
    { // test default argument
        opt::option<T> opt;
        opt = {Value(1)};
        check(opt.has_value());
        check(*opt == T(1));
    }
    { // test default argument
        opt::option<T> opt(Value(42));
        opt = {};
        check(!opt.has_value());
    }
}

template<typename T>
void test_with_type_multi() {
    test_with_type<T>();
    { // test default argument
        opt::option<T> opt;
        opt = {1, 2};
        check(opt.has_value());
        check(*opt == T(1, 2));
    }
    { // test default argument
        opt::option<T> opt(42);
        opt = {1, 2};
        check(opt.has_value());
        check(*opt == T(1, 2));
    }
}

void test_throws() {
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    using T = ThrowAssign;
    {
        opt::option<T> opt;
        check_throw(opt = 42, int);
        check(!opt.has_value());
    }
    check(T::dtor_called == 0);
    {
        T::dtor_called = 0;
        opt::option<T> opt(std::in_place);
        check_throw(opt = 42, int);
        check(opt.has_value());
        check(T::dtor_called == 0);
    }
    check(T::dtor_called == 1);
#endif
}

enum MyEnum { Zero, One, Two, Three, FortyTwo = 42 };

using Fn = void(*)();

// https://llvm.org/PR38638
template<typename T>
constexpr T pr38638(T v) {
    opt::option<T> o;
    o = v;
    return *o + 2;
}

// TODO: test_ref

}

void from_u_assign_test_suite() {
    test_sfinae();
    // Test with instrumented type
    test_with_test_type();
    // Test with various scalar types
    test_with_type<int>();
    test_with_type<MyEnum, MyEnum>();
    test_with_type<int, MyEnum>();
    test_with_type<Fn, Fn>();
    // Test types with multi argument constructors
    test_with_type_multi<constexpr_test_types::all_ctors>();
    test_with_type_multi<trivial_test_types::all_ctors>();
    // Test move only types
    {
        opt::option<std::unique_ptr<int>> opt;
        opt = std::make_unique<int>(3);
        check(opt.has_value());
        check(**opt == 3);
    }
    {
        opt::option<std::unique_ptr<int>> opt(std::make_unique<int>(2));
        opt = std::make_unique<int>(3);
        check(opt.has_value());
        check(**opt == 3);
    }
    test_throws();

    static_assert(pr38638(3) == 5);
}
