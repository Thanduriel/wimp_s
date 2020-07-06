cmake_minimum_required (VERSION 3.8)
project (wimp_s)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.hpp" "src/*.cpp")

# organize the files according to the organisation in the file system
# from: http://cmake.3232098.n2.nabble.com/Keep-folder-structure-in-Visual-Studio-Project-generated-with-CMake-td7586044.html
macro(GroupSources curdir)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
                  GroupSources(${curdir}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${curdir})
          # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
                   string(REPLACE "src" "Common" groupname ${groupname})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
          endif()
   endforeach()
endmacro()

# execute tha macro
GroupSources(src)

# needs to be known to link with the libraries
add_executable(wimp_s ${SOURCE_FILES})

# faster compilation with multithreading
if(MSVC)
	add_compile_options( /MP )
	
	set(OPTIMIZATION_OPTIONS 
		$<$<CONFIG:Debug>:>
		$<$<CONFIG:RelWithDebInfo>:>
		$<$<CONFIG:Release>: /O2 /Ob2 /Oi /Ot /GL > 
		$<$<CONFIG:MinSizeRel>:>
    )
	
	add_compile_options( "${OPTIMIZATION_OPTIONS}" )
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT wimp_s)
endif(MSVC)

# navigation should always start from root
include_directories("src")

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory ("dependencies/glfw")
include_directories("dependencies/glfw/include")
target_link_libraries (wimp_s PUBLIC glfw)

# glew
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_MULTI_CONTEXT OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory ("dependencies/glew/build/cmake")
include_directories("dependencies/glew/include")
target_link_libraries (wimp_s PUBLIC glew_s) # use static version

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(wimp_s PUBLIC ${OPENGL_gl_LIBRARY})

# epsilon
include_directories("dependencies/epsilon/include")
file(GLOB SOURCE_FILES_EPSILON "dependencies/epsilon/src/*.cpp")
add_library(epsilon STATIC ${SOURCE_FILES_EPSILON})
target_link_libraries (wimp_s PUBLIC epsilon)

# sound engine
#give hint where to find sdl from inside dependencies/clunk
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
	set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SDL")
	find_package(SDL2 REQUIRED)
endif(NOT SDL2_FOUND)
include_directories("dependencies/sdl/include")
add_subdirectory("dependencies/clunk")
#get_target_property(CLUNK_BIN_DIR clunk-static LOCATION)
#message(${CLUNK_BIN_DIR})
target_include_directories(clunk-static PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/dependencies/clunk")
include_directories("dependencies/clunk")
target_link_libraries (wimp_s PUBLIC clunk-static)
target_link_libraries (wimp_s PUBLIC ${SDL2_LIBRARIES})

# JoFileLib
add_subdirectory("dependencies/JoFileLib")
#target_include_directories(wimp_s PRIVATE "dependencies/JoFileLib/include")
target_link_libraries (wimp_s PRIVATE jofile)