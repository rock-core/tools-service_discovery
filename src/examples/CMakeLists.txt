set(EXAMPLES
    browse
    browse-publish-update
    browse-simplepoll
    publish
    servicediscovery
    logging
)

include_directories(${AVAHI_INCLUDES} ${SIGC++_INCLUDES})

# build all defined examples
foreach(currentExample ${EXAMPLES})
  add_executable(${currentExample} ${currentExample}.cpp)
  target_link_libraries(${currentExample} ${PROJECT_NAME} ${LIBRARIES})
endforeach(currentExample)


