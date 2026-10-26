# Copyright 2025 Dima Dmitriev.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(option-test
    "main.cpp"
    "option_test.cpp"
    "option_type_traits_test.cpp"
    "option_lifetime_test.cpp"
    "operators_test.cpp"

    "comp_ops/none.cpp"
    "comp_ops/comp_with_none.cpp"

    "comp_ops/comp_with_t.cpp"

    "tests.hpp"
    comp_ops/comparison_helpers.hpp
    comp_ops/relops.cpp
    ctors/default_none_ctor.cpp
    ctors/test_types.hpp
    ctors/from_u_ctor.cpp
    ctors/deduction_guides.cpp
    option_iterator.cpp
    option_observe.cpp
)
target_link_libraries(option-test PRIVATE option)
set_target_properties(option-test PROPERTIES CXX_EXTENSIONS FALSE)
target_compile_definitions(option-test PRIVATE OPTION_CONSUMED_ANNOTATION_CHECKING=1)
target_include_directories(option-test PRIVATE "${PROJECT_SOURCE_DIR}/test")

add_library(option-header-test OBJECT
    header_self_contained.cpp
)
target_link_libraries(option-header-test PRIVATE option)
set_target_properties(option-header-test PROPERTIES CXX_EXTENSIONS FALSE)
target_compile_definitions(option-header-test PRIVATE OPTION_CONSUMED_ANNOTATION_CHECKING=1)

add_dependencies(option-test option-header-test)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(option-test PRIVATE
        /Wall /WX /permissive-
        /wd4711
        /wd4710 # function not inlined
        /wd4626 # 'type': assignment operator was implicitly defined as deleted
        /wd5027 # 'type': move assignment operator was implicitly defined as deleted
        /wd4625 # 'type': copy constructor was implicitly defined as deleted
        /wd5026 # 'type': move constructor was implicitly defined as deleted
        /wd4514 # 'function': unreferenced inline function has been removed
        /wd4583 # 'type': destructor is not implicitly called
        /wd4702 # unreachable code
    )
    target_compile_options(option-header-test PRIVATE /Wall /WX /permissive-)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(option-test PRIVATE
        -Wconsumed

        -Weverything -Werror
        -Wno-unknown-warning-option
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-float-equal
        -Wno-self-assign-overloaded
        -Wno-double-promotion
        -Wno-missing-prototypes
        -Wno-zero-as-null-pointer-constant # false positives on <=>
        -Wno-lifetime-safety-invalidation
        -Wno-unsafe-buffer-usage
        -Wno-unsafe-buffer-usage-in-libc-call
        -Wno-unknown-sanitizers
    )

    target_compile_options(option-header-test PRIVATE -Weverything -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(option-test PRIVATE
        -Wall # Enables most warnings
        -Wextra # Enables some extra warning flags that are not enabled by -Wall
        -Wpedantic # Issue all the warnings demanded by strict ISO C and ISO C++
        -Wconversion # Warn for implicit conversions that may alter a value
        -Werror # Make all warnings into errors.
        -Wcast-align # Warn whenever a pointer is cast such that the required alignment of the target is increased
        -Wunused # -Wunused options combined
        -Wsign-conversion # Warn for implicit conversions that may change the sign of an integer value
        -Wdouble-promotion # Give a warning when a value of type float is implicitly promoted to double
        -Wold-style-cast # Warn for C style casting
        -Wshadow # Warn whenever a local variable or type declaration shadows another variable
        -Wno-self-move
    )
    target_compile_options(option-header-test PRIVATE
        -Wall -Wextra -Wpedantic -Werror -Wattributes -Waddress-of-packed-member
        -Wcast-align=strict -Wcast-qual -Wstrict-aliasing=3 -Wclass-memaccess
        -Wconversion -Warith-conversion -Wfloat-conversion -Wfloat-equal -Wenum-conversion -Wsign-promo
        -Wconditionally-supported -Wdeprecated-copy-dtor -Wdeprecated-enum-enum-conversion -Wdeprecated-enum-float-conversion
        -Winit-list-lifetime -Wnoexcept -Wnoexcept-type -Wtemplate-body -Wmissing-template-keyword -Wmissing-requires
        -Wchanges-meaning -Wstrict-overflow=5 -Wstringop-overflow=4 -Wstringop-overread -Wundef -Wpragmas -Wunknown-pragmas
        -Wint-in-bool-context -Wlogical-op -Wtautological-compare -Wtype-limits -Wduplicated-branches -Wduplicated-cond
        -Wnull-dereference -Wdangling-pointer=2 -Wuse-after-free=3 -Warray-bounds=2 -Wmaybe-uninitialized -Wuninitialized
        -Wmismatched-tags -Wredundant-tags -Wignored-attributes -Wignored-qualifiers
        -Wodr -Wlto-type-mismatch -Winvalid-constexpr -Wmultistatement-macros -Wunused -Wunused-const-variable=2
        -fanalyzer
    )
endif()

add_custom_target(run-option-test VERBATIM
    COMMAND "$<TARGET_FILE:option-test>"
)
