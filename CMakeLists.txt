# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(service_discovery)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Service discovery based on AVAHI")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_BUILD_TYPE Debug)
include(CommonSetup)
add_definitions(-Wall)
add_definitions(-DBASE_LOG_NAMESPACE=${PROJECT_NAME})

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
pkg_check_modules(SIGC++ REQUIRED "sigc++-2.0")
pkg_check_modules(BASE REQUIRED "base-lib")
pkg_check_modules(AVAHICLIENT REQUIRED "avahi-client")
pkg_check_modules(AVAHICORE REQUIRED "avahi-core")
pkg_check_modules(DBUS REQUIRED "dbus-1")

include_directories(${SIGC++_INCLUDE_DIRS})
link_directories(${SIGC++_LIBRARY_DIRS})

include_directories(${BASE_INCLUDE_DIRS})
message("BASE DIR ${BASE_INCLUDE_DIRS}")
link_directories(${BASE_LIBRARY_DIRS})

include_directories(${AVAHICLIENT_INCLUDE_DIRS})
link_directories(${AVAHICLIENT_LIBRARY_DIRS})

include_directories(${AVAHICORE_INCLUDE_DIRS})
link_directories(${AVAHICORE_LIBRARY_DIRS})

include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

# Using the quotation marks to get the ';' as field separator for string lists 
list(APPEND MY_CFLAGS ${SIGC++_INCLUDE_DIRS} ${BASE_INCLUDE_DIRS} ${AVAHICLIENT_INCLUDE_DIRS} ${AVAHICORE_INCLUDE_DIRS})
message(${MY_CFLAGS})
string(REGEX REPLACE ";" " -I" THIS_PKG_CFLAGS "${MY_CFLAGS}")
message(${THIS_PKG_CFLAGS})
set(THIS_PKG_CFLAGS "-I${THIS_PKG_CFLAGS}")
message(${THIS_PKG_CFLAGS})

find_package(Threads)

### Update external libraries that are available
set(EXTERNAL_LIBS ${Boost_LIBRARIES} ${SIGC++_LIBRARIES} ${BASE_LIBRARIES} ${AVAHICLIENT_LIBRARIES} ${AVAHICORE_LIBRARIES} ${DBUS_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(bindings/ruby)

include(PostSetup)


