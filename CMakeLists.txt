cmake_minimum_required (VERSION 3.4)
project (mic_bc)


add_compile_options(-std=c++11 -fopenmp)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
file(GLOB SOURCES *.cpp)


# add the executable
add_executable(mic_bc ${SOURCES})

add_custom_target(clean-all
        rm -rf *
        )
