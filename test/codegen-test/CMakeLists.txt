
add_library(option-codegen-test
    "sample.cpp"
    "operators.cpp"
)
target_link_libraries(option-codegen-test PRIVATE option)

try_compile(detect_toolchain_info_result # ignored
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/detect_toolchain_info.cpp"
    OUTPUT_VARIABLE detect_toolchain_info_output
    CMAKE_FLAGS "-DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY"
)

if(detect_toolchain_info_output MATCHES "cmake_stdlib_impl=([a-zA-Z0-9+]+) cmake_stdlib_impl_version=([a-zA-Z0-9_]+) cmake_arch=([a-zA-Z0-9_]+)")
    set(cxx_stdlib_impl "${CMAKE_MATCH_1}")
    set(cxx_stdlib_impl_version "${CMAKE_MATCH_2}")
    set(cxx_target_arch "${CMAKE_MATCH_3}")
else()
    message(WARNING "Could not detect stdlib/arch from try_compile output")
endif()

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(option-codegen-test PRIVATE /Fa)
else()
    target_compile_options(option-codegen-test PRIVATE -save-temps=obj)
    if (cxx_target_arch STREQUAL "x64" OR cxx_target_arch STREQUAL "x86")
        target_compile_options(option-codegen-test PRIVATE -masm=intel)
    endif()
endif()

find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_custom_target(run-option-codegen-test VERBATIM
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/asm-test.py"
        "${CMAKE_CXX_COMPILER_ID};${CMAKE_CXX_COMPILER_FRONTEND_VARIANT};${CMAKE_CXX_COMPILER_VERSION};${cxx_stdlib_impl};${cxx_stdlib_impl_version};${cxx_target_arch};$<CONFIG>;${CMAKE_SYSTEM_NAME}"
        "$<TARGET_OBJECTS:option-codegen-test>"
        "$<TARGET_PROPERTY:option-codegen-test,SOURCES>"
        "${CMAKE_BINARY_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Running option codegen test"
)
add_dependencies(run-option-codegen-test option-codegen-test)
