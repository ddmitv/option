#include <opt/option.hpp>
#include <tests.hpp>
#include <utility>

// template<typename U>
// constexpr remove_cv_t<T> option<T>::value_or(U&& v) const&;
// template<typename U>
// constexpr remove_cv_t<T> option<T>::value_or(U&& v) &&;

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.observe/value_or.pass.cpp
// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.observe/value_or_const.pass.cpp

namespace {

namespace const_ref {
    struct Y {
        int i_;

        constexpr Y(int i) : i_(i) {}
    };

    struct X {
        int i_;

        constexpr X(int i) : i_(i) {}
        constexpr X(const Y& y) : i_(y.i_) {}
        constexpr X(Y&& y) : i_(y.i_ + 1) {}
        friend constexpr bool operator==(const X& x, const X& y) {
            return x.i_ == y.i_;
        }
    };

    constexpr bool test() {
        {
            constexpr opt::option<X> opt(2);
            constexpr Y y(3);
            static_assert(opt.value_or(y) == 2);
        }
        {
            constexpr opt::option<X> opt(2);
            static_assert(opt.value_or(Y(3)) == 2);
        }
        {
            constexpr opt::option<X> opt;
            constexpr Y y(3);
            static_assert(opt.value_or(y) == 3);
        }
        {
            constexpr opt::option<X> opt;
            static_assert(opt.value_or(Y(3)) == 4);
        }
        {
            const opt::option<X> opt(2);
            const Y y(3);
            check(opt.value_or(y) == 2);
        }
        {
            const opt::option<X> opt(2);
            check(opt.value_or(Y(3)) == 2);
        }
        {
            const opt::option<X> opt;
            const Y y(3);
            check(opt.value_or(y) == 3);
        }
        {
            const opt::option<X> opt;
            check(opt.value_or(Y(3)) == 4);
        }
        {
            const opt::option<X> opt;
            check(opt.value_or({Y(3)}) == 4);
        }
        return true;
    }
} // namespace const_ref

namespace rvalue {
    struct Y {
        int i_;

        constexpr Y(int i) : i_(i) {}
    };

    struct X {
        int i_;

        constexpr X(int i) : i_(i) {}
        constexpr X(X&& x) : i_(x.i_) { x.i_ = 0; }
        constexpr X(const Y& y) : i_(y.i_) {}
        constexpr X(Y&& y) : i_(y.i_ + 1) {}
        friend constexpr bool operator==(const X& x, const X& y) {
            return x.i_ == y.i_;
        }
    };

    struct Z {
        int i_, j_;

        constexpr Z(int i, int j) : i_(i), j_(j) {}
        friend constexpr bool operator==(const Z& z1, const Z& z2) {
            return z1.i_ == z2.i_ && z1.j_ == z2.j_;
        }
    };

    constexpr bool test() {
        {
            opt::option<X> opt(std::in_place, 2);
            Y y(3);
            check(std::move(opt).value_or(y) == 2);
            SUPPRESS_WCONSUMED(
            check(*opt == 0);
            )
        }
        {
            opt::option<X> opt(std::in_place, 2);
            check(std::move(opt).value_or(Y(3)) == 2);
            SUPPRESS_WCONSUMED(
            check(*opt == 0);
            )
        }
        {
            opt::option<X> opt;
            Y y(3);
            check(std::move(opt).value_or(y) == 3);
            check(!opt.has_value());
        }
        {
            opt::option<X> opt;
            check(std::move(opt).value_or(Y(3)) == 4);
            check(!opt.has_value());
        }
        {
            opt::option<X> opt;
            check(std::move(opt).value_or({Y(3)}) == 4);
            check(!opt.has_value());
        }
        {
            opt::option<Z> opt;
            check((std::move(opt).value_or({2, 3}) == Z{2, 3}));
            check(!opt.has_value());
        }

        return true;
    }
} // namespace rvalue

// TODO: test_ref

namespace lwg3424_cv {
    // LWG 3424: value_or must return remove_cv_t<T>, never a cv-qualified type
    struct X {
        int i;
        constexpr X(int i_) : i(i_) {}
        constexpr X(const X&) = default;
        constexpr X(X&&) = default;
        friend constexpr bool operator==(const X& x, const X& y) = default;
    };

