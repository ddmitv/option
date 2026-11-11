#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>
#include <array>
#include <memory>
#include <type_traits>
#include <utility>

// From LWG2451:
// template<typename U>
// option& operator=(option<U>&& rhs);

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/optional_U.pass.cpp

namespace {

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
struct X {
    static inline bool throw_now = false;

    X(int&&) {
        if (throw_now) {
            throw 6;
        }
    }
};
#endif

struct B {
    virtual ~B() = default;
};

class D : public B {};

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
        opt::option<int> other(42);
        opt = std::move(other);
        check(T::alive == 1);
        check(T::constructed == 1);
        check(T::value_constructed == 1);
        check(T::assigned == 0);
        check(T::destroyed == 0);
        check(other.has_value());
        SUPPRESS_WCONSUMED(
        check(*other == 42);
        )
        check(opt.has_value());
        check(*opt == T(42));
    }
    check(T::alive == 0);
    { // non-empty to non-empty
        opt::option<T> opt(101);
        opt::option<int> other(42);
        T::reset_constructors();
        opt = std::move(other);
        check(T::alive == 1);
        check(T::constructed == 0);
        check(T::assigned == 1);
        check(T::value_assigned == 1);
        check(T::destroyed == 0);
        check(other.has_value());
        SUPPRESS_WCONSUMED(
        check(*other == 42);
        )
        check(opt.has_value());
        check(*opt == T(42));
    }
    check(T::alive == 0);
    { // empty to non-empty
        opt::option<T> opt(101);
        opt::option<int> other;
        T::reset_constructors();
        opt = std::move(other);
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
        opt::option<int> other;
        T::reset_constructors();
        opt = std::move(other);
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
        using T = AssignableFrom<OptInt&&>;
        T::reset();
        {
            OptInt a(42);
            opt::option<T> t;
            t = std::move(a);
            check(T::type_constructed == 1);
            check(T::type_assigned == 0);
            check(T::int_constructed == 0);
            check(T::int_assigned == 0);
        }
        {
            using Opt = opt::option<T>;
            static_assert(!std::is_assignable_v<Opt&, const OptInt&&>);
            static_assert(!std::is_assignable_v<Opt&, const OptInt&>);
            static_assert(!std::is_assignable_v<Opt&, OptInt&>);
        }
    }
    {
        using T = AssignableFrom<OptInt const&&>;
        T::reset();
        {
            const OptInt a(42);
            opt::option<T> t;
            t = std::move(a);
            check(T::type_constructed == 1);
            check(T::type_assigned == 0);
            check(T::int_constructed == 0);
            check(T::int_assigned == 0);
        }
        T::reset();
        {
            OptInt a(42);
            opt::option<T> t;
            t = std::move(a);
            check(T::type_constructed == 1);
            check(T::type_assigned == 0);
            check(T::int_constructed == 0);
            check(T::int_assigned == 0);
        }
        {
            using Opt = opt::option<T>;
            static_assert(std::is_assignable_v<Opt&, OptInt&&>);
            static_assert(!std::is_assignable_v<Opt&, const OptInt&>);
            static_assert(!std::is_assignable_v<Opt&, OptInt&>);
        }
    }
}

constexpr bool test() {
    {
        opt::option<int> opt;
        opt::option<short> opt2;
        opt = std::move(opt2);
        check(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    {
        opt::option<int> opt;
        opt::option<short> opt2(short{2});
        opt = std::move(opt2);
        check(opt2.has_value());
        SUPPRESS_WCONSUMED(
        check(*opt2 == 2);
        )
        check(opt.has_value() == opt2.has_value());
        SUPPRESS_WCONSUMED(
        check(*opt == *opt2);
        )
    }
    {
        opt::option<int> opt(3);
        opt::option<short> opt2;
        opt = std::move(opt2);
        check(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    {
        opt::option<int> opt(3);
        opt::option<short> opt2(short{2});
        opt = std::move(opt2);
        check(opt2.has_value());
        SUPPRESS_WCONSUMED(
        check(*opt2 == 2);
        )
        check(opt.has_value() == opt2.has_value());
        SUPPRESS_WCONSUMED(
        check(*opt == *opt2);
        )
    }

    enum class state_t { inactive, constructed, copy_assigned, move_assigned };
    class StateTracker {
    public:
        constexpr StateTracker(state_t& s OPTION_LIFETIMEBOUND) : state_(&s) {
            *state_ = state_t::constructed;
        }

        StateTracker(StateTracker&&) = default;
        StateTracker(StateTracker const&) = default;

        constexpr StateTracker& operator=(StateTracker&& other) noexcept {
            *state_ = state_t::inactive;
            state_ = other.state_;
            *state_ = state_t::move_assigned;
            other.state_ = nullptr;
            return *this;
        }

        constexpr StateTracker& operator=(StateTracker const& other) noexcept {
            *state_ = state_t::inactive;
            state_ = other.state_;
            *state_ = state_t::copy_assigned;
            return *this;
        }
    private:
        state_t* state_;
    };
    {
        std::array<state_t, 2> state{{state_t::inactive, state_t::inactive}};
        auto opt1 = opt::option<StateTracker>(state[0]);
        check(state[0] == state_t::constructed);

        auto opt2 = opt::option<StateTracker>(state[1]);
        check(state[1] == state_t::constructed);

        opt1 = std::move(opt2);
        check(state[0] == state_t::inactive);
        check(state[1] == state_t::move_assigned);
    }
    {
        std::array<state_t, 2> state{{state_t::inactive, state_t::inactive}};
        auto opt1 = opt::option<StateTracker>(state[0]);
        check(state[0] == state_t::constructed);

        auto opt2 = opt::option<StateTracker>(state[1]);
        check(state[1] == state_t::constructed);

        opt1 = opt2;
        check(state[0] == state_t::inactive);
        check(state[1] == state_t::copy_assigned);
    }

    // TODO: test_ref
    return true;
}

}

void conv_move_assign_test_suite() {
    static_assert(test());
    test();
    test_with_test_type();
    test_ambiguous_assign();
    test();
    {
        opt::option<std::unique_ptr<B>> opt;
        opt::option<std::unique_ptr<D>> other(std::make_unique<D>());
        opt = std::move(other);
        check(opt.has_value());
        check(other.has_value());
        check(opt->get() != nullptr);
        SUPPRESS_WCONSUMED(
        check(other->get() == nullptr);
        )
    }
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    {
        opt::option<X> opt;
        opt::option<int> opt2(42);
        check(opt2.has_value());
        X::throw_now = true;
        check_throw_value(opt = std::move(opt2), int, 6);
        check(!opt.has_value());
        X::throw_now = false;
    }
#endif
}
