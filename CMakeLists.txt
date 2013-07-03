# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)
find_package(Rock)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")
rock_init(service_discovery 1.0)
set(PROJECT_DESCRIPTION "Service discovery based on AVAHI")

find_package(Boost REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

pkg_check_modules(SIGC++ REQUIRED "sigc++-2.0")
## Using the quotation marks to get the ';' as field separator for string lists 
list(APPEND MY_CFLAGS ${SIGC++_INCLUDE_DIRS})
string(REGEX REPLACE ";" " -I" THIS_PKG_CFLAGS "${MY_CFLAGS}")
message(${THIS_PKG_CFLAGS})
set(THIS_PKG_CFLAGS "-I${THIS_PKG_CFLAGS}")
message(${THIS_PKG_CFLAGS})


## Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(bindings/ruby)

