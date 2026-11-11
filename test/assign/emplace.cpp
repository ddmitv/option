#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>
#include <initializer_list>
#include <type_traits>
#include <vector>

// template<typename... Args>
// T& option<T>::emplace(Args&&... args);
// template<typename U, typename... Args>
// T& option<T>::emplace(std::initializer_list<U> ilist, Args&&... args);

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/emplace.pass.cpp
// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/emplace_initializer_list.pass.cpp

namespace {

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
class Y {
public:
    static inline bool dtor_called = false;

    Y() = default;
    [[noreturn]] Y(int) { throw 6; }
    Y(const Y&) = default;
    Y& operator=(const Y&) = default;
    ~Y() { dtor_called = true; }
};
#endif

template<typename T>
constexpr bool test_one_arg() {
    using Opt = opt::option<T>;
    {
        Opt opt;
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(*opt == T(0));
        check(&v == &*opt);
    }
    {
        Opt opt;
        auto& v = opt.emplace(1);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(*opt == T(1));
        check(&v == &*opt);
    }
    {
        Opt opt(2);
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(*opt == T(0));
        check(&v == &*opt);
    }
    {
        Opt opt(2);
        auto& v = opt.emplace(1);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(*opt == T(1));
        check(&v == &*opt);
    }
    return true;
}

template<typename T>
constexpr bool test_multi_arg() {
    test_one_arg<T>();
    using Opt = opt::option<T>;
    {
        Opt opt;
        auto& v = opt.emplace(101, 41);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(v == T(101, 41));
        check(*opt == T(101, 41));
    }
    {
        Opt opt;
        auto& v = opt.emplace({1, 2, 3, 4});
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(v == T(4)); // T sets its value to the size of the init list
        check(*opt == T(4));
    }
    {
        Opt opt;
        auto& v = opt.emplace({1, 2, 3, 4, 5}, 6);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(opt.has_value());
        check(v == T(5)); // T sets its value to the size of the init list
        check(*opt == T(5));
    }
    return true;
}

template<typename T>
void test_on_test_type() {
    T::reset();
    opt::option<T> opt;
    check(T::alive == 0);
    {
        T::reset_constructors();
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::default_constructed == 1);
        check(T::destroyed == 0);
        check(opt.has_value());
        check(*opt == T());
        check(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace();
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::default_constructed == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T());
        check(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace(101);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(101));
        check(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace(-10, 99);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(-10, 99));
        check(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace(-10, 99);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(-10, 99));
        check(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace({-10, 99, 42, 1});
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(4)); // size of the initializer list
        check(&v == &*opt);
    }
    {
        T::reset_constructors();
        auto& v = opt.emplace({-10, 99, 42, 1}, 42);
        static_assert(std::is_same_v<T&, decltype(v)>);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::destroyed == 1);
        check(opt.has_value());
        check(*opt == T(4)); // size of the initializer list
        check(&v == &*opt);
    }
}

constexpr bool test_empty_emplace() {
    opt::option<const int> opt;
    auto& v = opt.emplace(42);
    static_assert(std::is_same_v<const int&, decltype(v)>);
    check(*opt == 42);
    check(v == 42);
    opt.emplace();
    check(*opt == 0);
    return true;
}

class init_list_X {
    int i_;
    int j_ = 0;
    bool* dtor_called_;
public:
    constexpr init_list_X(bool& dtor_called OPTION_LIFETIMEBOUND) : i_(0), dtor_called_(&dtor_called) {}
    [[maybe_unused]] constexpr init_list_X(int i, bool& dtor_called OPTION_LIFETIMEBOUND) : i_(i), dtor_called_(&dtor_called) {}
    constexpr init_list_X(std::initializer_list<int> il, bool& dtor_called OPTION_LIFETIMEBOUND)
        : i_(il.begin()[0]), j_(il.begin()[1]), dtor_called_(&dtor_called) {}

