// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>
#include <utility>
#include <memory>
#include <cstring>
#include <exception>

#include "details/preprocessor.hpp"
#include "details/asan.hpp"

OPTION_SUPPRESS_COMMON_WARNINGS_PUSH

#include "option_fwd.hpp"

namespace opt {

// https://eel.is/c++draft/optional#nullops
[[nodiscard]] constexpr std::strong_ordering operator<=>(const none_t, const none_t) noexcept { return std::strong_ordering::equal; }

[[nodiscard]] constexpr bool operator==(const none_t, const none_t) noexcept { return true; }
[[nodiscard]] constexpr bool operator!=(const none_t, const none_t) noexcept { return false; }
[[nodiscard]] constexpr bool operator>(const none_t, const none_t) noexcept { return false; }
[[nodiscard]] constexpr bool operator<(const none_t, const none_t) noexcept { return false; }
[[nodiscard]] constexpr bool operator>=(const none_t, const none_t) noexcept { return true; }
[[nodiscard]] constexpr bool operator<=(const none_t, const none_t) noexcept { return true; }

class bad_access final : public std::exception {
public:
    bad_access() = default;
    bad_access(const bad_access&) = default;
    bad_access& operator=(const bad_access&) = default;

    ~bad_access() override = default;
    [[nodiscard]] const char* what() const noexcept override {
        return "Bad opt::option access";
    }
};

namespace impl {
    [[noreturn]] inline void throw_bad_option() {
#if OPTION_HAS_EXCEPTIONS
        throw bad_access{};
#else
        OPTION_VERIFY(false, "opt::bad_access was thrown but exceptions were disabled");
#endif
    }
}

namespace impl {
    // from https://github.com/microsoft/STL/blob/da52dc0fcadbd83690cfd1ed32408d2847439493/stl/inc/type_traits#L2295
#if OPTION_CLANG || defined(__EDG__) // TRANSITION, VSO-1681199
    void swap() = delete; // Block unqualified name lookup
#else // ^^^ no workaround / workaround vvv
    void swap();
#endif // ^^^ workaround ^^^

