include(RockRuby)
include_directories(${CMAKE_BINARY_DIR}/include)
rock_find_pkgconfig(BASE_TYPES REQUIRED base-lib)
rock_find_pkgconfig(SIGC_TYPES REQUIRED sigc++-2.0)
rock_ruby_rice_extension(servicediscovery_ruby ext/ServiceDiscovery.cpp)
target_link_libraries(servicediscovery_ruby service_discovery)
set(FILES lib/servicediscovery.rb
    lib/servicediscovery/core.rb
    )

rock_ruby_library(servicediscovery ${FILES})

