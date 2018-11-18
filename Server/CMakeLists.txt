cmake_minimum_required(VERSION 3.5)

find_package(Protobuf REQUIRED)

if(PROTOBUF_FOUND)
    message(STATUS "protobuf library found")
else()
    message(FATAL_ERROR "protobuf library is needed but cant be found")
endif()

SET(PROJECT_ROOT "${CMAKE_SOURCE_DIR}/Src")
SET(SRC_ROOT "${CMAKE_SOURCE_DIR}/Src")
SET(PROTO_SRC_PATH "${CMAKE_SOURCE_DIR}/Proto")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs  -ftest-coverage -fPIC -O0")
SET(PROTO_GEN_PATH "${SRC_ROOT}")
SET(INSTALL_PATH "${CMAKE_SOURCE_DIR}/AppOut")

MESSAGE("-------------------------------------------")
MESSAGE("-----project root is ${PROJECT_ROOT}-------")
MESSAGE("-------------------------------------------")
MESSAGE("-------------${Protobuf_LIBRARIES}---------------")
ADD_DEFINITIONS(-D__WX__ -D__WXGTK__)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} wx-config --cxxflags -std=c11")
#add_compile_options()
INCLUDE("${SRC_ROOT}/CMakeLists.txt")
INCLUDE("${PROTO_SRC_PATH}/CMakeLists.txt")
# cmake --help-module FindProtobuf
ADD_DEFINITIONS(-g)
ADD_DEFINITIONS(-std=c++11 )
