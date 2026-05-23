# Ищем libcurl
find_package(CURL REQUIRED)

# Создаём библиотеку проекта
add_library(ipgeo_core

    core/GeoLocator.cpp
    core/Location.cpp

    network/HttpRequest.cpp

    storage/ConfigManager.cpp
    storage/History.cpp

    utils/Utils.cpp
)

# Include directories
target_include_directories(ipgeo_core PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}

    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Линкуем curl
target_link_libraries(ipgeo_core
    PUBLIC
    CURL::libcurl
)

# Главный executable
add_executable(ipgeo
    main.cpp
)

# Линкуем core библиотеку
target_link_libraries(ipgeo
    PRIVATE
    ipgeo_core
)