    init_list_X(const init_list_X&) = default;
    init_list_X& operator=(const init_list_X&) = default;
    constexpr ~init_list_X() { *dtor_called_ = true; }

    friend constexpr bool operator==(const init_list_X& x, const init_list_X& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

class init_list_Y {
    int i_;
    int j_ = 0;
public:
    [[maybe_unused]] constexpr init_list_Y() : i_(0) {}
    [[maybe_unused]] constexpr init_list_Y(int i) : i_(i) {}
    constexpr init_list_Y(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {}

    friend constexpr bool operator==(const init_list_Y& x, const init_list_Y& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
class init_list_Z {
    int i_;
    int j_ = 0;
public:
    static inline bool dtor_called = false;

    init_list_Z() : i_(0) {}
    [[maybe_unused]] init_list_Z(int i) : i_(i) {}
    [[noreturn]] init_list_Z(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) { throw 6; }
    init_list_Z(const init_list_Z&) = default;
    init_list_Z& operator=(const init_list_Z&) = default;
    ~init_list_Z() { dtor_called = true; }

    [[maybe_unused]] friend bool operator==(const init_list_Z& x, const init_list_Z& y) {
        return x.i_ == y.i_ && x.j_ == y.j_;
    }
};
#endif

constexpr bool test_emplace_init_list_X() {
    bool dtor_called = false;
    init_list_X x(dtor_called);
    opt::option<init_list_X> opt(x);
    check(dtor_called == false);
    auto& v = opt.emplace({1, 2}, dtor_called);
    static_assert(std::is_same_v<init_list_X&, decltype(v)>);
    check(dtor_called);
    check(*opt == init_list_X({1, 2}, dtor_called));
    check(&v == &*opt);
    return true;
}

constexpr bool test_emplace_init_list_Y() {
    opt::option<init_list_Y> opt;
    auto& v = opt.emplace({1, 2});
    static_assert(std::is_same_v<init_list_Y&, decltype(v)>);
    check(opt.has_value());
    check(*opt == init_list_Y({1, 2}));
    check(&v == &*opt);
    return true;
}

// TODO: test_ref

}

void emplace_test_suite() {
    {
        test_on_test_type<test_types::test_type>();
        test_on_test_type<explicit_test_types::all_ctors>();
    }
    {
        using T = int;
        test_one_arg<T>();
        test_one_arg<const T>();
        static_assert(test_one_arg<T>());
        static_assert(test_one_arg<const T>());
    }
    {
        using T = constexpr_test_types::all_ctors;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        using T = explicit_constexpr_test_types::all_ctors;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        using T = trivial_test_types::all_ctors;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        using T = explicit_trivial_test_types::all_ctors;
        test_multi_arg<T>();
        static_assert(test_multi_arg<T>());
    }
    {
        test_empty_emplace();
        static_assert(test_empty_emplace());
    }
    {
        test_emplace_init_list_X();
        static_assert(test_emplace_init_list_X());
    }
    {
        opt::option<std::vector<int>> opt;
        auto& v = opt.emplace({1, 2, 3}, std::allocator<int>());
        static_assert(std::is_same_v<std::vector<int>&, decltype(v)>);
        check(opt.has_value());
        check(*opt == std::vector<int>({1, 2, 3}));
        check(&v == &*opt);
    }
    {
        test_emplace_init_list_Y();
        static_assert(test_emplace_init_list_Y());
    }
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    Y::dtor_called = false;
    {
        Y y;
        opt::option<Y> opt(y);
        check(opt.has_value());
        check(Y::dtor_called == false);
        check_throw_value(opt.emplace(1), int, 6);
        check(!opt.has_value());
        check(Y::dtor_called == true);
    }
    {
        init_list_Z z;
        opt::option<init_list_Z> opt(z);
        check(opt.has_value());
        check(init_list_Z::dtor_called == false);
        check_throw_value(opt.emplace({1, 2}), int, 6);
        check(!opt.has_value());
        check(init_list_Z::dtor_called == true);
    }
#endif
}
