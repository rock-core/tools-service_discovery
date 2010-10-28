# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(service-discovery)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Service discovery based on AVAHI")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)

### Uncomment if you want to use Boost. Use the first find_package stanza to
### search for headers only, and the second one if you need binary libraries
set(Boost_ADDITIONAL_VERSION 1.40 1.41)
#find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})

include(FindPkgConfig)
pkg_check_modules(SIGC++ "sigc++-2.0")
pkg_check_modules(LOG4CXX "liblog4cxx")
pkg_check_modules(AVAHICLIENT "avahi-client")
pkg_check_modules(AVAHICORE "avahi-core")

include_directories(${SIGC++_INCLUDE_DIRS})
link_directories(${SIGC++_LIBRARY_DIRS})

include_directories(${LOG4CXX_INCLUDE_DIRS})
link_directories(${LOG4CXX_LIBRARY_DIRS})

include_directories(${AVAHICLIENT_INCLUDE_DIRS})
link_directories(${AVAHICLIENT_LIBRARY_DIRS})

include_directories(${AVAHICORE_INCLUDE_DIRS})
link_directories(${AVAHICORE_LIBRARY_DIRS})

# Using the quotation marks to get the ';' as field separator for string lists 
set(MY_CFLAGS "-I${SIGC++_INCLUDE_DIRS} -I${AVAHICLIENT_INCLUDE_DIRS} -I${AVAHICORE_INCLUDE_DIRS}")
string(REGEX REPLACE "-I " "" MY_CFLAGS "${MY_CFLAGS}" )
string(REGEX REPLACE ";" " -I" THIS_PKG_CFLAGS "${MY_CFLAGS}")

#TODO: not checked for version: avahi version used: 0.6.25
# find_package(AVAHI REQUIRED)
#TODO: not checked for version: ligsigc++ version used: 2.0.18-2
# find_package(SIGC++ REQUIRED)
# find_package(Log4Cxx REQUIRED)

### Update external libraries that are available
set(EXTERNAL_LIBS ${Boost_LIBRARIES} ${SIGC++_LIBRARIES} ${LOG4CXX_LIBRARIES} ${AVAHICLIENT_LIBRARIES} ${AVAHICORE_LIBRARIES} )

include(CommonSetup)
# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)
add_subdirectory(test)


