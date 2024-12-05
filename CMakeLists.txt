cmake_minimum_required(VERSION 3.10)

project(KNN_SIMD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -mfma -march=native -O3")

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/Plane.cpp
    ${SRC_DIR}/Gui.cpp
)

include_directories(${SRC_DIR})

add_executable(knn ${SOURCES})

find_package(X11 REQUIRED)
target_link_libraries(knn X11)
