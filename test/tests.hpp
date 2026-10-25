#pragma once

#include <opt/details/preprocessor.hpp>
#include <type_traits>

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define CURRENT_FUNCTION __FUNCSIG__
#else
    #define CURRENT_FUNCTION __func__
#endif

void on_check_failed(const char* message, const char* filename, unsigned int line, const char* function);

#define check(expr) \
    do { /* since type of expr can be anything, we cannot use operator! to get the inverted value */ \
        if (expr) {} else { \
            ::on_check_failed(#expr, __FILE__, __LINE__, CURRENT_FUNCTION); \
            OPTION_DEBUG_BREAK; \
        } \
    } while (0)

#define check_meta(expr, expected_type, expected_value, noexcept_op) \
    do { \
        static_assert(::std::is_same_v<decltype(expr), expected_type>, "unexpected type for: " #expr); \
        static_assert(noexcept_op(expr), "noexcept(" #expr ") expected " #noexcept_op); \
        if ((expr) == (expected_value)) {} else { \
            ::on_check_failed(#expr, __FILE__, __LINE__, CURRENT_FUNCTION); \
            OPTION_DEBUG_BREAK; \
        } \
    } while (0)

// noexcept_op must be either `noexcept` or `!noexcept`
#define check_meta_auto(expr, expected_value, noexcept_op) \
    do { \
        static_assert(::std::is_same_v<decltype(expr), decltype(expected_value)>, "unexpected type for: " #expr); \
        static_assert(noexcept_op(expr), "noexcept(" #expr ") expected " #noexcept_op); \
        if ((expr) == (expected_value)) {} else { \
            ::on_check_failed(#expr, __FILE__, __LINE__, CURRENT_FUNCTION); \
            OPTION_DEBUG_BREAK; \
        } \
    } while (0)

// noexcept_op must be either `noexcept` or `!noexcept`
#define static_check_meta(expr, expected_type, expected_value, noexcept_op) \
    static_assert(::std::is_same_v<decltype(expr), expected_type>, "unexpected type for: " #expr); \
    static_assert(noexcept_op(expr), "noexcept(" #expr ") expected " #noexcept_op); \
    static_assert((expr) == (expected_value), "unexpected value for: " #expr)
