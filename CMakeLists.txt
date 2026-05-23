cmake_minimum_required(VERSION 3.15)

project(IPGeoLocator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

if (NOT MSVC)
    add_compile_options(
        -fsanitize=address
        -fsanitize=undefined
    )

    add_link_options(
        -fsanitize=address
        -fsanitize=undefined
    )
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
