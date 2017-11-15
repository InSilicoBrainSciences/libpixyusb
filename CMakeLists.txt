cmake_minimum_required (VERSION 2.8)
project (libpixyusb CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_MULTITHREADED ON)

find_package(libusb-1.0 REQUIRED)
find_package(Boost 1.49 COMPONENTS thread system REQUIRED)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-D__MACOS__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
add_definitions(-D__LINUX__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

file(STRINGS "cmake/VERSION" LIBPIXY_VERSION)
add_definitions(-D__LIBPIXY_VERSION__="${LIBPIXY_VERSION}")

include_directories (src
                     src/util
                     include
                     ${Boost_INCLUDE_DIR}
                     ${LIBUSB_1_INCLUDE_DIRS})

add_library(pixyusb SHARED src/chirpreceiver.cpp
                           src/pixyinterpreter.cpp
                           src/pixyhandle.cpp
                           src/pixy.cpp
                           src/usblink.cpp
                           src/utils/timer.cpp
                           src/chirp.cpp)
target_link_libraries(pixyusb ${Boost_LIBRARIES} ${LIBUSB_1_LIBRARY})

add_executable(hello_pixy hello_pixy.cpp)
target_link_libraries(hello_pixy pixyusb)

add_executable(hello_pixies hello_pixies.cpp)
target_link_libraries(hello_pixies pixyusb)


install (TARGETS pixyusb
         DESTINATION lib)
install (FILES include/pixy.h
               include/pixyhandle.hpp include/pixydefs.h
         DESTINATION include)
install (TARGETS hello_pixy
                 hello_pixies
         DESTINATION bin)
