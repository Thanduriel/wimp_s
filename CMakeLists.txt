cmake_minimum_required (VERSION 3.1)
project (wimp_s)

# require c++14; in future maybe update to 17
set (CMAKE_CXX_STANDARD 14)

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
# navigation should always start from root
include_directories("src")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("dependencies/assimp")
include_directories("dependencies/assimp/include")
target_link_libraries(wimp_s assimp)

add_subdirectory ("dependencies/glfw")
include_directories("dependencies/glfw/include")
target_link_libraries (wimp_s glfw)

add_subdirectory ("dependencies/glew/build/cmake")
include_directories("dependencies/glew/include")
target_link_libraries (wimp_s glew_s) # use static version

find_package(OpenGL REQUIRED)
target_link_libraries(wimp_s ${OPENGL_gl_LIBRARY})

include_directories("dependencies/epsilon/include")
file(GLOB SOURCE_FILES_EPSILON "dependencies/epsilon/src/*.cpp")
add_library(epsilon ${SOURCE_FILES_EPSILON})
target_link_libraries (wimp_s epsilon)

# You can define two import-locations: one for debug and one for release.
#set_target_properties( jofilelib PROPERTIES IMPORTED_LOCATION dependencies/jofilelib/debug )
include_directories("dependencies/jofilelib/include")
target_link_libraries (wimp_s 
	debug dependencies/JoFileLib/lib/Debug/jofile
	optimized dependencies/JoFileLib/lib/Release/jofile)