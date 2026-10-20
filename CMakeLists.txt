# Copyright 2025 Dima Dmitriev.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.25)

project(option VERSION 2.0.0 LANGUAGES CXX)

option(option_BUILD_TEST "Adds 'option-test' target" ${PROJECT_IS_TOP_LEVEL})

add_library(option INTERFACE
    "include/opt/option.hpp"
    "include/opt/option_fwd.hpp"

    "include/opt/details/preprocessor.hpp"
)
target_include_directories(option INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
target_compile_features(option INTERFACE cxx_std_17)

if (option_BUILD_TEST)
    add_subdirectory(test)
endif()
