set(EXAMPLES
    browse
    browse-publish-update
    publish
    servicediscovery
    update
)

# build all defined examples
foreach(currentExample ${EXAMPLES})
  add_executable(${currentExample} ${currentExample}.cpp)
  target_link_libraries(${currentExample} ${PROJECT_NAME} ${EXTERNAL_LIBS}
  -lpthread)
endforeach(currentExample)