    constexpr bool test() {
        {
            constexpr opt::option<const int> opt(2);
            check_meta(opt.value_or(5), int, 2, !noexcept);
            constexpr opt::option<const int> empty;
            static_assert(empty.value_or(5) == 5);
        }
        {
            const opt::option<const X> opt(std::in_place, 2);
            check_meta(opt.value_or(X(3)), X, 2, !noexcept);
            const opt::option<const X> empty;
            check(empty.value_or(X(3)) == 3);
        }
        return true;
    }
} // namespace lwg3424_cv

namespace lwg4406_implicit {
    // LWG 4406: is_convertible_v<S, int> is true, but static_cast<int>(S{}) is ill-formed
    // (it would select the deleted explicit operator) - value_or must use implicit conversion only
    struct S {
        constexpr operator int() const { return 42; }
        explicit operator int() = delete;
    };

    constexpr bool test() {
        constexpr opt::option<int> empty;
        static_assert(empty.value_or(S{}) == 42);
        opt::option<int> opt;
        check(std::move(opt).value_or(S{}) == 42);
        return true;
    }
} // namespace lwg4406_implicit

namespace lwg4281_no_common_type {
    struct B;
    struct A {
        int i;
        constexpr A(int i_) : i(i_) {}
        constexpr operator B() const;
    };
    struct B {
        int i;
        constexpr B(int i_) : i(i_) {}
        [[maybe_unused]] constexpr operator A() const;
        friend constexpr bool operator==(const B& x, const B& y) { return x.i == y.i; }
    };
    constexpr A::operator B() const { return B{i}; }
    constexpr B::operator A() const { return A{i}; }

    constexpr bool test() {
        const opt::option<B> empty;
        check_meta(empty.value_or(A(7)), B, B{7}, !noexcept);
        return true;
    }
} // namespace lwg4281_no_common_type

namespace braces_and_const_rvalue {
    struct Z {
        int i, j;
        constexpr Z() : i{}, j{} {}
        constexpr Z(int i_, int j_) : i(i_), j(j_) {}
        friend constexpr bool operator==(const Z& z1, const Z& z2) { return z1.i == z2.i && z1.j == z2.j; }
    };
    struct X {
        int i;
        constexpr X(int i_) : i(i_) {}
        constexpr X(const X&) = default;
        constexpr X(X&& x) noexcept : i(x.i) { x.i = 0; }
        friend constexpr bool operator==(const X& x, const X& y) { return x.i == y.i; }
    };

    constexpr bool test() {
        {
            constexpr opt::option<Z> empty;
            static_assert(empty.value_or({}) == Z{});
            constexpr opt::option<Z> opt(std::in_place, 2, 3);
            static_assert(opt.value_or({}) == Z{2, 3});
        }
        {
            const opt::option<Z> opt;
            check((opt.value_or({2, 3}) == Z{2, 3}));
        }
        {
            const opt::option<X> opt(std::in_place, 2);
            check(std::move(opt).value_or(X(3)) == 2);
            SUPPRESS_WCONSUMED(
            check(*opt == 2);
            )
        }
        return true;
    }
} // namespace braces_and_const_rvalue

}

void value_or_test_suite() {
    static_assert(const_ref::test());
    const_ref::test();

    static_assert(rvalue::test());
    rvalue::test();

    static_assert(lwg3424_cv::test());
    lwg3424_cv::test();

    static_assert(lwg4406_implicit::test());
    lwg4406_implicit::test();

    static_assert(lwg4281_no_common_type::test());
    lwg4281_no_common_type::test();

    static_assert(braces_and_const_rvalue::test());
    braces_and_const_rvalue::test();
}