    template<typename T>
    inline constexpr bool is_trivially_swappable_v =
        OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)
        && OPTION_IS_TRIVIALLY_COPYABLE(T)
        && OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, T)
        && OPTION_IS_TRIVIALLY_ASSIGNABLE(T&, T)
        && !requires(T& x) { swap(x, x); };

    template<>
    inline constexpr bool is_trivially_swappable_v<std::byte> = true;

    template<class T>
    struct option_storage {
        union {
            OPTION_REMOVE_CV(T) value;
            // a dummy union member is needed so the union has an active member during constant evaluation
            // (see GCC PR 110245 rejects the empty-union constexpr default ctor)
            // the dummy member is activated only in the constant evaluation branch of the set_unengaged(),
            // so it is not initialized at runtime and does not affect codegen.
            char dummy;
        };
        bool engaged; // uninit

        constexpr ~option_storage() requires (!OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)) {
            if (engaged) {
                value.~T();

#if OPTION_DESTRUCTOR_TOMBSTONES
                // set state to empty to resist use-after-free mistakes
                engaged = false;
#endif
            } else {
                // https://github.com/microsoft/STL/pull/6010
                // https://github.com/llvm/llvm-project/blob/main/compiler-rt/include/sanitizer/asan_interface.h
#if OPTION_INSERT_ANNONATION
                // Unpoison our storage when the std::optional is destroyed.
                // We're giving up control of these bytes, which can be validly reused for something else.
                if (!std::is_constant_evaluated()) {
                    ::__asan_unpoison_memory_region(OPTION_ADDRESSOF(this->value), sizeof(T));
                }
#endif
            }
        }
        constexpr ~option_storage() = default;

        constexpr option_storage() noexcept {
            set_unengaged();
        }

        constexpr option_storage(const option_storage& other)
            requires(OPTION_IS_CONSTRUCTIBLE(T, const T&) && !OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, const T&))
        {
            if (other.has_value()) {
                this->construct(other.value);
            } else {
                set_unengaged();
            }
        }
        constexpr option_storage(const option_storage&) requires(OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, const T&)) = default;

        constexpr option_storage(option_storage&& other)
            noexcept(OPTION_IS_NOTHROW_CONSTRUCTIBLE(T, T&&))
            requires(OPTION_IS_CONSTRUCTIBLE(T, T&&) && !OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, T&&))
        {
            if (other.has_value()) {
                this->construct(static_cast<option_storage&&>(other).value);
            } else {
                set_unengaged();
            }
        }
        constexpr option_storage(option_storage&&) requires(OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, T&&)) = default;

        constexpr option_storage& operator=(const option_storage& other)
            requires(OPTION_IS_CONSTRUCTIBLE(T, const T&) && OPTION_IS_ASSIGNABLE(T&, const T&)
                && !(OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, const T&) && OPTION_IS_TRIVIALLY_ASSIGNABLE(T&, const T&) && OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)))
        {
            if (this->engaged == other.engaged) {
                if (this->engaged) {
                    this->value = other.value;
                }
            } else {
                if (this->engaged) {
                    this->reset();
                } else {
                    this->construct(other.value);
                }
            }
            return *this;
        }
        constexpr option_storage& operator=(const option_storage& other) requires(OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, const T&) && OPTION_IS_TRIVIALLY_ASSIGNABLE(T&, const T&) && OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T))
            = default;

        constexpr option_storage& operator=(option_storage&& other)
            noexcept(OPTION_IS_NOTHROW_ASSIGNABLE(T&, T&&) && OPTION_IS_NOTHROW_CONSTRUCTIBLE(T, T&&))
            requires(OPTION_IS_CONSTRUCTIBLE(T, T&&) && OPTION_IS_ASSIGNABLE(T&, T&&)
                && !(OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, T&&) && OPTION_IS_TRIVIALLY_ASSIGNABLE(T&, T&&) && OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)))
        {
            if (this->engaged == other.engaged) {
                if (this->engaged) {
                    this->value = static_cast<T&&>(other.value);
                }
            } else {
                if (this->engaged) {
                    this->reset();
                } else {
                    this->construct(static_cast<T&&>(other.value));
                }
            }
            return *this;
        }
        constexpr option_storage& operator=(option_storage&& other) requires(OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, T&&) && OPTION_IS_TRIVIALLY_ASSIGNABLE(T&, T&&) && OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T))
            = default;

        template<class... Args>
        constexpr option_storage(std::in_place_t, Args&&... args)
            // use direct-initialization (not direct-list-initialization)
            : value(static_cast<Args&&>(args)...),
              engaged{true} {}

        constexpr void reset() noexcept {
            if constexpr (OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)) {
                engaged = false;
            } else {
                if (engaged) {
                    value.~T();
                    set_unengaged();
                }
            }
        }
        template<typename That>
        constexpr void construct_from_another(That&& other) {
            if constexpr (OPTION_IS_INTEGRAL(T) && OPTION_IS_INTEGRAL(decltype(other.value))) {
                // Integer -> integer conversion is cheap, total and cannot fault, so skip the branch:
                // convert the (possibly never written) source value unconditionally and copy the flag as is.
                // Not usable in constant evaluation, where reading an inactive union member is an error.
                if (!std::is_constant_evaluated()) {
                    std::construct_at(OPTION_ADDRESSOF(this->value), other.value);
                    this->engaged = other.has_value();
                    return;
                }
            }

            if (other.has_value()) {
                this->construct(static_cast<That&&>(other).value);
            }
        }

        template<typename That>
        constexpr void assign_from_another(That&& other) {
            if constexpr (OPTION_IS_INTEGRAL(T) && OPTION_IS_INTEGRAL(decltype(other.value))) {
                // Integer -> integer conversion is total, side-effect free, and cannot
                // fault, so we can assign both the (possibly indeterminate) payload
                // and the state byte unconditionally — no branch, no reset/construct.
                // Not valid in constant evaluation: reading an unengaged source's
                // payload is UB there.
                if (!std::is_constant_evaluated()) {
                    this->value = other.value;
                    this->engaged = other.has_value();
                    return;
                }
            }

            if (other.has_value()) {
                if (this->has_value()) {
                    this->value = static_cast<That&&>(other).value;
                    OPTION_VERIFY(this->has_value(), "after assignment, the value is in an empty state");
                } else {
                    this->construct(static_cast<That&&>(other).value);
                }
            } else {
                this->reset();
            }
        }

        constexpr void set_unengaged() noexcept {
            if (std::is_constant_evaluated()) {
                // this is only evaluated for constexpr contexts because it produces poor codegen otherwise
                dummy = 0;
            }
            this->engaged = false;
#if OPTION_INSERT_ANNONATION
            if constexpr (!OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)) {
                // Poison our storage when starting empty, until that changes or the std::optional is destroyed.
                if (!std::is_constant_evaluated()) {
                    ::__asan_poison_memory_region(OPTION_ADDRESSOF(this->value), sizeof(T));
                }
            }
#endif
        }

        template<typename... Args>
        constexpr void construct(Args&&... args) {
#if OPTION_INSERT_ANNONATION
            // Only the non-trivially destructible case can use ASan annotations, as the destructor is needed to undo them.
            if constexpr (!OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)) {
                // Unpoison our storage when becoming non-empty.
                if (!std::is_constant_evaluated()) {
                    ::__asan_unpoison_memory_region(OPTION_ADDRESSOF(this->value), sizeof(T));
                }
            }
