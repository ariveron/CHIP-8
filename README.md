# CHIP-8

A CHIP-8 emulator implemented in C with no third-party dependencies. The project only supports WIN32 for now, but could easily support any other platform by reimplementing the `c8_host_*` functions for other platforms.

For CHIP-8 input use the following QWERTY to CHIP-8 keyboard mappings:
```
1 2 3 4  ->  1 2 3 C
Q W E R  ->  4 5 6 D
A S D F  ->  7 8 9 E
Z X C V  ->  A 0 B F
```

Additiona emulator controls include:
- Pause program `SPACE`
- Reset program `BACK`
- CPU speed up `+`
- CPU speed down `-`
- CPU speed reset `0`

To build the project as is:
- Install the MSVC v142 toolset with spectre mitigation
- Open `CHIP-8.sln` with Visual Studio 2019 and Build
- NOTE: You could build with a different toolset by changing the project settings

To run a program, use the following command in either CMD or PowerShell on Windows OS:
```
.\c8 <ROM FILE>
```

The ROMs folder includes some CHIP-8 programs from the public domain.
