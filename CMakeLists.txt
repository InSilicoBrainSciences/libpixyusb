cmake_minimum_required (VERSION 3.0)

file(STRINGS "cmake/VERSION" LIBPIXY_VERSION)
add_definitions(-D__LIBPIXY_VERSION__="${LIBPIXY_VERSION}")

project (libpixyusb LANGUAGES CXX VERSION "${LIBPIXY_VERSION}")

set (CMAKE_CXX_STANDARD 11)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
find_package(libusb-1.0 REQUIRED)
add_definitions(-D__MACOS__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
find_package(libusb-1.0 REQUIRED)
add_definitions(-D__LINUX__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
set(LIBUSB_1_LIBRARY "${CMAKE_SOURCE_DIR}/windows/libusb-1.0.lib") 
set(LIBUSB_1_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/windows/")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
configure_file("${CMAKE_SOURCE_DIR}/windows/libusb-1.0.dll" 
			   "${CMAKE_BINARY_DIR}/libusb-1.0.dll" COPYONLY)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")


include_directories (src
                     src/util
                     include
                     ${LIBUSB_1_INCLUDE_DIRS})

add_library(pixyusb SHARED src/chirpreceiver.cpp
                           src/pixyinterpreter.cpp
                           src/pixyhandle.cpp
                           src/pixy.cpp
                           src/usblink.cpp
                           src/utils/timer.cpp
                           src/chirp.cpp)
target_link_libraries(pixyusb ${LIBUSB_1_LIBRARY})

add_executable(hello_pixy hello_pixy.cpp)
target_link_libraries(hello_pixy pixyusb)

add_executable(hello_pixies hello_pixies.cpp)
target_link_libraries(hello_pixies pixyusb)

install (TARGETS pixyusb
         DESTINATION lib)
install (FILES include/pixy.h
               include/pixyhandle.hpp 
			   include/pixydefs.h
         DESTINATION include)
install (TARGETS hello_pixy
                 hello_pixies
         DESTINATION bin)
