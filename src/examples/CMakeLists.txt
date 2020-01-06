set( TARGET_NAME "ntsp_example" )

add_executable( ${TARGET_NAME} main.cpp )

add_dependencies( ${TARGET_NAME} ntsp )
target_link_libraries( ${TARGET_NAME} ntsp )