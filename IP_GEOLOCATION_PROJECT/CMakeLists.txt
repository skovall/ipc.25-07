cmake_minimum_required(VERSION 3.10)
project(ip_geolocation VERSION 1.0.0 LANGUAGES CXX)

# Настройки компиляции
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Опции
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_EXECUTABLE "Build console executable" ON)
option(USE_SYSTEM_CURL "Use system CURL" ON)
option(USE_SYSTEM_JSON "Use system nlohmann_json" ON)

# Определения для экспорта символов
if(BUILD_SHARED_LIBS)
    add_definitions(-DIP_GEOLOCATION_EXPORTS)
endif()

# Поиск зависимостей
find_package(CURL REQUIRED)
if(NOT USE_SYSTEM_JSON)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
else()
    find_package(nlohmann_json REQUIRED)
endif()

# Включаемые директории
include_directories(include)
include_directories(${CURL_INCLUDE_DIRS})

# Исходные файлы библиотеки
set(LIBRARY_SOURCES
    src/geolocation.cpp
    src/platform/platform_utils.cpp
)

set(LIBRARY_HEADERS
    include/ip_geolocation/geolocation.hpp
    include/ip_geolocation/ip_geolocation_export.h
    src/platform/platform_utils.hpp
)

# Создание библиотеки
if(BUILD_SHARED_LIBS)
    add_library(ip_geolocation SHARED ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
else()
    add_library(ip_geolocation STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
endif()

target_link_libraries(ip_geolocation
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Установка свойств для Windows
if(WIN32)
    set_target_properties(ip_geolocation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
endif()

# Установка
install(TARGETS ip_geolocation
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ip_geolocation
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Консольное приложение
if(BUILD_EXECUTABLE)
    add_executable(ip_geo_tool src/main.cpp)
    target_link_libraries(ip_geo_tool ip_geolocation)
    
    install(TARGETS ip_geo_tool RUNTIME DESTINATION bin)
endif()

# Тесты (опционально)
enable_testing()
add_test(NAME ip_geolocation_test
    COMMAND $<TARGET_FILE:ip_geo_tool> 8.8.8.8
)
