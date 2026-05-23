cmake_minimum_required(VERSION 3.15)

project(IPGeoLocator VERSION 1.0 LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Компиляторные предупреждения
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Санитайзеры (Linux/macOS)
if(NOT MSVC)
    add_compile_options(
        -fsanitize=address
        -fsanitize=undefined
    )

    add_link_options(
        -fsanitize=address
        -fsanitize=undefined
    )
endif()

# Включаем подпроекты
add_subdirectory(src)
add_subdirectory(tests)
