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

#ifndef __PIXY_HANDLE_H__
#define __PIXY_HANDLE_H__

#include <stdint.h>
#include <unistd.h>
#include <pixydefs.h>

#include <boost/shared_ptr.hpp>

#include "pixy.h"

class PixyInterpreter;

class PixyHandle {
public:
  PixyHandle()
  {}

  ~PixyHandle()
  {}

  int init();
  int blocks_are_new();
  int get_blocks(uint16_t max_blocks, struct Block *blocks);
  int command(const char *name, ...);
  int command(const char *name, va_list args);
  void close();
  void error(int error_code);

  int led_set_RGB(uint8_t red, uint8_t green, uint8_t blue);
  int led_set_max_current(uint32_t current);
  int led_get_max_current();
  int set_auto_white_balance(uint8_t enable);
  int get_auto_white_balance();
  int set_white_balance_value(uint8_t red, uint8_t green, uint8_t blue);
  int get_white_balance_value();
  int set_auto_exposure_compensation(uint8_t enable);
  int get_auto_exposure_compensation();
  int set_exposure_compensation(uint8_t gain, uint16_t compensation);
  int get_exposure_compensation(uint8_t *gain, uint16_t *compensation);
  int set_brightness(uint8_t brightness);
  int get_brightness();
  int rcs_get_position(uint8_t channel);
  int rcs_set_position(uint8_t channel, uint16_t position);
  int rcs_set_frequency(uint16_t frequency);
  int get_firmware_version(uint16_t *major, uint16_t *minor, uint16_t *build);

  bool available() const { return available_; }

  static int num_pixies_attached();
  static int num_pixies_in_use();
private:
  bool available_;
  boost::shared_ptr<PixyInterpreter> interpreter_;
};

#endif // __PIXY_HANDLE_H__
