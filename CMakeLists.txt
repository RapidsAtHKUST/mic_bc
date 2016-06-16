cmake_minimum_required (VERSION 3.4)

set(CMAKE_C_COMPILER "icc")
set(CMAKE_CXX_COMPILER "icpc")

project (mic_bc)

find_package(Boost 1.40 COMPONENTS regex)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

    add_compile_options(-std=c++11 -fopenmp -O3)

    include_directories("${PROJECT_BINARY_DIR}")
    file(GLOB SOURCES *.cpp)
    # add the executable
    add_executable(mic_bc ${SOURCES})
endif()
add_custom_target(clean-all
        rm -rf *
        )