#endif
            // if T's ctor throws an exception, the option must be unengaged (this->engaged = true MUST happen after std::construct_at call)
            std::construct_at(OPTION_ADDRESSOF(this->value), static_cast<Args&&>(args)...);
            this->engaged = true;
        }

        [[nodiscard]] OPTION_PURE constexpr bool has_value() const noexcept {
            return engaged;
        }
    };

    // https://eel.is/c++draft/optional#iterators
    template<typename T>
    class option_iterator {
        T* ptr{};

        template<typename>
        friend class option_iterator;

        template<typename, typename>
        friend class opt::option;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::contiguous_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using value_type = OPTION_REMOVE_CV(T);

        option_iterator() = default;

        template<typename U> requires OPTION_IS_CONVERTIBLE(U*, T*)
        constexpr option_iterator(const option_iterator<U>& other) noexcept : ptr{other.ptr} {}
    private:
        // only opt::option can construct option_iterator from a raw pointer from outside
        constexpr explicit option_iterator(T* const ptr_) noexcept : ptr{ptr_} {}
    public:
        [[nodiscard]] constexpr reference operator*() const noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot dereference value-initialized option_iterator");
            return *ptr;
        }
        [[nodiscard]] constexpr pointer operator->() const noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot dereference value-initialized option_iterator");
            return ptr;
        }
        constexpr option_iterator& operator++() noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot increment value-initialized option_iterator");
            ++ptr;
            return *this;
        }
        constexpr option_iterator operator++(int) noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot increment value-initialized option_iterator");
            option_iterator tmp{*this};
            ++ptr;
            return tmp;
        }
        constexpr option_iterator& operator--() noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot decrement value-initialized option_iterator");
            --ptr;
            return *this;
        }
        constexpr option_iterator operator--(int) noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot decrement value-initialized option_iterator");
            option_iterator tmp{*this};
            --ptr;
            return tmp;
        }
        constexpr option_iterator& operator+=(const difference_type offset) noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot seek value-initialized option_iterator");
            OPTION_VERIFY(offset >= -1 && offset <= 1, "cannot seek option_iterator beyond bounds [-1, 1]");
            ptr += offset;
            return *this;
        }
        constexpr option_iterator& operator-=(const difference_type offset) noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot seek value-initialized option_iterator");
            OPTION_VERIFY(offset >= -1 && offset <= 1, "cannot seek option_iterator beyond bounds [-1, 1]");
            ptr -= offset;
            return *this;
        }
        [[nodiscard]] constexpr reference operator[](const difference_type offset) const noexcept {
            OPTION_VERIFY(ptr != nullptr, "cannot dereference value-initialized option_iterator");
            OPTION_VERIFY(offset == -1 || offset == 0, "cannot dereference option_iterator with offset other than -1 or 0");
            return ptr[offset];
        }

        // https://eel.is/c++draft/forward.iterators#2
        // comparing value-initialized iterator with non-value-initialized is UB
        [[nodiscard]] friend constexpr bool operator==(const option_iterator& left, const option_iterator& right) noexcept {
            OPTION_VERIFY((left.ptr == nullptr) == (right.ptr == nullptr), "cannot compare value-initialized option_iterator with non-value-initialized option_iterator");
            OPTION_VERIFY((left.ptr - right.ptr) >= -1 && (left.ptr - right.ptr) <= 1, "cannot compare option iterators from different option containers");
            return left.ptr == right.ptr;
        }
        [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const option_iterator& left, const option_iterator& right) noexcept {
            OPTION_VERIFY((left.ptr == nullptr) == (right.ptr == nullptr), "cannot compare value-initialized option_iterator with non-value-initialized option_iterator");
            OPTION_VERIFY((left.ptr - right.ptr) >= -1 && (left.ptr - right.ptr) <= 1, "cannot compare option iterators from different option containers");
            return left.ptr <=> right.ptr;
        }
        [[nodiscard]] friend constexpr option_iterator operator+(const option_iterator& it, const difference_type offset) noexcept {
            OPTION_VERIFY(it.ptr != nullptr, "cannot seek value-initialized option_iterator");
            OPTION_VERIFY(offset >= -1 && offset <= 1, "cannot seek option_iterator beyond bounds [-1, 1]");
            return option_iterator{it.ptr + offset};
        }
        [[nodiscard]] friend constexpr option_iterator operator+(const difference_type offset, const option_iterator& it) noexcept {
            OPTION_VERIFY(it.ptr != nullptr, "cannot seek value-initialized option_iterator");
            OPTION_VERIFY(offset >= -1 && offset <= 1, "cannot seek option_iterator beyond bounds [-1, 1]");
            return option_iterator{it.ptr + offset};
        }
        [[nodiscard]] friend constexpr option_iterator operator-(const option_iterator& it, const difference_type offset) noexcept {
            OPTION_VERIFY(it.ptr != nullptr, "cannot seek value-initialized option_iterator");
            OPTION_VERIFY(offset >= -1 && offset <= 1, "cannot seek option_iterator beyond bounds [-1, 1]");
            return option_iterator{it.ptr - offset};
        }
        [[nodiscard]] friend constexpr difference_type operator-(const option_iterator& left, const option_iterator& right) noexcept {
            OPTION_VERIFY((left.ptr == nullptr) == (right.ptr == nullptr), "cannot compare value-initialized option_iterator with non-value-initialized option_iterator");
            OPTION_VERIFY((left.ptr - right.ptr) >= -1 && (left.ptr - right.ptr) <= 1, "cannot compare option iterators from different option containers");
            return left.ptr - right.ptr;
        }
    };
}

struct standard {};

// https://eel.is/c++draft/optional

template<class T, class Traits = standard>
class OPTION_CONSUMABLE(unknown) option final {
    // consumed == no value / disengaged
    // unconsumed == has value / engaged
    // unknown == maybe no value or has value / maybe engaged or disengaged

    using storage_t = impl::option_storage<T>;

    storage_t storage;

    template<class, class>
    friend class option;
public:
    using value_type = T;
    // https://eel.is/c++draft/optional#iterators
    using iterator = impl::option_iterator<T>;
    using const_iterator = impl::option_iterator<const T>;

