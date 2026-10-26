// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#ifdef __INTEL_COMPILER
    #define OPTION_CLANG 0
    #define OPTION_GCC   0
    #define OPTION_MSVC  0
    #define OPTION_ICC 1
#elif defined(_MSC_VER)
    #ifdef __clang__
        #define OPTION_CLANG 1
    #else
        #define OPTION_CLANG 0
    #endif
    #define OPTION_GCC   0
    #define OPTION_MSVC  1
    #define OPTION_ICC 0
#elif defined(__clang__)
    #define OPTION_CLANG 1
    #define OPTION_GCC   0
    #define OPTION_MSVC  0
    #define OPTION_ICC 0
#elif defined(__GNUC__) || defined(__GNUG__)
    #define OPTION_CLANG 0
    #define OPTION_GCC   1
    #define OPTION_MSVC  0
    #define OPTION_ICC 0
#else
    #define OPTION_CLANG 0
    #define OPTION_GCC   0
    #define OPTION_MSVC  0
    #define OPTION_ICC 0
#endif

#if OPTION_CLANG && !OPTION_ICC
    #define OPTION_PRAGMA_TO_STR(x) _Pragma(#x)
    #define OPTION_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
    #define OPTION_CLANG_SUPPRESS_WARNING(w) OPTION_PRAGMA_TO_STR(clang diagnostic ignored w)
    #define OPTION_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
#else
    #define OPTION_CLANG_SUPPRESS_WARNING_PUSH
    #define OPTION_CLANG_SUPPRESS_WARNING(w)
    #define OPTION_CLANG_SUPPRESS_WARNING_POP
#endif

#if OPTION_GCC
    #define OPTION_PRAGMA_TO_STR(x) _Pragma(#x)
    #define OPTION_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define OPTION_GCC_SUPPRESS_WARNING(w) OPTION_PRAGMA_TO_STR(GCC diagnostic ignored w)
    #define OPTION_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
#else
    #define OPTION_GCC_SUPPRESS_WARNING_PUSH
    #define OPTION_GCC_SUPPRESS_WARNING(w)
    #define OPTION_GCC_SUPPRESS_WARNING_POP
#endif

#if OPTION_MSVC
    #define OPTION_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
    #define OPTION_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w)) // NOLINT(bugprone-macro-parentheses)
    #define OPTION_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
#else
    #define OPTION_MSVC_SUPPRESS_WARNING_PUSH
    #define OPTION_MSVC_SUPPRESS_WARNING(w)
    #define OPTION_MSVC_SUPPRESS_WARNING_POP
    #define OPTION_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif

#define OPTION_SUPPRESS_COMMON_WARNINGS_PUSH \
    OPTION_MSVC_SUPPRESS_WARNING_PUSH \
    OPTION_MSVC_SUPPRESS_WARNING(4514 /* 'function' : unreferenced inline function has been removed */) \
    OPTION_MSVC_SUPPRESS_WARNING(4582 /* 'type' : constructor is not implicitly called */) \
    OPTION_MSVC_SUPPRESS_WARNING(4583 /* 'type' : destructor is not implicitly called */) \
    OPTION_MSVC_SUPPRESS_WARNING(4625 /* 'derived class' : copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted */) \
    OPTION_MSVC_SUPPRESS_WARNING(4626 /* 'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted */) \
    OPTION_MSVC_SUPPRESS_WARNING(4702 /* unreachable code */) \
    OPTION_MSVC_SUPPRESS_WARNING(4820 /* 'bytes' bytes padding added after construct 'member_name' */) \
    OPTION_MSVC_SUPPRESS_WARNING(5026 /* 'type' : move constructor was implicitly defined as deleted */) \
    OPTION_MSVC_SUPPRESS_WARNING(5027 /* 'type' : move assignment operator was implicitly defined as deleted */) \
    OPTION_CLANG_SUPPRESS_WARNING_PUSH \
    OPTION_CLANG_SUPPRESS_WARNING("-Wunknown-warning-option") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wc++98-compat") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wpre-c++17-compat") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wpre-c++20-compat") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wpadded") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wc++98-compat-bind-to-temporary-copy") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wshadow-field-in-constructor") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wbitwise-instead-of-logical") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wweak-vtables") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wlifetime-safety-suggestions") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wlifetime-safety-lifetimebound-violation") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage-in-libc-call") \
    OPTION_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage") \
    OPTION_GCC_SUPPRESS_WARNING_PUSH \
    OPTION_GCC_SUPPRESS_WARNING("-Wshadow")

