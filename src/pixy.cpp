#include <stdio.h>

#include <memory>

#include "pixy.h"
#include "pixyhandle.hpp"

using std::shared_ptr;

PixyHandle handle;

/** 

  \mainpage libpixyusb-0.4 API Reference

  \section introduction Introduction

  libpixyusb is an open source library that allows you to communicate with
  Pixy over the USB protocol.

  This documentation is aimed at application developers wishing to send
  commands to Pixy or read sensor data from Pixy.

  \section library_features Library features

  - Read blocks with or without color codes
  - RGB LED control (color/intensity)
  - Auto white balance control
  - Auto exposure compensation control
  - Brightness control
  - Servo position control/query
  - Custom commands

  \section dependencies Dependencies

  Required to build:

  - <a href=http://www.cmake.org>cmake</a>
 
  Required for runtime:

  - <a href=http://www.libusb.org>libusb</a>


  \section getting_started Getting Started
 
  The libpixyusb API reference documentation can be found here:

  libpixyusb API Reference

  Some tutorials that use libpixyusb can be found here:

  <a href=http://cmucam.org/projects/cmucam5/wiki/Hooking_up_Pixy_to_a_Raspberry_Pi>Hooking up Pixy to a Raspberry Pi</a>

  <a href=http://cmucam.org/projects/cmucam5/wiki/Hooking_up_Pixy_to_a_Beaglebone_Black>Hooking up Pixy to a BeagleBone Black</a>

  \section getting_help Getting Help

  Tutorials, walkthroughs, and more are available on the Pixy wiki page:

  <a href=http://www.cmucam.org/projects/cmucam5/wiki>Pixy Developer Wiki Page</a>

  Our friendly developers and users might be able to answer your question on the forums:

  <a href=http://www.cmucam.org/projects/cmucam5/boards/9>Pixy Software Discussion Forum</a>
  
  <a href=http://www.cmucam.org/projects/cmucam5/boards/8>Pixy Hardware Discussion Forum</a>

*/

// Pixy C API //

extern "C" 
{
  struct PixyError PIXY_ERROR_TABLE[NUM_PIXY_ERRORS] = {
    { 0,                          "Success" },
    { PIXY_ERROR_USB_IO,          "USB Error: I/O" },
    { PIXY_ERROR_USB_BUSY,        "USB Error: Busy" },
    { PIXY_ERROR_USB_NO_DEVICE,   "USB Error: No device" },
    { PIXY_ERROR_USB_NOT_FOUND,   "USB Error: Target not found" },
    { PIXY_ERROR_CHIRP,           "Chirp Protocol Error" },
    { PIXY_ERROR_INVALID_COMMAND, "Pixy Error: Invalid command" },
    { PIXY_ERROR_INITIALIZED,     "Pixy Error: Initialized" },
    { PIXY_ERROR_UNINITIALIZED,   "Pixy Error: Uninitialized" },
    { 0,                          0 }
  };

  const char* pixy_library_version() {
    return __LIBPIXY_VERSION__;
  }

  int pixy_init()
  {
    return handle.init();
  }

  int pixy_get_blocks(uint16_t max_blocks, struct Block * blocks)
  {
    return handle.get_blocks(max_blocks, blocks);
  }

  int pixy_blocks_are_new()
  {
    return handle.blocks_are_new();
  }

  int pixy_command(const char *name, ...)
  {
    va_list arguments;
    int return_value;

    va_start(arguments, name);
    return_value = handle.command(name, arguments);
    va_end(arguments);

    return return_value;
  }

  void pixy_close()
  {
    handle.close();
  }

  void pixy_error(int error_code)
  {
    handle.error(error_code);
  }

  int pixy_led_set_RGB(uint8_t red, uint8_t green, uint8_t blue)
  {
    return handle.led_set_RGB(red, green, blue);
  }

  int pixy_led_set_max_current(uint32_t current)
  {
    return handle.led_set_max_current(current);
  }

  int pixy_led_get_max_current()
  {
    return handle.led_get_max_current();
  }

  int pixy_cam_set_auto_white_balance(uint8_t enable)
  {
    return handle.set_auto_white_balance(enable);
  }

  int pixy_cam_get_auto_white_balance()
  {
    return handle.get_auto_white_balance();
  }

  uint32_t pixy_cam_get_white_balance_value()
  {
    return handle.get_white_balance_value();
  }

  int pixy_cam_set_white_balance_value(uint8_t red, uint8_t green, uint8_t blue)
  {
    return handle.set_white_balance_value(red, green, blue);
  }

  int pixy_cam_set_auto_exposure_compensation(uint8_t enable)
  {
    return handle.set_auto_exposure_compensation(enable);
  }
  
  int pixy_cam_get_auto_exposure_compensation()
  {
    return handle.get_auto_exposure_compensation();
  }

  int pixy_cam_set_exposure_compensation(uint8_t gain, uint16_t compensation)
  {
    return handle.set_exposure_compensation(gain, compensation);
  }

  int pixy_cam_get_exposure_compensation(uint8_t * gain, uint16_t * compensation)
  {
    return handle.get_exposure_compensation(gain, compensation);
  }

  int pixy_cam_set_brightness(uint8_t brightness)
  {
    return handle.set_brightness(brightness);
  }

  int pixy_cam_get_brightness()
  {
    return handle.get_brightness();
  }

  int pixy_rcs_get_position(uint8_t channel)
  {
    return handle.rcs_get_position(channel);
  }

  int pixy_rcs_set_position(uint8_t channel, uint16_t position)
  {
    return handle.rcs_set_position(channel, position);
  }

  int pixy_rcs_set_frequency(uint16_t frequency)
  {
    return handle.rcs_set_frequency(frequency);
  }

  int pixy_get_firmware_version(uint16_t * major, uint16_t * minor, uint16_t * build)
  {
    return handle.get_firmware_version(major, minor, build);
  }
}
