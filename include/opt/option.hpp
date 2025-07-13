// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>
#include <utility>

#include "details/preprocessor.hpp"
#include "option_fwd.hpp"

#pragma warning(push)
#pragma warning(disable : \
    4820 /* warning C4820: 'bytes' bytes padding added after construct 'member_name' */ \
    4582 /* warning C4582: 'type': constructor is not implicitly called */ \
    )

namespace opt {

namespace impl {
    template<class T, bool trivially_destructible>
    struct option_destruct_base;

    template<class T>
    struct option_destruct_base<T, false> {
        union {
            char dummy;
            std::remove_cv_t<T> value;
        };
        bool engaged;

        OPTION_CONSTEXPR_CXX20 ~option_destruct_base() {
            if (engaged) {
                value.~T();
            }
        }
        constexpr option_destruct_base() noexcept
            : dummy{}, engaged{false} {}

        template<class... Args>
        constexpr option_destruct_base(std::in_place_t, Args&&... args)
            : value(std::forward<Args>(args)...), // use direct-initialization (not direct-list-initialization)
              engaged{true} {}

        constexpr void reset() {
            if (engaged) {
                value.~T();
                engaged = false;
            }
        }
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return engaged;
        }
    };

    template<class T>
    struct option_destruct_base<T, true> {
        union {
            char dummy;
            std::remove_cv_t<T> value;
        };
        bool engaged;

        constexpr option_destruct_base() noexcept
            : dummy{}, engaged{false} {}

        template<class... Args>
        constexpr option_destruct_base(std::in_place_t, Args&&... args)
            : value(std::forward<Args>(args)...), // use direct-initialization (not direct-list-initialization)
              engaged{true} {}

        constexpr void reset() {
            engaged = false;
        }
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return engaged;
        }
    };
}

struct standard {};

template<class T, class Traits = standard>
class option : private impl::option_destruct_base<T, std::is_trivially_destructible_v<T>> {
private:
    using base = impl::option_destruct_base<T, std::is_trivially_destructible_v<T>>;

    template<class, class>
    friend class option;
public:
    using value_type = T;

    constexpr option() noexcept {}
    constexpr option(none_t) noexcept {}

    template<class... Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
    constexpr explicit option(std::in_place_t, Args&&... args)
        : base(std::in_place, std::forward<Args>(args)...) {}

    template<class U, class... Args, std::enable_if_t<std::is_constructible_v<T, std::initializer_list<U>&, Args...>, int> = 0>
    constexpr explicit option(std::in_place_t, std::initializer_list<U> ilist, Args&&... args)
        : base(std::in_place, ilist, std::forward<Args>(args)...) {}

    constexpr option& operator=(none_t) noexcept {
        base::reset();
        return *this;
    }

    using base::has_value;
    using base::reset;

    [[nodiscard]] constexpr T& get() & OPTION_LIFETIMEBOUND {
        return base::value;
    }
    [[nodiscard]] constexpr const T& get() const& OPTION_LIFETIMEBOUND {
        return base::value;
    }
    [[nodiscard]] constexpr T&& get() && OPTION_LIFETIMEBOUND {
        return std::move(base::value);
    }
    [[nodiscard]] constexpr const T&& get() const&& OPTION_LIFETIMEBOUND {
        return std::move(base::value);
    }
};

template<class T>
option(T) -> option<T>;

}

#pragma warning(pop)