#define OPTION_SUPPRESS_COMMON_WARNINGS_POP \
    OPTION_MSVC_SUPPRESS_WARNING_POP \
    OPTION_CLANG_SUPPRESS_WARNING_POP \
    OPTION_GCC_SUPPRESS_WARNING_POP

OPTION_SUPPRESS_COMMON_WARNINGS_PUSH

#ifdef _MSVC_LANG
    #if _MSVC_LANG > __cplusplus
        #define OPTION_CXX_VER _MSVC_LANG
    #else
        #define OPTION_CXX_VER __cplusplus
    #endif
#else
    #define OPTION_CXX_VER __cplusplus
#endif

#if OPTION_CXX_VER >= 202002L
    #define OPTION_IS_CXX20 1
#else
    #define OPTION_IS_CXX20 0
#endif

#ifdef __has_attribute
    #define OPTION_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define OPTION_HAS_ATTRIBUTE(x) (0)
#endif

#ifdef __has_cpp_attribute
    #define OPTION_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define OPTION_HAS_CPP_ATTRIBUTE(x) (0)
#endif

#ifdef __has_builtin
    #ifdef __is_identifier
        // Intel and older Clang require !__is_identifier for some built-ins:
        #define OPTION_HAS_BUILTIN(x) (__has_builtin(x) || !__is_identifier(x))
    #else
        #define OPTION_HAS_BUILTIN(x) __has_builtin(x)
    #endif
#else
    #define OPTION_HAS_BUILTIN(x) (0)
#endif

#ifdef __has_feature
    #define OPTION_HAS_FEATURE(x) __has_feature(x)
#else
    #define OPTION_HAS_FEATURE(x) (0)
#endif

#if OPTION_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
    #define OPTION_LIFETIMEBOUND [[msvc::lifetimebound]]
#elif OPTION_HAS_CPP_ATTRIBUTE(clang::lifetimebound)
    #define OPTION_LIFETIMEBOUND [[clang::lifetimebound]]
#else
    #define OPTION_LIFETIMEBOUND
#endif

#if (OPTION_HAS_BUILTIN(__is_trivially_destructible) && !OPTION_GCC) || OPTION_MSVC
    #define OPTION_IS_TRIVIALLY_DESTRUCTIBLE(x) __is_trivially_destructible(x)
#else
    #define OPTION_IS_TRIVIALLY_DESTRUCTIBLE(x) ::std::is_trivially_destructible_v<x>
#endif

#if (OPTION_HAS_BUILTIN(__is_trivially_constructible) && !OPTION_GCC) || OPTION_MSVC
    #define OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(...) __is_trivially_constructible(__VA_ARGS__)
#else
    #define OPTION_IS_TRIVIALLY_CONSTRUCTIBLE(...) ::std::is_trivially_constructible_v<__VA_ARGS__>
#endif

#if OPTION_HAS_BUILTIN(__remove_cv)
    #define OPTION_REMOVE_CV(x) __remove_cv(x)
#else
    #define OPTION_REMOVE_CV(x) ::std::remove_cv_t<x>
#endif

#if (OPTION_HAS_BUILTIN(__is_constructible) && !OPTION_GCC) || OPTION_MSVC
    #define OPTION_IS_CONSTRUCTIBLE(...) __is_constructible(__VA_ARGS__)
#else
    #define OPTION_IS_CONSTRUCTIBLE(...) ::std::is_constructible_v<__VA_ARGS__>
#endif

#if OPTION_HAS_BUILTIN(__is_nothrow_constructible) || OPTION_MSVC
    #define OPTION_IS_NOTHROW_CONSTRUCTIBLE(...) __is_nothrow_constructible(__VA_ARGS__)
#else
    #define OPTION_IS_NOTHROW_CONSTRUCTIBLE(...) ::std::is_nothrow_constructible_v<__VA_ARGS__>
#endif

#if (OPTION_HAS_BUILTIN(__is_assignable) && !OPTION_GCC) || OPTION_MSVC
    #define OPTION_IS_ASSIGNABLE(...) __is_assignable(__VA_ARGS__)
#else
    #define OPTION_IS_ASSIGNABLE(...) ::std::is_assignable_v<__VA_ARGS__>
#endif