    // https://eel.is/c++draft/optional.optional#general-2

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor
    // not trivially default constructible ("= default") since it produces worse codegen than non trivial one
    OPTION_RETURN_TYPESTATE(consumed)
    constexpr option() noexcept {}
    OPTION_RETURN_TYPESTATE(consumed)
    constexpr option(none_t) noexcept {}

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor_
    OPTION_RETURN_TYPESTATE(unknown)
    option(const option&) = default;
    // https://eel.is/c++draft/optional.optional#lib:optional,constructor__
    OPTION_RETURN_TYPESTATE(unknown)
    option(option&&) = default;

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor_____
    template<typename U = OPTION_REMOVE_CV(T)> requires(
        OPTION_IS_CONSTRUCTIBLE(T, U)
        && !OPTION_IS_SAME(OPTION_REMOVE_CVREF(U), std::in_place_t)
        && !OPTION_IS_SAME(OPTION_REMOVE_CVREF(U), option)
        && !(OPTION_IS_SAME(OPTION_REMOVE_CVREF(T), bool) && opt::is_option_v<OPTION_REMOVE_CVREF(U)>)
    )
    OPTION_RETURN_TYPESTATE(unconsumed)
    constexpr explicit(!OPTION_IS_CONVERTIBLE(U, T)) option(U&& value)
        : storage(std::in_place, static_cast<U&&>(value)) {}

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor______
    template<typename U> requires(
        OPTION_IS_CONSTRUCTIBLE(T, const U&)
        && !OPTION_IS_SAME(OPTION_REMOVE_CVREF(T), bool) && !(
            OPTION_IS_CONSTRUCTIBLE(T, const option<U>) || OPTION_IS_CONSTRUCTIBLE(T, const option<U>&) ||
            OPTION_IS_CONSTRUCTIBLE(T, option<U>) || OPTION_IS_CONSTRUCTIBLE(T, option<U>&) ||
            OPTION_IS_CONVERTIBLE(const option<U>, T) || OPTION_IS_CONVERTIBLE(const option<U>&, T) ||
            OPTION_IS_CONVERTIBLE(option<U>, T) || OPTION_IS_CONVERTIBLE(option<U>&, T)
        )
    )
    OPTION_RETURN_TYPESTATE(unknown)
    constexpr explicit(!OPTION_IS_CONVERTIBLE(const U&, T)) option(const option<U>& other) {
        this->storage.construct_from_another(other.storage);
    }

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor_______
    template<typename U> requires(
        OPTION_IS_CONSTRUCTIBLE(T, U)
        && !OPTION_IS_SAME(OPTION_REMOVE_CVREF(T), bool) && !(
            OPTION_IS_CONSTRUCTIBLE(T, const option<U>) || OPTION_IS_CONSTRUCTIBLE(T, const option<U>&) ||
            OPTION_IS_CONSTRUCTIBLE(T, option<U>) || OPTION_IS_CONSTRUCTIBLE(T, option<U>&) ||
            OPTION_IS_CONVERTIBLE(const option<U>, T) || OPTION_IS_CONVERTIBLE(const option<U>&, T) ||
            OPTION_IS_CONVERTIBLE(option<U>, T) || OPTION_IS_CONVERTIBLE(option<U>&, T)
        )
    )
    OPTION_RETURN_TYPESTATE(unknown)
    constexpr explicit(!OPTION_IS_CONVERTIBLE(U, T)) option(option<U>&& other) {
        this->storage.construct_from_another(static_cast<option<U>&&>(other).storage);
    }

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor___
    template<class... Args> requires(OPTION_IS_CONSTRUCTIBLE(T, Args...))
    OPTION_RETURN_TYPESTATE(unconsumed)
    constexpr explicit option(std::in_place_t, Args&&... args)
        : storage(std::in_place, static_cast<Args&&>(args)...) {}

    // https://eel.is/c++draft/optional.optional#lib:optional,constructor____
    template<class U, class... Args> requires(OPTION_IS_CONSTRUCTIBLE(T, std::initializer_list<U>&, Args...))
    OPTION_RETURN_TYPESTATE(unconsumed)
    constexpr explicit option(std::in_place_t, std::initializer_list<U> ilist, Args&&... args)
        : storage(std::in_place, ilist, static_cast<Args&&>(args)...) {}

    // https://eel.is/c++draft/optional.optional#lib:operator=,optional_
    OPTION_SET_TYPESTATE(unknown)
    option& operator=(const option&) = default;
    // https://eel.is/c++draft/optional.optional#lib:operator=,optional__
    OPTION_SET_TYPESTATE(unknown)
    option& operator=(option&&) = default;

    // https://eel.is/c++draft/optional.optional#lib:operator=,optional
    OPTION_SET_TYPESTATE(consumed)
    constexpr option& operator=(none_t) noexcept {
        storage.reset();
        return *this;
    }

