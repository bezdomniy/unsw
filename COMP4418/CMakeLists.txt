cmake_minimum_required(VERSION 3.0.0)
project(ass1 VERSION 0.1.0)

include(CTest)
enable_testing()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.65.0 COMPONENTS regex) 



file(GLOB SOURCES
    *.h
    *.cpp
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(ass1 ${SOURCES}) 
    target_link_libraries(ass1 ${Boost_LIBRARIES})
endif()

# add_executable(ass1 ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