#if (OPTION_HAS_BUILTIN(__is_trivially_assignable) && !OPTION_GCC) || OPTION_MSVC
    #define OPTION_IS_TRIVIALLY_ASSIGNABLE(...) __is_trivially_assignable(__VA_ARGS__)
#else
    #define OPTION_IS_TRIVIALLY_ASSIGNABLE(...) ::std::is_trivially_assignable_v<__VA_ARGS__>
#endif

#if OPTION_HAS_BUILTIN(__is_nothrow_assignable) || OPTION_MSVC
    #define OPTION_IS_NOTHROW_ASSIGNABLE(...) __is_nothrow_assignable(__VA_ARGS__)
#else
    #define OPTION_IS_NOTHROW_ASSIGNABLE(...) ::std::is_nothrow_assignable_v<__VA_ARGS__>
#endif

#if OPTION_HAS_BUILTIN(__is_convertible)
    #define OPTION_IS_CONVERTIBLE(...) __is_convertible(__VA_ARGS__)
#elif OPTION_MSVC
    #define OPTION_IS_CONVERTIBLE(...) __is_convertible_to(__VA_ARGS__)
#else
    #define OPTION_IS_CONVERTIBLE(...) ::std::is_convertible_v<__VA_ARGS__>
#endif

#if OPTION_HAS_BUILTIN(__is_same)
    #define OPTION_IS_SAME(x, y) __is_same(x, y)
#else
    #define OPTION_IS_SAME(x, y) ::std::is_same_v<x, y>
#endif

#if OPTION_HAS_BUILTIN(__remove_cvref)
    #define OPTION_REMOVE_CVREF(x) __remove_cvref(x)
#else
    #define OPTION_REMOVE_CVREF(x) ::std::remove_cvref_t<x>
#endif

#if OPTION_HAS_BUILTIN(__is_scalar) && 0 // OPTION_HAS_BUILTIN(__is_scalar) for some reason returns 1 where __is_scalar is not supported
    #define OPTION_IS_SCALAR(x) __is_scalar(x)
#else
    #define OPTION_IS_SCALAR(x) ::std::is_scalar_v<x>
#endif

#if OPTION_HAS_BUILTIN(__decay)
    #define OPTION_DECAY(x) __decay(x)
#else
    #define OPTION_DECAY(x) ::std::decay_t<x>
#endif

#if OPTION_HAS_BUILTIN(__is_integral)
    #define OPTION_IS_INTEGRAL(x) __is_integral(x)
#else
    #define OPTION_IS_INTEGRAL(x) ::std::is_integral_v<x>
#endif

#if OPTION_HAS_BUILTIN(__is_trivially_copyable)
    #define OPTION_IS_TRIVIALLY_COPYABLE(x) __is_trivially_copyable(x)
#else
    #define OPTION_IS_TRIVIALLY_COPYABLE(x) ::std::is_trivially_copyable_v<x>
#endif

#if OPTION_HAS_BUILTIN(__builtin_addressof) || OPTION_MSVC
    #define OPTION_ADDRESSOF(x) __builtin_addressof(x)
#else
    #define OPTION_ADDRESSOF(x) ::std::addressof(x)
#endif

#ifndef OPTION_CONSUMED_ANNOTATION_CHECKING
    #define OPTION_CONSUMED_ANNOTATION_CHECKING 0
#endif

#if OPTION_CONSUMED_ANNOTATION_CHECKING \
    && OPTION_HAS_CPP_ATTRIBUTE(clang::consumable) && OPTION_HAS_CPP_ATTRIBUTE(clang::callable_when) \
    && OPTION_HAS_CPP_ATTRIBUTE(clang::param_typestate) && OPTION_HAS_CPP_ATTRIBUTE(clang::return_typestate) \
    && OPTION_HAS_CPP_ATTRIBUTE(clang::set_typestate) && OPTION_HAS_CPP_ATTRIBUTE(clang::test_typestate)

    #define OPTION_CONSUMABLE(...) [[clang::consumable(__VA_ARGS__)]]
    #define OPTION_CALLABLE_WHEN(...) [[clang::callable_when(__VA_ARGS__)]]
    #define OPTION_PARAM_TYPESTATE(...) [[clang::param_typestate(__VA_ARGS__)]]
    #define OPTION_RETURN_TYPESTATE(...) [[clang::return_typestate(__VA_ARGS__)]]
    #define OPTION_SET_TYPESTATE(...) [[clang::set_typestate(__VA_ARGS__)]]
    #define OPTION_TEST_TYPESTATE(...) [[clang::test_typestate(__VA_ARGS__)]]
