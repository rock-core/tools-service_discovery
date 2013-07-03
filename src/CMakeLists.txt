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

set( HEADERS
    service_discovery.h
    service.h
    service_description.h
    service_pattern.h
    local_service.h
    remote_service.h
    list.h
    client.h
    service_browser.h
    service_configuration.h
    service_event.h
)

rock_library(${PROJECT_NAME} 
                    SOURCES ${SOURCES}
                    HEADERS ${HEADERS}
                    DEPS_PKGCONFIG sigc++-2.0
                        base-lib
                        avahi-client
                        avahi-core
                        dbus-1
                    DEPS_PLAIN Threads
)
target_link_libraries(${PROJECT_NAME} ${Boost_REGEX_LIBRARY})

set(UTIL_BROWSE ${PROJECT_NAME}-browse)
rock_executable(${UTIL_BROWSE} utils/browse.cpp
                     DEPS ${PROJECT_NAME})
set(UTIL_PUBLISH ${PROJECT_NAME}-publish)

rock_executable(${UTIL_PUBLISH} utils/publish.cpp
                     DEPS ${PROJECT_NAME})

add_subdirectory(examples)
