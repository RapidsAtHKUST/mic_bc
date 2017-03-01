cmake_minimum_required(VERSION 3.4)

project(mic_bc)

#set(CMAKE_C_COMPILER "icc")
#set(CMAKE_CXX_COMPILER "icpc")

find_package(Boost 1.40 COMPONENTS regex)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 11)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    # using Clang
elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*GNU")
    # using GCC
    add_compile_options(-std=c++11 -fopenmp -O3 -g)
elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Intel")
    # using Intel C++
    add_compile_options(-std=c++11 -fopenmp -qoffload -O3 -g)
elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*MSVC")
    # using Visual Studio C++
endif()

#add_definitions(-DKNL)

include_directories("${PROJECT_BINARY_DIR}")
file(GLOB SOURCES *.cpp)
# add the executable
add_executable(mic_bc ${SOURCES})
target_link_libraries(mic_bc -fopenmp)

set(CMAKE_VERBOSE_MAKEFILE ON)
add_custom_target(clean-all
        rm -rf *
        )
