cmake_minimum_required(VERSION 3.16)
project(MITL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)
find_package(Python COMPONENTS Interpreter Development REQUIRED)

add_subdirectory(core)
add_subdirectory(bindings/python)

