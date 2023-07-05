cmake_minimum_required(VERSION 3.14)

project(FCGIServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EXECUTE_NAME fcgi_server.fcgi)

add_executable(${EXECUTE_NAME}
    fcgi_server.cpp)

find_package(jsoncpp REQUIRED)
target_link_libraries(${EXECUTE_NAME} PRIVATE jsoncpp_lib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(fcgi fcgi REQUIRED IMPORTED_TARGET)

target_include_directories(${EXECUTE_NAME} PRIVATE ${fcgi_INCLUDEDIR})

target_link_libraries(${EXECUTE_NAME} PRIVATE PkgConfig::fcgi)
