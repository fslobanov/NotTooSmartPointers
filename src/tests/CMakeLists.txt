set( TARGET_NAME ntsp_tests )

enable_testing()
find_package( GTest REQUIRED )

include( GoogleTest )
include_directories( ${GTEST_INCLUDE_DIR} )

add_executable(
	${TARGET_NAME} main.cpp

	shared_ptr.cpp
	weak_ptr_test.cpp
	enable_shared_from_this.cpp
)

add_dependencies( ${TARGET_NAME} ntsp )
target_link_libraries( ${TARGET_NAME} ntsp ${GTEST_LIBRARIES} Threads::Threads )

add_test( NAME ${TARGET_NAME} COMMAND ${TARGET_NAME} )