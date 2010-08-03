set(EXAMPLES
    browse
    browse-publish-update
    browse-simplepoll
    publish
    servicediscovery
    logging
)

# build all defined examples
foreach(currentExample ${EXAMPLES})
  add_executable(${currentExample} ${currentExample}.cpp)
  target_link_libraries(${currentExample} ${PROJECT_NAME} ${EXTERNAL_LIBS})
endforeach(currentExample)


