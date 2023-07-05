cmake_minimum_required(VERSION 3.14)

project(FCGIServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EXECUTE_NAME fcgi_server.fcgi)

add_executable(${EXECUTE_NAME}
    fcgi_server.cpp
    authenticate.cpp)

find_package(jsoncpp REQUIRED)
target_link_libraries(${EXECUTE_NAME} PRIVATE jsoncpp_lib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(fcgi fcgi REQUIRED IMPORTED_TARGET)

target_include_directories(${EXECUTE_NAME} PRIVATE ${fcgi_INCLUDEDIR})

target_link_libraries(${EXECUTE_NAME} PRIVATE PkgConfig::fcgi)

find_package(OpenSSL)
target_include_directories(${EXECUTE_NAME} PRIVATE "third_party/jwt-cpp/include/")
target_link_libraries(${EXECUTE_NAME} PRIVATE OpenSSL::Crypto)

add_executable(a.out
    test.cpp)
target_link_libraries(a.out PRIVATE jsoncpp_lib)
