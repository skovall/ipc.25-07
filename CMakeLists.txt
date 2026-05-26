cmake_minimum_required(VERSION 3.15)

project(IPGeoLocator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" OFF)

# Автоматическая загрузка curl
include(FetchContent)

# Загрузка curl для Windows
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CURL_ARCH "win64")
        set(CURL_DLL_NAME "libcurl-x64.dll")
    endif()
    
    set(CURL_VERSION "8.20.0")
    set(CURL_URL "https://curl.se/windows/dl-${CURL_VERSION}_3/curl-${CURL_VERSION}_3-${CURL_ARCH}-mingw.zip")
    
    FetchContent_Declare(
        curl
        URL ${CURL_URL}
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    
    FetchContent_MakeAvailable(curl)
    
    # Указываем пути к curl
    set(CURL_INCLUDE_DIR "${curl_SOURCE_DIR}/include")
    set(CURL_LIBRARY "${curl_SOURCE_DIR}/lib/libcurl.dll.a")
    
    message(STATUS "CURL include: ${CURL_INCLUDE_DIR}")
    message(STATUS "CURL library: ${CURL_LIBRARY}")
endif()

# Скачиваем Google Test
if(BUILD_TESTS)
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
endif()

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
