cmake_minimum_required(VERSION 3.10.0)
project(assn1q3 VERSION 0.1.0)

include(CTest)
enable_testing()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.65.1 COMPONENTS regex) 

file(GLOB SOURCES
    *.h
    *.cpp
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(assn1q3 ${SOURCES}) 
    target_link_libraries(assn1q3 ${Boost_LIBRARIES})
endif()


set_target_properties(assn1q3 PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
