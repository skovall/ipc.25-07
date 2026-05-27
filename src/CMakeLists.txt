cmake_minimum_required(VERSION 3.15)

# Добавляем библиотеку ipgeo_lib
add_library(ipgeo_lib STATIC
    core/GeoLocator.cpp
    core/Location.cpp
    network/HttpRequest.cpp
    storage/ConfigManager.cpp
    storage/History.cpp
    utils/Utils.cpp
)

# Указываем пути к заголовкам
target_include_directories(ipgeo_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann
)

# Добавляем curl include если на Windows
if(WIN32 AND CURL_INCLUDE_DIR)
    target_include_directories(ipgeo_lib PUBLIC ${CURL_INCLUDE_DIR})
endif()

# Для Windows используем WinHTTP + curl
if(WIN32)
    target_link_libraries(ipgeo_lib
        PUBLIC
        winhttp
        ws2_32
        crypt32
    )
    if(CURL_LIBRARY)
        target_link_libraries(ipgeo_lib PUBLIC ${CURL_LIBRARY})
    endif()
endif()

# Исполняемый файл
add_executable(ipgeo main.cpp)
target_link_libraries(ipgeo PRIVATE ipgeo_lib)

# Для Windows — DLL копирование (winhttp/curl уже транзитивны через ipgeo_lib)
if(WIN32)
    if(CURL_DLL_NAME AND curl_SOURCE_DIR)
        add_custom_command(TARGET ipgeo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${curl_SOURCE_DIR}/bin/${CURL_DLL_NAME}"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CURL_DLL_NAME}"
            COMMENT "Copying ${CURL_DLL_NAME} to executable directory"
        )
    endif()
endif()
