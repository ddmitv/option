#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>

// constexpr option& operator=(const option& rhs);

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/copy.pass.cpp

namespace {

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
struct X {
    static inline bool throw_now = false;

    [[maybe_unused]] X() = default;
    X(const X&) {
        if (throw_now) {
            throw 6;
        }
    }
    X& operator=(const X&) = default;
};
#endif

template<typename Tp>
constexpr bool assign_empty(opt::option<Tp>&& lhs) {
    const opt::option<Tp> rhs;
    lhs = rhs;
    return !lhs.has_value() && !rhs.has_value();
}

template<typename Tp>
constexpr bool assign_value(opt::option<Tp>&& lhs) {
    const opt::option<Tp> rhs(101);
    lhs = rhs;
    return lhs.has_value() && rhs.has_value() && *lhs == *rhs;
}

// TODO: test_ref

}

void copy_assign_test_suite() {
    {
        using O = opt::option<int>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        check(assign_empty(O{42}));
        check(assign_value(O{42}));
    }
    {
        using O = opt::option<trivial_test_types::all_ctors>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        check(assign_empty(O{42}));
        check(assign_value(O{42}));
    }
    {
        using O = opt::option<test_types::test_type>;
        check(assign_empty(O{42}));
        check(assign_value(O{42}));
    }
    {
        using T = test_types::test_type;
        T::reset();
        opt::option<T> opt(3);
        const opt::option<T> opt2;
        check(T::alive == 1);
        opt = opt2;
        check(T::alive == 0);
        check(!opt2.has_value());
        check(!opt.has_value());
    }
    test_types::test_type::reset();
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    {
        opt::option<X> opt;
        opt::option<X> opt2(X{});
        check(opt2.has_value());
        X::throw_now = true;
        check_throw_value(opt = opt2, int, 6);
        check(!opt.has_value());
        X::throw_now = false;
    }
#endif
}
