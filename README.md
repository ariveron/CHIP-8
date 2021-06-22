# CHIP-8

A cross-platform CHIP-8 emulator implemented in C with SDL2.

For CHIP-8 input use the following QWERTY to CHIP-8 keyboard mappings:
```
1 2 3 4  ->  1 2 3 C
Q W E R  ->  4 5 6 D
A S D F  ->  7 8 9 E
Z X C V  ->  A 0 B F
```

Additional emulator controls include:
- Pause program `SPACE`
- Reset program `BACK`
- CPU speed up `+`
- CPU speed down `=`
- CPU speed reset `0`

To build the project as is:
- Install CMAKE, Git, and a C compiler compatible with your platform, such as MSVC, GCC, or CLang
- Clone this repo, `git clone https://github.com/ariveron/CHIP-8.git`
- Go into the repo folder, `cd CHIP-8`
- Run the `premake.sh` on Linux/Mac or `premake.bat` on Windows to install VCPKG and SDL2 locally under this folder
- Create a `build` folder, `mkdir build`
- Go into the `build` folder, `cd build`
- Create the build files, `cmake .`
- Build the program, `cmake build .`
- The executable should be in the `build/bin/` folder

To run a CHIP-8 ROM, use the following command:
```
./chip8 <ROM FILE PATH>
```

The ROMs folder in this repo includes some CHIP-8 programs from the public domain.
