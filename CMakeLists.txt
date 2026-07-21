cmake_minimum_required(VERSION 3.24)

project(DependencyInjection
    VERSION 1.0.0
    DESCRIPTION "Attribute-style dependency-injection container for C++"
    HOMEPAGE_URL "https://github.com/Ukine9999/DI"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DiModule)

option(DI_BUILD_TESTS "Build DependencyInjection tests" ${PROJECT_IS_TOP_LEVEL})
option(DI_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(di_compile_options INTERFACE)
add_library(Di::CompileOptions ALIAS di_compile_options)
target_compile_features(di_compile_options INTERFACE cxx_std_20)
if(MSVC)
    target_compile_options(di_compile_options INTERFACE /W4 /permissive- /utf-8 /EHsc /Zc:__cplusplus /Zc:preprocessor /Zc:inline)
    if(DI_WARNINGS_AS_ERRORS)
        target_compile_options(di_compile_options INTERFACE /WX)
    endif()
else()
    target_compile_options(di_compile_options INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)
    if(DI_WARNINGS_AS_ERRORS)
        target_compile_options(di_compile_options INTERFACE -Werror)
    endif()
endif()

add_library(DependencyInjection STATIC
    src/Di/Provide/DiServiceProvider.cpp
    src/Di/Provide/DiScope.cpp
    src/Di/Registry/DiRegistry.cpp
    src/Di/Registry/DiRegistryValidator.cpp
    src/Di/Scheme/Factory/DiSchemeFactory.cpp
    src/Di/Scheme/Read/DiSchemeReader.cpp
    src/Di/Container/DiContainer.cpp)
add_library(Di::DependencyInjection ALIAS DependencyInjection)

target_include_directories(DependencyInjection PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_link_libraries(DependencyInjection PRIVATE di_compile_options)

if(DI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
