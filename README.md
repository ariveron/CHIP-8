# CHIP-8

A CHIP-8 emulator implemented in C. The project only supports WIN32 for now, but could easily support any other platform by reimplementing the `c8_host_*` functions.

This project doesn't use any third-party libraries, except for the platform specific libraries, `Windows.h`. The CHIP-8 is simple enough that there shouldn't be a need to use third-party libraries to implement any of the platform specific functions.

For CHIP-8 input use the following keys: `1234QWERASDFZXCV`.

To pause hold the `SPACE` key, to reset the program use the `BACK` key, to adjust the CPU speed use the following keys: `0-=`.

To start a program, type the following command: `C8 <ROM>`, <ROM> is a ROM file for the CHIP-8.

The ROMs folder includes some CHIP-8 ROMs that are in the public domain.
