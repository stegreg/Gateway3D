add_executable(Application main.cpp)

target_link_libraries(Application
	PRIVATE
		Gateway
)

set_property(TARGET Application
	PROPERTY 
		VS_DEBUGGER_WORKING_DIRECTORY "${GATEWAY_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")