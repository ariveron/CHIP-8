add_executable(chip8 main.c c8.c c8_fontset.c c8_util.c c8_host_sdl2.c)

set_property(TARGET chip8 PROPERTY C_STANDARD 11)
set_property(TARGET chip8 PROPERTY C_STANDARD_REQUIRED ON)

target_link_libraries(chip8 PRIVATE SDL2::SDL2main SDL2::SDL2-static m)