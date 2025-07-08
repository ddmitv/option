# Copyright 2025 Dima Dmitriev.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(option-test
    "main.cpp"
    "option_test.cpp"
)
target_link_libraries(option-test PRIVATE option)

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
