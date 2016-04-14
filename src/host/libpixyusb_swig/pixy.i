%module pixy

%include "stdint.i"
%include "carrays.i"

%{
#define SWIG_FILE_WITH_INIT
#include "pixy.h"
%}

%array_class(struct Block, BlockArray);

int pixy_init(int pixy_id);
int pixy_get_blocks(int pixy_id, uint16_t max_blocks, BlockArray *blocks);
void pixy_close(int pixy_id);

struct Block
{
  uint16_t type;
  uint16_t signature;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  int16_t  angle;
};