#else
    #define OPTION_CONSUMABLE(...)
    #define OPTION_CALLABLE_WHEN(...)
    #define OPTION_PARAM_TYPESTATE(...)
    #define OPTION_RETURN_TYPESTATE(...)
    #define OPTION_SET_TYPESTATE(...)
    #define OPTION_TEST_TYPESTATE(...)
#endif

#ifndef OPTION_DEBUG_BREAK
    #if OPTION_HAS_BUILTIN(__builtin_debugtrap) || OPTION_CLANG
        #define OPTION_DEBUG_BREAK __builtin_debugtrap()
    #elif OPTION_MSVC
        #define OPTION_DEBUG_BREAK __debugbreak()
    #elif OPTION_HAS_BUILTIN(__builtin_trap) || OPTION_GCC
        #define OPTION_DEBUG_BREAK __builtin_trap()
    #else
        #include <csignal>
        #include <cstdlib>
        #ifdef SIGTRAP
            #define OPTION_DEBUG_BREAK (::std::raise(SIGTRAP), ::std::abort())
        #else
            #define OPTION_DEBUG_BREAK (::std::raise(SIGABRT), ::std::abort())
        #endif
    #endif
#endif

#ifndef OPTION_VERIFY
    #ifndef NDEBUG
        #include <cstdio>
        #define OPTION_VERIFY(expr, msg) \
            ((expr) ? void() : (\
                void(::std::fprintf(stderr, "%s:%d: assertion '%s' failed: %s\n", __FILE__, __LINE__, #expr, msg)), \
                void(::std::fflush(stderr)), \
                void(OPTION_DEBUG_BREAK) \
            ))
    #else
        // clang <=18 reported that [[assume]] attribute existed but not emitted the @llvm.assume IR, so we firstly check for clang and use __builtin_assume
        #if OPTION_CLANG
            #define OPTION_VERIFY(expr, msg) __builtin_assume(expr)
        #elif OPTION_HAS_CPP_ATTRIBUTE(assume)
            #define OPTION_VERIFY(expr, msg) [[assume(expr)]]
        #elif OPTION_MSVC
            #define OPTION_VERIFY(expr, msg) __assume(expr)
        #elif OPTION_GCC
            #define OPTION_VERIFY(expr, msg) (expr ? void() : void(__builtin_unreachable()))
        #else
            #define OPTION_VERIFY(expr, msg) // noop
        #endif
    #endif
#endif

#if OPTION_HAS_CPP_ATTRIBUTE(gnu::pure)
    #define OPTION_PURE [[gnu::pure]]
#else
    #define OPTION_PURE
#endif

#if OPTION_HAS_CPP_ATTRIBUTE(gnu::no_sanitize)
    #define OPTION_NO_SANITIZE(...) [[gnu::no_sanitize(__VA_ARGS__)]]
#elif OPTION_HAS_ATTRIBUTE(no_sanitize)
    #define OPTION_NO_SANITIZE(...) __attribute__((no_sanitize(__VA_ARGS__)))
#else
    #define OPTION_NO_SANITIZE(...)
#endif

// see https://github.com/microsoft/STL/issues/5315, https://github.com/microsoft/STL/pull/5318
#define OPTION_DESTRUCTOR_TOMBSTONES 1

#if OPTION_MSVC
    #ifdef _CPPUNWIND
        #define OPTION_HAS_EXCEPTIONS 1
    #else
        #define OPTION_HAS_EXCEPTIONS 0
    #endif
#else
    #if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || OPTION_HAS_FEATURE(cxx_exceptions)
        #define OPTION_HAS_EXCEPTIONS 1
    #else
        #define OPTION_HAS_EXCEPTIONS 0
    #endif
#endif

#if OPTION_HAS_CPP_ATTRIBUTE(gnu::cold) && OPTION_HAS_CPP_ATTRIBUTE(gnu::noinline)
    #define OPTION_COLD [[gnu::cold, gnu::noinline]]
#elif OPTION_MSVC
    #define OPTION_COLD __declspec(noinline)
#else
    #define OPTION_COLD
#endif

#if OPTION_HAS_CPP_ATTRIBUTE(gsl::Pointer)
    #define OPTION_GSL_POINTER [[gsl::Pointer]]
#else
    #define OPTION_GSL_POINTER
#endif

OPTION_SUPPRESS_COMMON_WARNINGS_POP
