cmake_minimum_required(VERSION 3.0.0)
project(meshprocessor VERSION 0.0.0)

file(GLOB SOURCE_FILES
    "src/*.hpp"
    "src/*.cpp"
)

# faster compilation with multithreading
if(MSVC)
	add_compile_options( /MP )
endif(MSVC)

add_executable(meshprocessor ${SOURCE_FILES})

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("dependencies/assimp")
include_directories("dependencies/assimp/include")
target_link_libraries(meshprocessor assimp)

include_directories("dependencies/quickhull")
add_library(quickhull STATIC dependencies/quickhull/QuickHull.cpp)
target_link_libraries (meshprocessor quickhull)