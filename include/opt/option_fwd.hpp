// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace opt {

template<class T, class Traits>
class option;

namespace impl {
    struct none_tag {};
}
struct none_t {
    constexpr none_t() = delete;
    constexpr explicit none_t(impl::none_tag) {}
};
inline constexpr none_t none{impl::none_tag{}};

}
