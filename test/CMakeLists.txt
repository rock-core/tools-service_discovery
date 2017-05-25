rock_testsuite(servicediscovery-test
  suite.cpp
  discovery_test.cpp
  configuration_test.cpp
  search_pattern_test.cpp
  DEPS service_discovery)
    
add_executable(sdtest EXCLUDE_FROM_ALL
  test.cpp ${SOURCES}
)
target_link_libraries(sdtest service_discovery)

