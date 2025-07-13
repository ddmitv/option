// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

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

#if OPTION_IS_CXX20
    #define OPTION_CONSTEXPR_CXX20 constexpr
#else
    #define OPTION_CONSTEXPR_CXX20 inline
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
    #define OPTION_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define OPTION_HAS_BUILTIN(x) (0)
#endif

#if OPTION_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
    #define OPTION_LIFETIMEBOUND [[msvc::lifetimebound]]
#elif OPTION_HAS_CPP_ATTRIBUTE(clang::lifetimebound)
    #define OPTION_LIFETIMEBOUND [[clang::lifetimebound]]
#else
    #define OPTION_LIFETIMEBOUND
#endif
