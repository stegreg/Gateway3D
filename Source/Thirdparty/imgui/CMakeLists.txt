add_library(ImGui STATIC
	imgui.h
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_tables.cpp
	imgui_widgets.cpp
	
	backends/imgui_impl_opengl3.h	backends/imgui_impl_opengl3.cpp
	backends/imgui_impl_sdl2.h		backends/imgui_impl_sdl2.cpp
)

target_include_directories(ImGui
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ImGui
	PRIVATE
		SDL2
)