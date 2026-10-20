#pragma once

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define CURRENT_FUNCTION __FUNCSIG__
#else
    #define CURRENT_FUNCTION __func__
#endif

void on_check_failed(const char* expr, const char* filename, unsigned int line, const char* function);

#if defined(__clang__) || defined(__INTEL_COMPILER) || defined(__GNUC__) || defined(__GNUG__)
    #define DEBUG_BREAK __builtin_trap()
#elif defined(_MSC_VER)
    #define DEBUG_BREAK __debugbreak()
#else
    #include <csignal>
    #if defined(SIGTRAP)
        #define DEBUG_BREAK ::std::raise(SIGTRAP)
    #else
        #define DEBUG_BREAK ::std::raise(SIGABRT)
    #endif
#endif

#define check(expr) \
    do { \
        if (expr) {} else { \
            ::on_check_failed(#expr, __FILE__, __LINE__, CURRENT_FUNCTION); \
            DEBUG_BREAK; \
        } \
    } while (0)

#define check_meta(expr, expected_type, expected_value, noexcept_op) \
    do { \
        static_assert(::std::is_same_v<decltype(expr), expected_type>, "unexpected type for: " #expr); \
        static_assert(noexcept_op(expr), "noexcept(" #expr ") expected " #noexcept_op); \
        if ((expr) == (expected_value)) {} else { \
            ::on_check_failed(#expr, __FILE__, __LINE__, CURRENT_FUNCTION); \
            DEBUG_BREAK; \
        } \
    } while (0)

#define check_meta_auto(expr, expected_value, noexcept_op) \
    do { \
        static_assert(::std::is_same_v<decltype(expr), decltype(expected_value)>, "unexpected type for: " #expr); \
        static_assert(noexcept_op(expr), "noexcept(" #expr ") expected " #noexcept_op); \
        if ((expr) == (expected_value)) {} else { \
            ::on_check_failed(#expr, __FILE__, __LINE__, CURRENT_FUNCTION); \
            DEBUG_BREAK; \
        } \
    } while (0)

#define static_check_meta(expr, expected_type, expected_value, noexcept_op) \
    static_assert(::std::is_same_v<decltype(expr), expected_type>, "unexpected type for: " #expr); \
    static_assert(noexcept_op(expr), "noexcept(" #expr ") expected " #noexcept_op); \
    static_assert((expr) == (expected_value), "unexpected value for: " #expr)

void main_test_suite();
void lifetime_test_suite();
void none_test_suite();
void comp_with_none_test_suite();
void comp_with_t_test_suite();
void relops_test_suite();
