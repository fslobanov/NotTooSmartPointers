add_subdirectory( library )

if( NTSP_BUILD_TESTS )
	message( STATUS "NTSP: Tests will be built .." )
	add_subdirectory( tests )
endif()

if( NTSP_BUILD_EXAMPLES )
	message( STATUS "NTSP: Examples will be built .." )
	add_subdirectory( examples )
endif()