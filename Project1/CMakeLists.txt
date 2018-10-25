cmake_minimum_required(VERSION 2.8)

project( FMAlgorithm )

# Set the active build type
set( CMAKE_BUILD_TYPE Release )
#set( CMAKE_BUILD_TYPE Debug )

# Add compiler flags
set( CMAKE_CXX_FLAGS "-Wall -std=c++11" )

# Bring in the project headers
include_directories( ${CMAKE_SOURCE_DIR}/ )

# Add all of the files to the build
file( GLOB_RECURSE Headers ${CMAKE_SOURCE_DIR}/*.h )
file( GLOB_RECURSE Sources ${CMAKE_SOURCE_DIR}/*.cpp )

# Add the executable output
add_executable( ${PROJECT_NAME} ${Headers} ${Sources} )
