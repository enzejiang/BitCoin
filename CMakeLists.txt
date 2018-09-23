cmake_minimum_required(VERSION 2.8.0)

SET(PROJECT_ROOT "${CMAKE_SOURCE_DIR}/Src")
SET(SRC_ROOT "${CMAKE_SOURCE_DIR}/Src")
SET(LIB_PATH "${CMAKE_SOURCE_DIR}/Lib")
SET(INCLUDE_PATH "${PROJECT_ROOT}/pubinclude")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs  -ftest-coverage -fPIC -O0")

MESSAGE("-------------------------------------------")
MESSAGE("-----project root is ${PROJECT_ROOT}-------")
MESSAGE("-------------------------------------------")
ADD_DEFINITIONS(-D__WX__ -D__WXGTK__)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} wx-config --cxxflags -std=c11")
#add_compile_options()
INCLUDE("${PROJECT_ROOT}/CMakeLists.txt")

ADD_DEFINITIONS(-g)
