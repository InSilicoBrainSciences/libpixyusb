#include <stdio.h>

#include <boost/container/map.hpp>
#include <boost/shared_ptr.hpp>

#include "pixy.h"
#include "pixydebug.h"
#include "pixyinterpreter.hpp"

using boost::container::map;
using boost::shared_ptr;

map<int, shared_ptr<PixyInterpreter> > interpreter;

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
  - <a href=http://www.boost.org>libboost</a>

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
  static struct
  {
    int           error;
    const char *  text;
  } PIXY_ERROR_TABLE[] = {
    { 0,                          "Success" },
    { PIXY_ERROR_USB_IO,          "USB Error: I/O" },
    { PIXY_ERROR_USB_BUSY,        "USB Error: Busy" },
    { PIXY_ERROR_USB_NO_DEVICE,   "USB Error: No device" },
    { PIXY_ERROR_USB_NOT_FOUND,   "USB Error: Target not found" },
    { PIXY_ERROR_CHIRP,           "Chirp Protocol Error" },
    { PIXY_ERROR_INVALID_COMMAND, "Pixy Error: Invalid command" },
    { PIXY_ERROR_INVALID_ID,      "Pixy Error: Invalid ID" },
    { 0,                          0 }
  };

  // static int pixy_initialized = false;

  int pixy_init(int pixy_id)
  {
    // PIXYDEBUG("Entering pixy_init\n");
    fprintf(stderr, "Entering pixy_init\n");
    // TODO: Determine if this is the right set of error codes.
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      fprintf(stderr, "Existing interpreter found for pixy with id %d\n", pixy_id);
      return PIXY_ERROR_INVALID_ID;  
    }

    int return_value;

    fprintf(stderr, "Generating new interpreter for pixy with id %d\n", pixy_id);
    interpreter.emplace(pixy_id, shared_ptr<PixyInterpreter>(new PixyInterpreter));
    return_value = interpreter[pixy_id]->init();

    // TODO: Determine if I need to check if the pixy has been initialized.
    if(return_value != 0) 
    {
      fprintf(stderr, "Could not initialize interpreter for pixy with id %d\n", pixy_id);
      interpreter.erase(pixy_id);
    }

    return return_value;
  }

  int pixy_get_blocks(int pixy_id, uint16_t max_blocks, struct Block * blocks)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      return interpreter[pixy_id]->get_blocks(max_blocks, blocks);
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_blocks_are_new(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      return interpreter[pixy_id]->blocks_are_new();
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_command(int pixy_id, const char *name, ...)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      va_list arguments;
      int     return_value;

      // if(!pixy_initialized) return -1;

      va_start(arguments, name);
      return_value = interpreter[pixy_id]->send_command(name, arguments);
      va_end(arguments);

      return return_value;
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  void pixy_close(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      // if(!pixy_initialized) return;

      interpreter[pixy_id]->close();
      interpreter.erase(pixy_id);
    }
  }

  void pixy_error(int pixy_id, int error_code)
  {
    int index;

    // Convert pixy error code to string and display to stdout //

    index = 0;

    while(PIXY_ERROR_TABLE[index].text != 0) {

      if(PIXY_ERROR_TABLE[index].error == error_code) {
        fprintf(stderr, "(Pixy ID: %d) %s\n", pixy_id, PIXY_ERROR_TABLE[index].text);
        return;
      }

      index += 1;
    }

    fprintf(stderr, "(Pixy ID: %d) Undefined error: [%d]\n", pixy_id, error_code);
  }

  int pixy_led_set_RGB(int pixy_id, uint8_t red, uint8_t green, uint8_t blue)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      chirp_response;
      int      return_value;
      uint32_t RGB;

      // Pack the RGB value //
      RGB = blue + (green << 8) + (red << 16);

      return_value = pixy_command(pixy_id, "led_set", INT32(RGB), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

     if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_led_set_max_current(int pixy_id, uint32_t current)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int chirp_response;
      int return_value;

      return_value = pixy_command(pixy_id, "led_setMaxCurrent", INT32(current), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

     if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_led_get_max_current(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;

      return_value = pixy_command(pixy_id, "led_getMaxCurrent", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_set_auto_white_balance(int pixy_id, uint8_t enable)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;

      return_value = pixy_command(pixy_id, "cam_setAWB", UINT8(enable), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

     if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_get_auto_white_balance(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;

      return_value = pixy_command(pixy_id, "cam_getAWB", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  uint32_t pixy_cam_get_white_balance_value(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;

      return_value = pixy_command(pixy_id, "cam_getWBV", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

     if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_set_white_balance_value(int pixy_id, uint8_t red, uint8_t green, uint8_t blue)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;
      uint32_t white_balance;

      white_balance = green + (red << 8) + (blue << 16);

      return_value = pixy_command(pixy_id, "cam_setAWB", UINT32(white_balance), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

     if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_set_auto_exposure_compensation(int pixy_id, uint8_t enable)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;

      return_value = pixy_command(pixy_id, "cam_setAEC", UINT8(enable), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }
  
  int pixy_cam_get_auto_exposure_compensation(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;

      return_value = pixy_command(pixy_id, "cam_getAEC", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_set_exposure_compensation(int pixy_id, uint8_t gain, uint16_t compensation)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int      return_value;
      uint32_t chirp_response;
      uint32_t exposure;

      exposure = gain + (compensation << 8);

      return_value = pixy_command(pixy_id, "cam_setECV", UINT32(exposure), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_get_exposure_compensation(int pixy_id, uint8_t * gain, uint16_t * compensation)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      uint32_t exposure;
      int      return_value;

      return_value = pixy_command(pixy_id, "cam_getECV", END_OUT_ARGS, &exposure, END_IN_ARGS);

      if (return_value < 0) {
        // Chirp error //
        return return_value;
      }

      if(gain == 0 || compensation == 0) {
        // Error: Null pointer //
        return PIXY_ERROR_INVALID_PARAMETER;
      }

      fprintf(stderr, "exp:%08x\n", exposure);

      *gain         = exposure & 0xFF;
      *compensation = 0xFFFF & (exposure >> 8);

      return 0;
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_set_brightness(int pixy_id, uint8_t brightness)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int chirp_response;
      int return_value;

      return_value = pixy_command(pixy_id, "cam_setBrightness", UINT8(brightness), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

     if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_cam_get_brightness(int pixy_id)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int chirp_response;
      int return_value;

      return_value = pixy_command(pixy_id, "cam_getBrightness", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_rcs_get_position(int pixy_id, uint8_t channel)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int chirp_response;
      int return_value;

      return_value = pixy_command(pixy_id, "rcs_getPos", UINT8(channel), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_rcs_set_position(int pixy_id, uint8_t channel, uint16_t position)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int chirp_response;
      int return_value;

      return_value = pixy_command(pixy_id, "rcs_setPos", UINT8(channel), INT16(position), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_rcs_set_frequency(int pixy_id, uint16_t frequency)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      int chirp_response;
      int return_value;

      return_value = pixy_command(pixy_id, "rcs_setFreq", UINT16(frequency), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      } else {
        // Success //
        return chirp_response;
      }
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }

  int pixy_get_firmware_version(int pixy_id, uint16_t * major, uint16_t * minor, uint16_t * build)
  {
    if (interpreter.find(pixy_id) != interpreter.cend()) {
      uint16_t * pixy_version;
      uint32_t   version_length;
      uint32_t   response;
      uint16_t   version[3];
      int        return_value;
      int        chirp_response;

      if(major == 0 || minor == 0 || build == 0) {
        // Error: Null pointer //
        return PIXY_ERROR_INVALID_PARAMETER;
      }

      return_value = pixy_command(pixy_id, "version",  END_OUT_ARGS, &response, &version_length, &pixy_version, END_IN_ARGS);

      if (return_value < 0) {
        // Error //
        return return_value;
      }

      memcpy((void *) version, pixy_version, 3 * sizeof(uint16_t));

      *major = version[0];
      *minor = version[1];
      *build = version[2];

      return 0;
    } else {
      return PIXY_ERROR_INVALID_ID;
    }
  }
}
