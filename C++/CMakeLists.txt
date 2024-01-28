cmake_minimum_required(VERSION 3.10)
set(PYBIND11_PYTHON_VERSION "3.10")
set(CMAKE_CXX_STANDARD 20)

project(URLP)
add_library(URLProxyC SHARED URLProxyC.cpp) # replace SHARED with STATIC if you want a STATIC library

target_include_directories(URLProxyC PRIVATE /usr/include/python3.10)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

find_package(jsoncpp REQUIRED)
target_link_libraries(URLProxyC PRIVATE jsoncpp)

find_package(CURL REQUIRED)
target_link_libraries(URLProxyC PRIVATE CURL::libcurl)