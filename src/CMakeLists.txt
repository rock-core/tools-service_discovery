
####### Uncomment the following block for Qt setup
## include_directories(${CMAKE_CURRENT_BINARY_DIR})
####### Uncomment the following block for Qt setup
## include_directories(${CMAKE_CURRENT_BINARY_DIR})
## # Add your .ui files ot the following line (and uncomment it)
## QT4_WRAP_UI(ui_cpp myproject.ui)
## # Add the .h files that define QObject subclasses (that have a Q_OBJECT
## # statement)
## QT4_WRAP_CPP(moc_cpp dummy.h)

####### Define a binary
## Note: the library (see below) and executable names can't be the same
#add_executable(${PROJECT_NAME} main.cpp dummy.cpp ${moc_cpp} ${ui_cpp})

####### Define a library
## Note: the library and executable names can't be the same
##add_library(${PROJECT_NAME} SHARED dummy.cpp ${EXTERNAL_LIBS} ${moc_cpp} ${ui_cpp})
## Link the defined target (either binary or library, see above) to libraries.
## You must use ${QT_LIBRARIES} if you have enabled Qt support in the root
## CMakeLists.txt, and ${Boost_LIBRARIES} for boost libraries
##
## If you want to use a library defined in this package, just use its name
##
##target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})

#install(TARGETS ${PROJECT_NAME} 
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#)
 
set(SOURCES
    afAvahiClient.cpp
    afList.cpp
    afLocalService.cpp
    afPoll.cpp
    afRemoteService.cpp
    afService.cpp
    afServiceBase.cpp
    afServiceBrowser.cpp
    afSimplePoll.cpp
    afThreadPoll.cpp
    afLoggingWrapper.cpp
    afConfigurator.cpp
    OrocosComponentService.cpp
    ServiceDiscovery.cpp
)

#set(HEADERS
#    afAvahiFramework.h
#    afAvahiClient.h
#    afList.h
#    afLocalService.h
#    afPoll.h
#    afRemoteService.h
#    afService.h
#    afServiceBase.h
#    afServiceBrowser.h
#    afSimplePoll.h
#    afThreadPoll.h
#)


include_directories(${AVAHI_INCLUDES} ${SIGC++_INCLUDES} ${LOG4CXX_INCLUDE_DIR})

set(LIBRARIES ${AVAHI_LIBRARIES} ${SIGC++_LIBRARY} ${LOG4CXX_LIBRARIES})

#add_executable(testavahi-framework testmain.cpp ${SOURCES})
#target_link_libraries(testavahi-framework ${LIBRARIES})

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)	

add_subdirectory(examples)
