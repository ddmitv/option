#pragma once

#include <opt/details/preprocessor.hpp>

#if defined(__SANITIZE_ADDRESS__) || OPTION_HAS_FEATURE(address_sanitizer)
    #ifdef _WIN32
        #define OPTION_SANITIZER_CDECL __cdecl
    #else
        #define OPTION_SANITIZER_CDECL
    #endif

OPTION_CLANG_SUPPRESS_WARNING_PUSH
OPTION_CLANG_SUPPRESS_WARNING("-Wreserved-identifier")

extern "C" {
void OPTION_SANITIZER_CDECL __asan_poison_memory_region(const volatile void* addr, size_t size);
void OPTION_SANITIZER_CDECL __asan_unpoison_memory_region(const volatile void* addr, size_t size);
int OPTION_SANITIZER_CDECL __asan_address_is_poisoned(const volatile void* addr);
}

OPTION_CLANG_SUPPRESS_WARNING_POP

    #define OPTION_ADDRESS_SANITIZER 1
#else
    #define OPTION_ADDRESS_SANITIZER 0
#endif

#ifndef OPTION_INSERT_ANNONATION
    #define OPTION_INSERT_ANNONATION OPTION_ADDRESS_SANITIZER
#endif
