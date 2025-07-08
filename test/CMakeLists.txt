# Copyright 2025 Dima Dmitriev.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(option-test
    "main.cpp"
    "option_test.cpp"
)
target_link_libraries(option-test PRIVATE option)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(option-test PRIVATE
        /Wall /WX /permissive-
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
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
    )
endif()

include(FetchContent)
FetchContent_Declare(
    doctest
    URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.12.zip
    # URL_HASH SHA512=0
    FIND_PACKAGE_ARGS 2.4.12
)
FetchContent_MakeAvailable(doctest)

target_link_libraries(option-test PRIVATE doctest::doctest)
target_compile_definitions(option-test PRIVATE
    DOCTEST_CONFIG_NO_MULTITHREADING # https://github.com/doctest/doctest/blob/master/doc/markdown/configuration.md#doctest_config_no_multithreading
    DOCTEST_CONFIG_SUPER_FAST_ASSERTS # https://github.com/doctest/doctest/blob/master/doc/markdown/configuration.md#doctest_config_super_fast_asserts
)

add_custom_target(exec-option-test VERBATIM
    COMMAND "$<TARGET_FILE:option-test>"
)
