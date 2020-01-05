cmake_minimum_required( VERSION 3.15 )
project( NotTooSmartPointers )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
	include( "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
	conan_basic_setup()
else()
	message( WARNING "Run 'conan install' first" )
endif()

add_subdirectory( src )

add_executable( NtspSample main.cpp )
add_dependencies( NtspSample ntsp )
target_link_libraries( NtspSample ntsp )