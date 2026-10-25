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

#if OPTION_HAS_EXCEPTIONS
    #define check_no_throw(statement) \
        do { \
            bool _thrown = false; \
            try { statement; } \
            catch (...) { _thrown = true; } \
            if (_thrown) { \
                ::on_check_failed("unexpected exception from: " #statement, __FILE__, __LINE__, CURRENT_FUNCTION); \
                OPTION_DEBUG_BREAK; \
            } \
        } while (0)
    #define check_throw(statement, exception_type) \
        do { \
            int _state = 0; /* 0 = no throw, 1 = expected type, 2 = wrong type */ \
            try { statement; } \
            catch (const exception_type&) { _state = 1; } \
            catch (...) { _state = 2; } \
            if (_state != 1) { \
                ::on_check_failed(_state == 2 ? "expected " #exception_type " but got a different exception from: " #statement \
                                              : "expected " #exception_type " from: " #statement, __FILE__, __LINE__, CURRENT_FUNCTION); \
                OPTION_DEBUG_BREAK; \
            } \
        } while (0)
    #define check_throw_value(statement, exception_type, expected_value) \
        do { \
            int _state = 0; /* 0 = no throw, 1 = match, 2 = wrong type, 3 = wrong value */ \
            try { statement; } \
            catch (const exception_type& _e) { _state = ((_e == (expected_value)) ? 1 : 3); } \
            catch (...) { _state = 2; } \
            if (_state != 1) { \
                ::on_check_failed( \
                    _state == 2 ? "expected " #exception_type " but got a different exception from: " #statement \
                  : _state == 3 ? "expected " #exception_type " == " #expected_value " but got a different value from: " #statement \
                                : "expected " #exception_type " from: " #statement, __FILE__, __LINE__, CURRENT_FUNCTION); \
                OPTION_DEBUG_BREAK; \
            } \
        } while (0)
#else
    #define check_no_throw(statement) do { statement; } while (0)
    #define check_throw(statement, exception_type) do { statement; } while (0)
    #define check_throw_value(statement, exception_type, expected_value) do { statement; } while (0)
#endif
