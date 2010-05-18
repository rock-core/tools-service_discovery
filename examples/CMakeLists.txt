set(EXAMPLES
    browse
    browse-publish-update
    browse-simplepoll
    publish
    servicediscovery
)

# pkg config available?
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)

  # service-discovery available? 
  pkg_check_modules(AVAHI_FRAMEWORK "service-discovery")

  # set libs and includes 
  if(AVAHI_FRAMEWORK_FOUND)
  set(LIBRARIES 
    ${AVAHI_FRAMEWORK_LIBRARIES}
    ${AVAHI_LIBRARIES}
    ${SIGC++_LIBRARY}
  )
  include_directories(${AVAHI_FRAMEWORK_INCLUDE_DIRS} ${AVAHI_INCLUDES} ${SIGC++_INCLUDES})

  # build all defined examples
  foreach(currentExample ${EXAMPLES})
    add_executable(${currentExample} ${currentExample}.cpp)
    target_link_libraries(${currentExample} ${LIBRARIES})

    ## can be comment in if the xamples should be installed (to local/bin)
    #install(TARGETS ${currentExample}
	#  RUNTIME DESTINATION bin
	#  ARCHIVE DESTINATION lib
    #)	
  endforeach(currentExample)

  else(AVAHI_FRAMEWORK_FOUND)
    message("NOT FOUND: service-discovery")
  endif(AVAHI_FRAMEWORK_FOUND)

endif(PKG_CONFIG_FOUND)

