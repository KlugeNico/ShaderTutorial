cmake_minimum_required(VERSION 3.6)
cmake_policy(SET CMP0072 NEW)

set( CMAKE_CXX_STANDARD 14 )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ )
list( APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ )

project( ShaderTests )

file( GLOB_RECURSE codeSource "code/*.cpp" )

find_package( OpenGL )
find_package( SFML 2 COMPONENTS graphics window system )
find_package( GLEW REQUIRED )

IF( SFML_FOUND )
    include_directories( ${SFML_INCLUDE_DIR} )
    add_executable( ShaderTests main.cpp ${codeSource} )
    target_link_libraries( ShaderTests ${SFML_LIBRARIES} GLEW ${OPENGL_LIBRARY} ${GLEW_LIBRARIES} )
ELSE()
    message( "You have to install SFML!!!" )
ENDIF()