find_package(CURL REQUIRED)

add_library(ipgeo_core

    core/GeoLocator.cpp
    core/Location.cpp

    network/HttpRequest.cpp

    storage/ConfigManager.cpp
    storage/History.cpp

    utils/Utils.cpp
)

target_include_directories(ipgeo_core PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}

    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_link_libraries(ipgeo_core
    PUBLIC
    CURL::libcurl
)

add_executable(ipgeo

    main.cpp
)

target_link_libraries(ipgeo
    PRIVATE
    ipgeo_core
)
