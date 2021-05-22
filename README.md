# CHIP-8

A CHIP-8 emulator implemented in C. The project only support WIN32 for now, but could easily support any other platforms by reimplementing the `c8_host_*` functions for another platform.

This project doesn't use any third-party libraries, except for the platform specific libraries, `Windows.h`. The CHIP-8 is simple enough that there shouldn't be a need to use third-party libraries to implement any of the platform specific I/O.

For CHIP-8 input use the following keys: `1234QWERASDFZXCV`.

To pause hold the `SPACE` key, for reset the program use the `BACK` key, to adjust the CPU speed use the following keys: `0-=`.

To start a program just run `C8 <ROM>`.

The ROMs folder includes some CHIP-8 ROMs in the public domain.
