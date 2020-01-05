set( TARGET_NAME "ntsp" )
add_library( ${TARGET_NAME} STATIC )

set( INCLUDE_DIR "${NotTooSmartPointers_SOURCE_DIR}/include" )
set( HEADERS_DIR "${INCLUDE_DIR}/ntsp" )
target_include_directories( ${TARGET_NAME} PUBLIC ${INCLUDE_DIR} )

target_sources( ${TARGET_NAME}
		PUBLIC

		"${HEADERS_DIR}/traits.h"
		"${HEADERS_DIR}/reference_counter.h"
		"${HEADERS_DIR}/shared_pointer.h"
		"${HEADERS_DIR}/weak_pointer.h"
		"${HEADERS_DIR}/enable_shared_from_this.h"

		PRIVATE

		"${CMAKE_CURRENT_SOURCE_DIR}/ntsp/reference_counter.cpp"
)