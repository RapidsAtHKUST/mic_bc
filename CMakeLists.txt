cmake_minimum_required (VERSION 3.4)

project (mic_bc)

#set(CMAKE_C_COMPILER "icc")
#set(CMAKE_CXX_COMPILER "icpc")

find_package(Boost 1.40 COMPONENTS regex)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_compile_options(-std=c++11 -fopenmp -O3 -g)
#add_definitions(-DSTAGET)

include_directories("${PROJECT_BINARY_DIR}")
file(GLOB SOURCES *.cpp)
# add the executable
add_executable(mic_bc ${SOURCES})

set(CMAKE_VERBOSE_MAKEFILE ON)
add_custom_target(clean-all
        rm -rf *
        )
