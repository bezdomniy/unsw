cmake_minimum_required(VERSION 3.0.2)
project(assn1q3 VERSION 0.1.0)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")

file(GLOB SOURCES
    *.h
    *.cpp
)

add_executable(assn1q3 ${SOURCES}) 
target_link_libraries(assn1q3)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
