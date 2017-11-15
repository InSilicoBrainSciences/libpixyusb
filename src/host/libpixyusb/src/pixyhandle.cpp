//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <stdint.h>
#include <unistd.h>
#include <pixydefs.h>

#include <boost/container/map.hpp>
#include <boost/shared_ptr.hpp>

#include "pixy.h"
#include "pixyhandle.hpp"
#include "pixyinterpreter.hpp"

using boost::container::map;
using boost::shared_ptr;

map<uint8_t, shared_ptr<PixyInterpreter> > interpreters_;

int PixyHandle::init() 
{
  available_ = false;
  shared_ptr<PixyInterpreter> t_interpreter(new PixyInterpreter);
  int init_code = t_interpreter->init();
  if (init_code != 0) {
    return init_code;
  }
  
  interpreter_ = t_interpreter;
  available_ = true;
  return 0;
}

int PixyHandle::get_blocks(uint16_t max_blocks, struct Block *blocks) 
{
  if (interpreter_) {
    return interpreter_->get_blocks(max_blocks, blocks);
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::blocks_are_new() 
{
  if (interpreter_) {
    return interpreter_->blocks_are_new();
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::command(const char *name, ...) 
{
  if (interpreter_) {
    va_list arguments;
    int return_value;

    va_start(arguments, name);
    return_value = command(name, arguments);
    va_end(arguments);

    return return_value;
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::command(const char *name, va_list args) 
{
  if (interpreter_) {
    va_list arguments;
    int     return_value;

    va_copy(arguments, args);
    return_value = interpreter_->send_command(name, arguments);
    va_end(arguments);

    return return_value;
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

void PixyHandle::close() 
{
  if (interpreter_) {
    interpreter_->close();
    interpreter_.reset();
  }
}

void PixyHandle::error(int error_code)
{
  int index;

  // Convert pixy error code to string and display to stdout //
  index = 0;

  while(PIXY_ERROR_TABLE[index].text != 0) {

    if(PIXY_ERROR_TABLE[index].error == error_code) {
      fprintf(stderr, "%s\n", PIXY_ERROR_TABLE[index].text);
      return;
    }

    index += 1;
  }

  fprintf(stderr, "Undefined error: [%d]\n", error_code);
}

int PixyHandle::led_set_RGB(uint8_t red, uint8_t green, uint8_t blue) 
{
  if (interpreter_) {
    int      chirp_response;
    int      return_value;
    uint32_t RGB;

    // Pack the RGB value //
    RGB = blue + (green << 8) + (red << 16);

    return_value = command("led_set", INT32(RGB), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

   if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::led_set_max_current(uint32_t current) 
{
  if (interpreter_) {
    int chirp_response;
    int return_value;

    return_value = command("led_setMaxCurrent", INT32(current), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

   if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::led_get_max_current() 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;

    return_value = command("led_getMaxCurrent", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::set_auto_white_balance(uint8_t enable) 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;

    return_value = command("cam_setAWB", UINT8(enable), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

   if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::get_auto_white_balance() 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;

    return_value = command("cam_getAWB", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::get_white_balance_value() 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;

    return_value = command("cam_getWBV", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

   if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::set_white_balance_value(uint8_t red, uint8_t green, uint8_t blue)
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;
    uint32_t white_balance;

    white_balance = green + (red << 8) + (blue << 16);

    return_value = command("cam_setAWB", UINT32(white_balance), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

   if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::set_auto_exposure_compensation(uint8_t enable) 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;

    return_value = command("cam_setAEC", UINT8(enable), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::get_auto_exposure_compensation() 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;

    return_value = command("cam_getAEC", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::set_exposure_compensation(uint8_t gain, uint16_t compensation) 
{
  if (interpreter_) {
    int      return_value;
    uint32_t chirp_response;
    uint32_t exposure;

    exposure = gain + (compensation << 8);

    return_value = command("cam_setECV", UINT32(exposure), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::get_exposure_compensation(uint8_t *gain, uint16_t *compensation) 
{
  if (interpreter_) {
    uint32_t exposure;
    int      return_value;

    return_value = command("cam_getECV", END_OUT_ARGS, &exposure, END_IN_ARGS);

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
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::set_brightness(uint8_t brightness) 
{
  if (interpreter_) {
    int chirp_response;
    int return_value;

    return_value = command("cam_setBrightness", UINT8(brightness), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

   if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::get_brightness() 
{
  if (interpreter_) {
    int chirp_response;
    int return_value;

    return_value = command("cam_getBrightness", END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::rcs_get_position(uint8_t channel) 
{
  if (interpreter_) {
    int chirp_response;
    int return_value;

    return_value = command("rcs_getPos", UINT8(channel), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::rcs_set_position(uint8_t channel, uint16_t position) 
{
  if (interpreter_) {
    int chirp_response;
    int return_value;

    return_value = command("rcs_setPos", UINT8(channel), INT16(position), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::rcs_set_frequency(uint16_t frequency) 
{
  if (interpreter_) {
    int chirp_response;
    int return_value;

    return_value = command("rcs_setFreq", UINT16(frequency), END_OUT_ARGS, &chirp_response, END_IN_ARGS);

    if (return_value < 0) {
      // Error //
      return return_value;
    } else {
      // Success //
      return chirp_response;
    }
  } else {
    return PIXY_ERROR_UNINITIALIZED;
  }
}

int PixyHandle::num_pixies_attached() 
{
  return USBLink::numDevices();
}

int PixyHandle::num_pixies_in_use() 
{
  return USBLink::numDevicesInUse();
}

int PixyHandle::get_firmware_version(uint16_t *major, uint16_t *minor, uint16_t *build) 
{
  if (interpreter_) {
    if(!major || !minor || !build) {
      // Error: Null pointer //
      return PIXY_ERROR_INVALID_PARAMETER;
    }

    uint16_t * pixy_version;
    uint32_t   version_length;
    uint32_t   response;
    uint16_t   version[3];
    int        return_value;
    int        chirp_response;

    return_value = command("version",  END_OUT_ARGS, &response, &version_length, &pixy_version, END_IN_ARGS);

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
    return PIXY_ERROR_UNINITIALIZED;
  }
}
