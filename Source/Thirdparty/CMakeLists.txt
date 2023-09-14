add_subdirectory(glad)
add_subdirectory(json)
add_subdirectory(SDL)
add_subdirectory(toml11)
add_subdirectory(stbi)


set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)

execute_process(
	COMMAND ${CMAKE_COMMAND} -B ${CMAKE_BINARY_DIR}/Source/Thirdparty/freetype -S ${GATEWAY_ROOT}/Source/Thirdparty/freetype
)

execute_process(
	COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/Source/Thirdparty/freetype --config Release
)
execute_process(
	COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}/Source/Thirdparty/freetype --prefix ${GATEWAY_ROOT}/Source/Thirdparty/RmlUi/Dependencies --config Release
)

set(BUILD_TESTING OFF)
add_subdirectory(RmlUi)

add_subdirectory(imgui)
add_subdirectory(tracy)
