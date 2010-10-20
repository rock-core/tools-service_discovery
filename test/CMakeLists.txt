set(TEST_EXECUTABLE ${PROJECT_NAME}-test)

set(TEST_SOURCE
  testmain.cpp
  discovery_test.cpp
)

find_package(Boost COMPONENTS unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(${TEST_EXECUTABLE} EXCLUDE_FROM_ALL
  ${TEST_SOURCE} ${SOURCES}
)

target_link_libraries(${TEST_EXECUTABLE} 
  ${PROJECT_NAME} ${EXTERNAL_LIBS} ${Boost_LIBRARIES}
)

add_custom_target(test 
  ${TEST_EXECUTABLE} --log_level=message
  DEPENDS ${TEST_EXECUTABLE}
)