    // https://eel.is/c++draft/optional.optional#lib:operator=,optional___
    template<typename U = OPTION_REMOVE_CV(T)> requires(
        !OPTION_IS_SAME(OPTION_REMOVE_CVREF(U), option)
        && OPTION_IS_CONSTRUCTIBLE(T, U) && OPTION_IS_ASSIGNABLE(T&, U)
        && !(OPTION_IS_SCALAR(T) && OPTION_IS_SAME(T, OPTION_DECAY(U)))
    )
    OPTION_SET_TYPESTATE(unconsumed)
    constexpr option& operator=(U&& value) {
        if constexpr (OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)
                  && OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(T, U)
                  && OPTION_IS_TRIVIALLY_ASSIGNABLE(T&, U)) {
            // Constructing over a live trivially-destructible object is fine, and
            // trivial construct == trivial assign, so there's no reason to branch.
            // improves codegen
            this->storage.construct(static_cast<U&&>(value));
        } else {
            if (this->has_value()) {
                this->storage.value = static_cast<U&&>(value);
                OPTION_VERIFY(this->has_value(), "after assignment, the value is in an empty state");
            } else {
                this->storage.construct(static_cast<U&&>(value));
            }
        }
        return *this;
    }

    // https://eel.is/c++draft/optional.optional#lib:operator=,optional____
    template<typename U> requires(
        OPTION_IS_CONSTRUCTIBLE(T, const U&) && OPTION_IS_ASSIGNABLE(T&, const U&)
        && !(
            OPTION_IS_CONSTRUCTIBLE(T, const option<U>) || OPTION_IS_CONSTRUCTIBLE(T, const option<U>&) ||
            OPTION_IS_CONSTRUCTIBLE(T, option<U>) || OPTION_IS_CONSTRUCTIBLE(T, option<U>&) ||
            OPTION_IS_CONVERTIBLE(const option<U>, T) || OPTION_IS_CONVERTIBLE(const option<U>&, T) ||
            OPTION_IS_CONVERTIBLE(option<U>, T) || OPTION_IS_CONVERTIBLE(option<U>&, T) ||
            OPTION_IS_ASSIGNABLE(T&, const option<U>) || OPTION_IS_ASSIGNABLE(T&, const option<U>&) ||
            OPTION_IS_ASSIGNABLE(T&, option<U>) || OPTION_IS_ASSIGNABLE(T&, option<U>&)
        )
    )
    OPTION_SET_TYPESTATE(unknown)
    constexpr option& operator=(const option<U>& other) {
        this->storage.assign_from_another(other.storage);
        return *this;
    }

    // https://eel.is/c++draft/optional.optional#lib:operator=,optional_____
    template<typename U> requires(
        OPTION_IS_CONSTRUCTIBLE(T, U) && OPTION_IS_ASSIGNABLE(T&, U)
        && !(
            OPTION_IS_CONSTRUCTIBLE(T, const option<U>) || OPTION_IS_CONSTRUCTIBLE(T, const option<U>&) ||
            OPTION_IS_CONSTRUCTIBLE(T, option<U>) || OPTION_IS_CONSTRUCTIBLE(T, option<U>&) ||
            OPTION_IS_CONVERTIBLE(const option<U>, T) || OPTION_IS_CONVERTIBLE(const option<U>&, T) ||
            OPTION_IS_CONVERTIBLE(option<U>, T) || OPTION_IS_CONVERTIBLE(option<U>&, T) ||
            OPTION_IS_ASSIGNABLE(T&, const option<U>) || OPTION_IS_ASSIGNABLE(T&, const option<U>&) ||
            OPTION_IS_ASSIGNABLE(T&, option<U>) || OPTION_IS_ASSIGNABLE(T&, option<U>&)
        )
    )
    OPTION_SET_TYPESTATE(unknown)
    constexpr option& operator=(option<U>&& other) {
        this->storage.assign_from_another(static_cast<option<U>&&>(other).storage);
        return *this;
    }

    // https://eel.is/c++draft/optional.optional#lib:has_value,optional
    OPTION_TEST_TYPESTATE(unconsumed) OPTION_PURE
    [[nodiscard]] constexpr bool has_value() const noexcept {
        return this->storage.has_value();
    }

    // https://eel.is/c++draft/optional.optional#lib:reset,optional
    OPTION_SET_TYPESTATE(consumed)
    constexpr void reset() noexcept {
        this->storage.reset();
    }

    // https://eel.is/c++draft/optional.assign#lib:emplace,optional
    template<typename... Args> requires(OPTION_IS_CONSTRUCTIBLE(T, Args...))
    OPTION_SET_TYPESTATE(unconsumed)
    constexpr T& emplace(Args&&... args) {
        // if exception is thrown during the call to T's ctor, option must not contain a value and the previous value must be dtored.
        this->storage.reset();
        this->storage.construct(static_cast<Args&&>(args)...);
        return this->storage.value;
    }
    // https://eel.is/c++draft/optional.assign#lib:emplace,optional_
    template<typename U, typename... Args> requires(OPTION_IS_CONSTRUCTIBLE(T, std::initializer_list<U>&, Args...))
    OPTION_SET_TYPESTATE(unconsumed)
    constexpr T& emplace(std::initializer_list<U> ilist, Args&&... args) {
        this->storage.reset();
        this->storage.construct(ilist, static_cast<Args&&>(args)...);
        return this->storage.value;
    }

    // https://eel.is/c++draft/optional.swap
    OPTION_SET_TYPESTATE(unknown)
    constexpr void swap(option& other) noexcept(OPTION_IS_NOTHROW_CONSTRUCTIBLE(T) && std::is_nothrow_swappable_v<T>) {
        // https://github.com/microsoft/STL/blob/da52dc0fcadbd83690cfd1ed32408d2847439493/stl/inc/optional#L422-L447

        if constexpr (impl::is_trivially_swappable_v<T>) {
            if (!std::is_constant_evaluated()) {
                // for trivially swappable types improves codegen

                // static_cast<void*> and static_cast<const void*> are to avoid -Wclass-memaccess warning on GCC
                alignas(storage_t) unsigned char tmp[sizeof(storage_t)];
                std::memcpy(static_cast<void*>(&tmp), static_cast<const void*>(&this->storage), sizeof(storage_t));
                std::memcpy(static_cast<void*>(&this->storage), static_cast<const void*>(&other.storage), sizeof(storage_t));
                std::memcpy(static_cast<void*>(&other.storage), static_cast<const void*>(&tmp), sizeof(storage_t));
                return;
            }
        }
        if (this->has_value() == other.has_value()) {
            if (this->has_value()) {
                using std::swap;
                swap(this->storage.value, other.storage.value);
            }
        } else {
            if (this->has_value()) {
                other.storage.construct(static_cast<option&&>(*this).storage.value);
                this->reset();
            } else {
                this->storage.construct(static_cast<option&&>(other).storage.value);
                other.storage.reset();
            }
        }
    }

    // https://eel.is/c++draft/optional#iterators
    [[nodiscard]] constexpr iterator begin() noexcept OPTION_LIFETIMEBOUND {
        return iterator{OPTION_ADDRESSOF(this->storage.value)};
    }
    [[nodiscard]] constexpr const_iterator begin() const noexcept OPTION_LIFETIMEBOUND {
        return const_iterator{OPTION_ADDRESSOF(this->storage.value)};
    }
    [[nodiscard]] constexpr iterator end() noexcept OPTION_LIFETIMEBOUND {
        // NOTE: written as begin() + has_value() (the standard's literal wording, see https://eel.is/c++draft/optional#iterators)
        // instead of iterator{OPTION_ADDRESSOF(storage.value) + has_value()}. Both are semantically equivalent, but the latter performs pointer
        // arithmetic on the unqualified T* and only then converts to const T*, which GCC >= 8.1 fails to constant-evaluate for const-qualified
        // option_iterator (see GCC PR c++/XXXXXX). Doing the arithmetic on the already-converted const T* stored in the iterator avoids that.
        return begin() + (this->storage.has_value() ? 1 : 0);
    }
    [[nodiscard]] constexpr const_iterator end() const noexcept OPTION_LIFETIMEBOUND {
        return begin() + (this->storage.has_value() ? 1 : 0); // same as above
    }

    // not sure if i should remove "unknown" state available since it may be useful to detect bugs on accessing option from function argument
    // which's state is unknown
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr T& get() & OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot access value of empty opt::option");
        return this->storage.value;
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr const T& get() const& OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot access value of empty opt::option");
        return this->storage.value;
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr T&& get() && OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot access value of empty opt::option");
        return std::move(this->storage.value);
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr const T&& get() const&& OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot access value of empty opt::option");
        return std::move(this->storage.value);
    }

    // on purpose no clang's consumed annotation attributes
    [[nodiscard]] OPTION_PURE constexpr T& get_unchecked() & OPTION_LIFETIMEBOUND {
        return this->storage.value;
    }
    [[nodiscard]] OPTION_PURE constexpr const T& get_unchecked() const& OPTION_LIFETIMEBOUND {
        return this->storage.value;
    }
    [[nodiscard]] OPTION_PURE constexpr T&& get_unchecked() && OPTION_LIFETIMEBOUND {
        return std::move(this->storage.value);
    }
    [[nodiscard]] OPTION_PURE constexpr const T&& get_unchecked() const&& OPTION_LIFETIMEBOUND {
        return std::move(this->storage.value);
    }

    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr const T* operator->() const noexcept OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot dereference empty opt::option");
        return OPTION_ADDRESSOF(this->storage.value);
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr T* operator->() noexcept OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot dereference empty opt::option");
        return OPTION_ADDRESSOF(this->storage.value);
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr T& operator*() & noexcept OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot dereference empty opt::option");
        return this->storage.value;
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr const T& operator*() const& noexcept OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot dereference empty opt::option");
        return this->storage.value;
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr T&& operator*() && noexcept OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot dereference empty opt::option");
        return static_cast<T&&>(this->storage.value);
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown") OPTION_PURE
    [[nodiscard]] constexpr const T&& operator*() const&& noexcept OPTION_LIFETIMEBOUND {
        OPTION_VERIFY(this->storage.has_value(), "cannot dereference empty opt::option");
        return static_cast<const T&&>(this->storage.value);
    }

    // https://eel.is/c++draft/optional.optional#optional.observe
    // no OPTION_PURE since clang treats it as nounwind in IR which means throwing is UB
    OPTION_CALLABLE_WHEN("unconsumed", "unknown")
    [[nodiscard]] constexpr T& value() & OPTION_LIFETIMEBOUND {
        if (!this->storage.has_value()) {
            impl::throw_bad_option();
        }
        return this->storage.value;
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown")
    [[nodiscard]] constexpr const T& value() const& OPTION_LIFETIMEBOUND {
        if (!this->storage.has_value()) {
            impl::throw_bad_option();
        }
        return this->storage.value;
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown")
    [[nodiscard]] constexpr T&& value() && OPTION_LIFETIMEBOUND {
        if (!this->storage.has_value()) {
            impl::throw_bad_option();
        }
        return static_cast<T&&>(this->storage.value);
    }
    OPTION_CALLABLE_WHEN("unconsumed", "unknown")
    [[nodiscard]] constexpr const T&& value() const&& OPTION_LIFETIMEBOUND {
        if (!this->storage.has_value()) {
            impl::throw_bad_option();
        }
        return static_cast<const T&&>(this->storage.value);
    }
};

template<class T>
option(T) -> option<T>;

namespace impl {
    template<typename From, typename To>
    concept core_convertible_to = requires(void (*consumer)(To), From (*producer)()) {
        // producer() returns prvalue instead of rvalue like std::declval does
        consumer(producer());
    };
}

// https://eel.is/c++draft/optional#lib:operator==,optional
template<typename T1, typename T2> OPTION_NO_SANITIZE("bool")
constexpr bool operator==(const option<T1>& x, const option<T2>& y) requires requires {
    { x.get() == y.get() } -> impl::core_convertible_to<bool>;
} {
    if constexpr (OPTION_IS_INTEGRAL(T1) && OPTION_IS_INTEGRAL(T2)) {
        if (!std::is_constant_evaluated()) {
            // improves codegen (requires no_sanitize("bool") attribute)
            return ((x.has_value() ^ y.has_value()) ^ 1) & (!x.has_value() | (x.get_unchecked() == y.get_unchecked()));
        }
    }
    if (!x.has_value()) {
        return !y.has_value();
    }
    if (!y.has_value()) {
        return false;
    }
    return x.get() == y.get();
}
// https://eel.is/c++draft/optional#lib:operator!=,optional
template<typename T1, typename T2>
constexpr bool operator!=(const option<T1>& x, const option<T2>& y) requires requires {
    { x.get() != y.get() } -> impl::core_convertible_to<bool>;
} {
    if (!x.has_value()) {
        return y.has_value();
    }
    if (!y.has_value()) {
        return true;
    }
    return x.get() != y.get();
}

template<typename T1, typename T2>
constexpr bool operator<(const option<T1>& x, const option<T2>& y) requires requires {
    { x.get() < y.get() } -> impl::core_convertible_to<bool>;
} {
    if (!y.has_value()) {
        return false;
    }
    if (!x.has_value()) {
        return true;
    }
    return x.get() < y.get();
}

template<typename T1, typename T2>
constexpr bool operator>(const option<T1>& x, const option<T2>& y) requires requires {
    { x.get() > y.get() } -> impl::core_convertible_to<bool>;
} {
    if (!x.has_value()) {
        return false;
    }
    if (!y.has_value()) {
        return true;
    }
    return x.get() > y.get();
}

template<typename T1, typename T2>
constexpr bool operator<=(const option<T1>& x, const option<T2>& y) requires requires {
    { x.get() <= y.get() } -> impl::core_convertible_to<bool>;
} {
    if (!x.has_value()) {
        return true;
    }
    if (!y.has_value()) {
        return false;
    }
    return x.get() <= y.get();
}

template<typename T1, typename T2>
constexpr bool operator>=(const option<T1>& x, const option<T2>& y) requires requires {
    { x.get() >= y.get() } -> impl::core_convertible_to<bool>;
} {
    if (!y.has_value()) {
        return true;
    }
    if (!x.has_value()) {
        return false;
    }
    return x.get() >= y.get();
}

// https://eel.is/c++draft/optional#lib:operator%3c=%3e,optional
template<typename T1, typename T2> requires std::three_way_comparable_with<T1, T2>
constexpr std::compare_three_way_result_t<T1, T2> operator<=>(const option<T1>& x, const option<T2>& y) {
    if (x.has_value() && y.has_value()) {
        return x.get() <=> y.get();
    }
    return x.has_value() <=> y.has_value();
}

// https://eel.is/c++draft/optional#lib:operator==,optional_
template<typename T>
constexpr bool operator==(const option<T>& x, const none_t) noexcept {
    return !x.has_value();
}

// https://eel.is/c++draft/optional#lib:operator%3c=%3e,optional_
template<typename T>
constexpr std::strong_ordering operator<=>(const option<T>& x, const none_t) noexcept {
    return x.has_value() <=> false;
}


// https://eel.is/c++draft/optional.comp.with.t

// should use impl::is_derived_from_option instead of is_option_v for comparison operators with T?

template<typename T1, typename T2> OPTION_NO_SANITIZE("bool")
constexpr bool operator==(const option<T1>& x, const T2& y) requires (!is_option_v<T2>) && requires {
    { x.get() == y } -> impl::core_convertible_to<bool>;
} {
    if constexpr (OPTION_IS_INTEGRAL(T1) && OPTION_IS_INTEGRAL(T2)) {
        if (!std::is_constant_evaluated()) {
            // improves codegen (requires no_sanitize("bool") attribute)
            return x.has_value() & (x.get_unchecked() == y);
        }
    }
    if (x.has_value()) {
        return x.get() == y;
    }
    return false;
}

template<typename T1, typename T2> OPTION_NO_SANITIZE("bool")
constexpr bool operator==(const T1& x, const option<T2>& y) requires (!is_option_v<T1>) && requires {
    { x == y.get() } -> impl::core_convertible_to<bool>;
} {
    if constexpr (OPTION_IS_INTEGRAL(T1) && OPTION_IS_INTEGRAL(T2)) {
        if (!std::is_constant_evaluated()) {
            // improves codegen (requires no_sanitize("bool") attribute)
            return y.has_value() & (x == y.get_unchecked());
        }
    }
    if (y.has_value()) {
        return x == y.get();
    }
    return false;
}

template<typename T1, typename T2>
constexpr bool operator!=(const option<T1>& x, const T2& y) requires (!is_option_v<T2>) && requires {
    { x.get() != y } -> impl::core_convertible_to<bool>;
} {
    if (x.has_value()) {
        return x.get() != y;
    }
    return true;
}

template<typename T1, typename T2>
constexpr bool operator!=(const T1& x, const option<T2>& y) requires (!is_option_v<T1>) && requires {
    { x != y.get() } -> impl::core_convertible_to<bool>;
} {
    if (y.has_value()) {
        return x != y.get();
    }
    return true;
}

template<typename T1, typename T2>
constexpr bool operator<(const option<T1>& x, const T2& y) requires (!is_option_v<T2>) && requires {
    { x.get() < y } -> impl::core_convertible_to<bool>;
} {
    if (x.has_value()) {
        return x.get() < y;
    }
    return true;
}

template<typename T1, typename T2>
constexpr bool operator<(const T1& x, const option<T2>& y) requires (!is_option_v<T1>) && requires {
    { x < y.get() } -> impl::core_convertible_to<bool>;
} {
    if (y.has_value()) {
        return x < y.get();
    }
    return false;
}

template<typename T1, typename T2>
constexpr bool operator<=(const option<T1>& x, const T2& y) requires (!is_option_v<T2>) && requires {
    { x.get() <= y } -> impl::core_convertible_to<bool>;
} {
    if (x.has_value()) {
        return x.get() <= y;
    }
    return true;
}

template<typename T1, typename T2>
constexpr bool operator<=(const T1& x, const option<T2>& y) requires (!is_option_v<T1>) && requires {
    { x <= y.get() } -> impl::core_convertible_to<bool>;
} {
    if (y.has_value()) {
        return x <= y.get();
    }
    return false;
}

template<typename T1, typename T2>
constexpr bool operator>(const option<T1>& x, const T2& y) requires (!is_option_v<T2>) && requires {
    { x.get() > y } -> impl::core_convertible_to<bool>;
} {
    if (x.has_value()) {
        return x.get() > y;
    }
    return false;
}

template<typename T1, typename T2>
constexpr bool operator>(const T1& x, const option<T2>& y) requires (!is_option_v<T1>) && requires {
    { x > y.get() } -> impl::core_convertible_to<bool>;
} {
    if (y.has_value()) {
        return x > y.get();
    }
    return true;
}

template<typename T1, typename T2>
constexpr bool operator>=(const option<T1>& x, const T2& y) requires (!is_option_v<T2>) && requires {
    { x.get() >= y } -> impl::core_convertible_to<bool>;
} {
    if (x.has_value()) {
        return x.get() >= y;
    }
    return false;
}

template<typename T1, typename T2>
constexpr bool operator>=(const T1& x, const option<T2>& y) requires (!is_option_v<T1>) && requires {
    { x >= y.get() } -> impl::core_convertible_to<bool>;
} {
    if (y.has_value()) {
        return x >= y.get();
    }
    return true;
}

namespace impl {
    // https://eel.is/c++draft/optional#concept:is-derived-from-optional
#if OPTION_MSVC // workaround for msvc
    // https://github.com/microsoft/STL/blob/da52dc0fcadbd83690cfd1ed32408d2847439493/stl/inc/optional#L1012-L1020
    template<typename U>
    void is_derived_from_option_helper(const option<U>&);

    template<typename T>
    concept is_derived_from_option = requires(const T& x) {
        is_derived_from_option_helper(x);
    };
#else
    template<typename T>
    concept is_derived_from_option = requires(const T& x) {
        []<typename U>(const option<U>&) {}(x);
    };
#endif
}

// https://eel.is/c++draft/optional#lib:operator%3c=%3e,optional__
template<typename T1, typename T2> requires (!impl::is_derived_from_option<T2> && std::three_way_comparable_with<T1, T2>)
constexpr std::compare_three_way_result_t<T1, T2> operator<=>(const option<T1>& x, const T2& y) {
    return x.has_value() ? x.get() <=> y : std::strong_ordering::less;
}

// https://eel.is/c++draft/optional#specalg
template<typename T> requires OPTION_IS_CONSTRUCTIBLE(T, T) && std::is_swappable_v<T>
constexpr void swap(option<T>& x, option<T>& y) noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

}

template<typename T>
constexpr bool std::ranges::enable_view<opt::option<T>> = true;

OPTION_SUPPRESS_COMMON_WARNINGS_POP
