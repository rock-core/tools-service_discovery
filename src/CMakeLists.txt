set(SOURCES
    client.cpp
    local_service.cpp
    remote_service.cpp
    service.cpp
    service_browser.cpp
    service_discovery.cpp
    service_description.cpp 
    service_event.cpp
    service_configuration.cpp
    service_pattern.cpp
)

add_executable(test_servicediscovery testmain.cpp ${SOURCES})
target_link_libraries(test_servicediscovery ${LIBRARIES})

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)	

add_subdirectory(examples)
