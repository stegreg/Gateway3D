function(define_example ExampleName FileName)
	add_executable(${ExampleName} ${FileName})
	
	target_link_libraries(${ExampleName}
		PRIVATE
			Gateway
	)
	
	set_property(TARGET ${ExampleName}
		PROPERTY 
			VS_DEBUGGER_WORKING_DIRECTORY "${GATEWAY_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
	)
	
endfunction()

define_example(01_HelloWindow 				HelloWindow.cpp)
define_example(02_HelloSplashScreen			HelloSplashScreen.cpp)
define_example(03_HelloCube   				HelloCube.cpp)
define_example(04_HelloMultipleWindows		HelloMultipleWindows.cpp)
define_example(05_HelloScene  				HelloScene.cpp)
define_example(06_HelloMultiModelInstance 	HelloMultiModelInstance.cpp)
define_example(07_HelloSceneLoading			HelloSceneLoading.cpp)
