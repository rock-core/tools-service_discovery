set(SOURCES
    impl/avahi/Client.cpp
    impl/avahi/LocalService.cpp
    impl/avahi/RemoteService.cpp
    impl/avahi/Service.cpp
    impl/avahi/ServiceBrowser.cpp
    impl/avahi/ServiceDiscovery.cpp
    impl/avahi/ServiceDescription.cpp
    impl/avahi/ServiceEvent.cpp
    impl/avahi/ServiceConfiguration.cpp
    impl/avahi/ServicePattern.cpp
)

set( HEADERS
    impl/avahi/ServiceDiscovery.hpp
    impl/avahi/Service.hpp
    impl/avahi/ServiceDescription.hpp
    impl/avahi/ServicePattern.hpp
    impl/avahi/LocalService.hpp
    impl/avahi/RemoteService.hpp
    utils/List.hpp
    impl/avahi/Client.hpp
    impl/avahi/ServiceBrowser.hpp
    impl/avahi/ServiceConfiguration.hpp
    impl/avahi/ServiceEvent.hpp
    servicediscovery/backward/servicediscovery/service_discovery.h
)

find_package(Threads)
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
target_link_libraries(${PROJECT_NAME} ${Boost_REGEX_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

set(UTIL_BROWSE ${PROJECT_NAME}-browse)
rock_executable(${UTIL_BROWSE} utils/Browse.cpp
                     DEPS ${PROJECT_NAME})
set(UTIL_PUBLISH ${PROJECT_NAME}-publish)

rock_executable(${UTIL_PUBLISH} utils/Publish.cpp
                     DEPS ${PROJECT_NAME})

add_subdirectory(examples)
