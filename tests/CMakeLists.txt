find_package(Catch2 3 REQUIRED)

add_executable(unit_tests

    test_utils.cpp
    test_history.cpp
    test_config.cpp
    test_geolocator.cpp
)

target_link_libraries(unit_tests

    PRIVATE
    ipgeo_core
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)

catch_discover_tests(unit_tests)
