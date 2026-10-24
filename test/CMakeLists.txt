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
    ctors/default_ctor.cpp
    ctors/test_types.hpp
    ctors/deduction_guides.cpp
    option_iterator.cpp
)
target_link_libraries(option-test PRIVATE option)
set_target_properties(option-test PROPERTIES CXX_EXTENSIONS FALSE)
target_compile_definitions(option-test PRIVATE OPTION_CONSUMED_ANNOTATION_CHECKING=1)
target_include_directories(option-test PRIVATE "${PROJECT_SOURCE_DIR}/test")

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
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(option-test PRIVATE
        -Wconsumed

        -Weverything -Werror
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-unsafe-buffer-usage-in-libc-call
        -Wno-float-equal
        -Wno-unsafe-buffer-usage
        -Wno-self-assign-overloaded
    )
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
endif()

add_custom_target(run-option-test VERBATIM
    COMMAND "$<TARGET_FILE:option-test>"
)
