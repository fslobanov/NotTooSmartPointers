cmake_minimum_required( VERSION 3.15 )
project( NotTooSmartPointers )

option( NTSP_BUILD_SAMPLES "Build NTSP tests" ON )
option( NTSP_BUILD_SAMPLES "Build NTSP examples" ON )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

include( GNUInstallDirs )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}" )

if( CMAKE_CXX_COMPILER_ID MATCHES GNU )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fconcepts" )
endif()

if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
	include( "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
	conan_basic_setup()
else()
	message( WARNING "Run 'conan install' first" )
endif()

add_subdirectory( src )