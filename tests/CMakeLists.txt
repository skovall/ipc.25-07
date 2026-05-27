cmake_minimum_required(VERSION 3.15)

if(NOT BUILD_TESTS)
    return()
endif()

# Создаём исполняемый файл для тестов
add_executable(ipgeo_tests
    main_test.cpp
    test_config.cpp
    test_geolocator.cpp
    test_history.cpp
    test_utils.cpp
)

# Добавляем include директории
target_include_directories(ipgeo_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/storage
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/nlohmann
)

# Добавляем curl include для тестов
if(WIN32 AND CURL_INCLUDE_DIR)
    target_include_directories(ipgeo_tests PRIVATE ${CURL_INCLUDE_DIR})
endif()

# Линкуем библиотеки
target_link_libraries(ipgeo_tests
    PRIVATE
    ipgeo_lib
    gtest
    gtest_main
    gmock
)

# Для Windows дополнительные библиотеки
if(WIN32)
    target_link_libraries(ipgeo_tests
        PRIVATE
        winhttp
        ws2_32
        crypt32
    )

    if(CURL_LIBRARY)
        target_link_libraries(ipgeo_tests PRIVATE ${CURL_LIBRARY})
    endif()

    if(CURL_DLL_NAME AND curl_SOURCE_DIR)
        # Копируем DLL на этапе конфигурации cmake — надёжнее чем POST_BUILD с MSBuild
        file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
        file(COPY "${curl_SOURCE_DIR}/bin/${CURL_DLL_NAME}"
            DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
        file(COPY "${curl_SOURCE_DIR}/bin/${CURL_DLL_NAME}"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
endif()

# Включаем тесты — WORKING_DIRECTORY указывает папку где лежит exe и DLL
# PRE_TEST откладывает обнаружение тестов до запуска, а не сборки
include(GoogleTest)
gtest_discover_tests(ipgeo_tests
    WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
    DISCOVERY_MODE PRE_TEST
)

# Добавляем флаг для ожидания ввода на Windows
if(WIN32)
    target_compile_definitions(ipgeo_tests PRIVATE WAIT_ON_EXIT)
endif()
