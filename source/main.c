#include "c8.h"
#include "c8_host.h"
#include "c8_util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  // Parse command line args
  if (argc < 2)
  {
      printf("No ROM file specified\n");
      return 1;
  }
  const char* romPath = argv[1];

  // Init host environment
  if (c8_host_init("CHIP-8", 64, 32, 12) == 0)
  {
      printf("Unable to initialize host environment\n");
      return 1;
  }

  // Init CHIP-8 VM
  c8 vm;
  c8_initialize(&vm);

  // Get the program and load it into the VM
  uint8_t* program;
  c8_word programSize = c8_util_file_to_byte_array(romPath, &program);
  if (program == NULL || programSize == 0)
  {
      printf("Unable to use file %s\n", romPath);
      return 1;
  }
  c8_load_program(&vm, program, programSize);

  // Initialize internal timers
  uint64_t srt = c8_host_get_100nanoseconds();
  uint64_t end = srt;
  uint64_t dur = 0;
  uint64_t durCPU = 0;
  uint64_t durIO = 0;
  uint64_t durTimers = 0;

  // Rates are in 100-nanoseconds
  uint64_t rateCPU = 18518;      // 540 Hz
  uint64_t rateIO = 83333;       // 120 Hz
  uint64_t rateTimers = 166667;  //  60 Hz

  // Main loop
  while (c8_host_is_not_quit())
  {
      // Handle reset of program
      if (c8_host_is_program_reset())
      {
          c8_initialize(&vm);
          c8_load_program(&vm, program, programSize);
          durCPU = 0;
          durIO = 0;
          durTimers = 0;
      }

      // Handle timing and pauses
      do
      {
          c8_host_sleep(1);
          end = c8_host_get_100nanoseconds();
          dur = end - srt;
          srt = end;
      } while (c8_host_is_paused());

      // Run instructions
      durCPU += dur;
      while (durCPU > rateCPU)
      {
          c8_cpu_cyle(&vm);
          durCPU -= (rateCPU + 1);
      }

      // Update timers
      durTimers += dur;
      while (durTimers > rateTimers)
      {
          c8_timer_cycle(&vm);
          durTimers -= (rateTimers + 1);
      }

      // Handle I/O
      durIO += dur;
      if (durIO > rateIO)
      {
          // Input
          c8_update_keys(&vm, c8_host_get_keys());
          rateCPU -= 100ULL * c8_host_is_speed_up() * (rateCPU > 5000);
          rateCPU += 100ULL * c8_host_is_speed_down() * (rateCPU < 300000);
          if (c8_host_is_speed_reset()) rateCPU = 20000;

          // Output
          if (c8_is_playing_sound(&vm)) c8_host_make_sound();
          c8_host_render(vm.display_buffer);

          durIO = 0;
      }
  }

  // Cleanup resources
  free(program);
  c8_host_cleanup();
}
