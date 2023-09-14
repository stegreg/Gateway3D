add_library(STBI INTERFACE)

target_include_directories(STBI
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)