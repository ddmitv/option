#include <version>

#define STR2(x) #x
#define STR(x) STR2(x)

#if defined(_LIBCPP_VERSION)
static_assert(false, "cmake_stdlib_impl=libc++ cmake_stdlib_impl_version=" STR(_LIBCPP_VERSION));
#elif defined(__GLIBCXX__)
static_assert(false, "cmake_stdlib_impl=libstdc++ cmake_stdlib_impl_version=" STR(_GLIBCXX_RELEASE));
#elif defined(_MSVC_STL_VERSION)
static_assert(false, "cmake_stdlib_impl=stl cmake_stdlib_impl_version=" STR(_MSVC_STL_VERSION));
#else
static_assert(false, "cmake_stdlib_impl=unknown cmake_stdlib_impl_version=0");
#endif
