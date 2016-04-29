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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <vector>

#include "pixyhandle.hpp"

#define BLOCK_BUFFER_SIZE    25

using std::vector;

// Pixy Block buffer // 

static bool run_flag = true;

void handle_SIGINT(int unused)
{
  // On CTRL+C - abort! //

  run_flag = false;
}

int main(int argc, char * argv[])
{
  // Catch CTRL+C (SIGINT) signals //
  signal(SIGINT, handle_SIGINT);

  int num_pixies = PixyHandle::num_pixies_attached();
  if (num_pixies <= 0) {
    fprintf(stderr, "Invalid number of pixies connected.\n");
    return EXIT_FAILURE;
  }

  fprintf(stderr, "Hello Pixies:\n libpixyusb Version: %s\n", __LIBPIXY_VERSION__);

  vector<int>         pixy_init_status(num_pixies);
  vector<PixyHandle>  pixy_handles(num_pixies);
  for (int i = 0; i < num_pixies; i++) {
    // Connect to Pixy //
    pixy_init_status[i] = pixy_handles[i].init();

    // Was there an error initializing pixy? //
    if(pixy_init_status[i] != 0)
    {
      // Error initializing Pixy //
      fprintf(stderr, "pixy_init(): ");
      pixy_handles[i].error(pixy_init_status[i]);

      return pixy_init_status[i];
    }

    // Request Pixy firmware version //
    {
      uint16_t major;
      uint16_t minor;
      uint16_t build;
      int      return_value;

      return_value = pixy_handles[i].get_firmware_version(&major, &minor, &build);

      if (return_value) {
        // Error //
        fprintf(stderr, "Failed to retrieve Pixy firmware version. ");
        pixy_handles[i].error(return_value);

        return return_value;
      } else {
        // Success //
        fprintf(stderr, " Pixy Firmware Version: %d.%d.%d\n", major, minor, build);
      }
    }
  }

  int      frame = 0;
  int      index;
  char     buf[128];

  vector<int>      blocks_copied(num_pixies);
  struct Block **blocks = new struct Block*[num_pixies];
  for (int i = 0 ; i < num_pixies; i++) {
    blocks[i] = new struct Block[BLOCK_BUFFER_SIZE];
  }

  fprintf(stderr, "Detecting blocks...\n");
  while(run_flag)
  {
    fprintf(stderr, "frame %d:\n", frame);
    for (int i = 0; i < num_pixies; i++) {
      // Wait for new blocks to be available //
      while(!pixy_handles[i].blocks_are_new() && run_flag); 

      // Get blocks from Pixy //
      blocks_copied[i] = pixy_handles[i].get_blocks(BLOCK_BUFFER_SIZE, &(blocks[i][0]));

      fprintf(stderr, " camera %d:\n", i);

      if(blocks_copied[i] < 0) {
        // Error: pixy_get_blocks //
        fprintf(stderr, "pixy_get_blocks(): ");
        pixy_handles[i].error(blocks_copied[i]);
      }

      // Display received blocks //
      for(index = 0; index != blocks_copied[i]; ++index) {    
         blocks[i][index].print(buf);
         fprintf(stderr, "  %s\n", buf);
      }
    }
    frame++;
  }

  for (int i = 0; i < num_pixies; i++) {
    pixy_handles[i].close();
  }

  for (int i = 0; i < num_pixies; i++) {
    delete[] blocks[i];
  }
  delete[] blocks;

  return EXIT_SUCCESS;
}
