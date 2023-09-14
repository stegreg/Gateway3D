set(CLIENT_INTERFACE
	Gateway3D.h
)

set(GRAPHICS
	Model.h					Model.cpp
	Texture.h				Texture.cpp
	RenderTarget.h			RenderTarget.cpp
	Graphics.h				Graphics.cpp
	RenderPath.h			RenderPath.cpp
	Shader.h				Shader.cpp
	Material.h				Material.cpp
	PostProcess.h			PostProcess.cpp
)

set(IO
	FileSystem.h			FileSystem.cpp
	TextDocument.h			TextDocument.cpp	
)

set(CORE
	RefCounted.h 			RefCounted.cpp
	Device.h 				Device.cpp
	System.h				System.cpp
)


set(WINDOW
	InputState.h
	Keyboard.cpp 			Keyboard.h
	Mouse.cpp 				Mouse.h
	WindowEventReceiver.cpp WindowEventReceiver.h
	Window.cpp 				Window.h
	Display.cpp 			Display.h
	Context.cpp 			Context.h
)

set(SCENE
	World.h 				World.cpp
	SimObject.h 			SimObject.cpp
	VisualObject.h 			VisualObject.cpp
	Camera.h				Camera.cpp
	ModelInstance.h			ModelInstance.cpp
	MultiModelInstance.h 	MultiModelInstance.cpp
	Light.h					Light.cpp
	AmbientLight.h			AmbientLight.cpp
	DirectionalLight.h		DirectionalLight.cpp
	PointLight.h			PointLight.cpp
	SpotLight.h				SpotLight.cpp
	UICanvas.h 				UICanvas.cpp
)

set(UI
	UI.h UI.cpp 
	SystemUI.h SystemUI.cpp 
)

set(MATH	
	Vec2.h 			Vec2.cpp	
	Vec3.h 			Vec3.cpp
	Vec4.h 			Vec4.cpp
	Mat4.h 			Mat4.cpp	
	Quat.h 			Quat.cpp
	BoundingBox.h 	BoundingBox.cpp
	Sphere.h 		Sphere.cpp
)

add_library(Gateway STATIC
	${GRAPHICS} ${CORE} ${IO} ${SCENE} ${MATH} ${WINDOW} ${UI}
)

source_group("Graphics"
	FILES ${GRAPHICS}
)
source_group("Scene"
	FILES ${SCENE}
)
source_group("IO"
	FILES ${IO}
)
source_group("Core"
	FILES ${CORE}
)
source_group("Math"
	FILES ${MATH}
)
source_group("Window"
	FILES ${WINDOW}
)

source_group("UI"
	FILES ${UI}
)


set_property(TARGET Gateway
	PROPERTY CXX_STANDARD 20
)

target_include_directories(Gateway
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Gateway
	PRIVATE
		SDL2
		GLAD
		nlohmann_json::nlohmann_json
		assimp
		STBI
	
	PUBLIC # let applications linking Gateway also include these libs
		RmlCore
		RmlDebugger
		toml11
		ImGui
		Tracy::TracyClient
)


add_custom_command(
	TARGET Gateway
	COMMAND ${CMAKE_COMMAND}
	ARGS -E create_symlink ${GATEWAY_ROOT}/GameData ${GATEWAY_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/GameData/
)