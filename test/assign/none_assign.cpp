#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>

// option& operator=(none_t) noexcept;

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/nullopt_t.pass.cpp

namespace {

constexpr bool test() {
    enum class State { inactive, constructed, destroyed };
    State state = State::inactive;

    struct StateTracker {
        constexpr StateTracker(State& s OPTION_LIFETIMEBOUND) : state_(&s) {
            s = State::constructed;
        }
        constexpr ~StateTracker() { *state_ = State::destroyed; }

        State* state_;
    };
    {
        opt::option<int> opt;
        static_assert(noexcept(opt = opt::none));
        opt = opt::none;
        check(!opt.has_value());
    }
    {
        opt::option<int> opt(3);
        opt = opt::none;
        check(!opt.has_value());
    }
    {
        opt::option<StateTracker> opt;
        opt = opt::none;
        check(state == State::inactive);
        check(!opt.has_value());
    }
    {
        opt::option<StateTracker> opt(state);
        check(state == State::constructed);
        opt = opt::none;
        check(state == State::destroyed);
        check(!opt.has_value());
    }

    // TODO: test_ref
    return true;
}

}

void none_assign_test_suite() {
    static_assert(test());
    test();

    using TT = test_types::test_type;
    TT::reset();
    {
        opt::option<TT> opt;
        static_assert(noexcept(opt = opt::none));
        check(TT::destroyed == 0);
        opt = opt::none;
        check(TT::constructed == 0);
        check(TT::alive == 0);
        check(TT::destroyed == 0);
        check(!opt.has_value());
    }
    check(TT::alive == 0);
    check(TT::destroyed == 0);
    TT::reset();
    {
        opt::option<TT> opt(42);
        check(TT::destroyed == 0);
        TT::reset_constructors();
        opt = opt::none;
        check(TT::constructed == 0);
        check(TT::alive == 0);
        check(TT::destroyed == 1);
        check(!opt.has_value());
    }
    check(TT::alive == 0);
    check(TT::destroyed == 1);
    TT::reset();
}
