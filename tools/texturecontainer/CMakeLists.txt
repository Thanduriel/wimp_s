cmake_minimum_required(VERSION 3.0.0)
project(textureContainer VERSION 1.0.0)

file(GLOB project_SRC
    "*.hpp"
	"*.h"
    "*.cpp"
)

add_executable(texContainer ${project_SRC})

include_directories("${CMAKE_SOURCE_DIR}/../../dependencies/jofilelib/include")
target_link_libraries (texContainer 
	debug "${CMAKE_SOURCE_DIR}/../../dependencies/JoFileLib/lib/Debug/jofile"
	optimized "${CMAKE_SOURCE_DIR}/../../dependencies/JoFileLib/lib/Release/jofile")