#include <version>

#define STR2(x) #x
#define STR(x) STR2(x)

#if defined(_M_ARM64EC)
    #define ARCH "arm64ec"
#elif defined(__x86_64__) || defined(_M_X64)
    #define ARCH "x64"
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH "arm64"
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH "arm"
#elif defined(__riscv)
    #define ARCH "riscv"
#elif defined(__powerpc64__)
    #define ARCH "ppc64"
#else
    #define ARCH "unknown"
#endif

#if defined(_LIBCPP_VERSION)
static_assert(false, "cmake_stdlib_impl=libc++ cmake_stdlib_impl_version=" STR(_LIBCPP_VERSION) " cmake_arch=" ARCH);
#elif defined(__GLIBCXX__)
static_assert(false, "cmake_stdlib_impl=libstdc++ cmake_stdlib_impl_version=" STR(_GLIBCXX_RELEASE) " cmake_arch=" ARCH);
#elif defined(_MSVC_STL_VERSION)
static_assert(false, "cmake_stdlib_impl=stl cmake_stdlib_impl_version=" STR(_MSVC_STL_VERSION) " cmake_arch=" ARCH);
#else
static_assert(false, "cmake_stdlib_impl=unknown cmake_stdlib_impl_version=0 cmake_arch=" ARCH);
#endif
