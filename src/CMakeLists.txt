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

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})

set(UTIL_BROWSE ${PROJECT_NAME}-browse)
add_executable(${UTIL_BROWSE} utils/browse.cpp)
target_link_libraries(${UTIL_BROWSE} ${PROJECT_NAME})

set(UTIL_PUBLISH ${PROJECT_NAME}-publish)
add_executable(${UTIL_PUBLISH} utils/publish.cpp)
target_link_libraries(${UTIL_PUBLISH} ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} ${UTIL_BROWSE} ${UTIL_PUBLISH}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)	

add_subdirectory(examples)
