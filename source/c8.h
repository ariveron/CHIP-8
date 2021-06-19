#ifndef C8_H
#define C8_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t c8_byte;
typedef uint16_t c8_word;

// Bit field values for CHIP-8 keys
#define C8_KEY_0 1
#define C8_KEY_1 2
#define C8_KEY_2 4
#define C8_KEY_3 8
#define C8_KEY_4 16
#define C8_KEY_5 32
#define C8_KEY_6 64
#define C8_KEY_7 128
#define C8_KEY_8 256
#define C8_KEY_9 512
#define C8_KEY_A 1024
#define C8_KEY_B 2048
#define C8_KEY_C 4096
#define C8_KEY_D 8192
#define C8_KEY_E 16384
#define C8_KEY_F 32768

// CHIP-8 entire state in 4096 bytes (4 KiB)
typedef struct c8
{
    c8_word pc;                     // 0x000            -> Program counter
    c8_word sp;                     // 0x002            -> Stack pointer
    c8_word i;                      // 0x004            -> Index register
    c8_word opcode;                 // 0x006            -> Current opcode
    c8_word prev_keys;              // 0x008            -> Bit field for previous keys (0 - F)
    c8_word keys;                   // 0x00a            -> Bit field for keys (0 - F)
    c8_word flags;                  // 0x00c            -> Bit field for op flags
    c8_word stack[23];              // 0x00e - 0x03a    -> Only stores return addresses
    c8_byte s1;                     // 0x03c            -> Seed for PRNG
    c8_byte s2;                     // 0x03d            -> Seed dfor PRNG
    c8_byte delay_timer;            // 0x03e            -> Decrements to 0 at 60 Hz
    c8_byte sound_timer;            // 0x03f            -> Decrements to 0 at 60 Hz, sound when nonzero
    c8_byte v[16];                  // 0x040 - 0x04f    -> General purpose registers, v[F] used as a flag
    c8_byte fontset[80];            // 0x050 - 0x09f    -> Built in 4x5 pixel font set (0 - F)
    c8_byte display_buffer[8 * 32]; // 0x0a0 - 0x19f    -> Bit field for 64x32 monocrhomatic pixels
    c8_byte buf_96[96];             // 0x1a0 - 0x1ff    -> Free 96 byte buffer
    c8_byte program[3584];          // 0x200 - 0xfff    -> Big-Endian program memory
} c8;

// Clear all memory, load fontset, and set program counter to 0x200
void c8_initialize(c8* vm);

// Emulate single CHIP-8 cpu cycle
void c8_cpu_cyle(c8* vm);

// Load a program into memory starting at 0x200
void c8_load_program(c8* vm, c8_byte* program, c8_word program_size);

// Emulate single CHIP-8 timer cycle
void c8_timer_cycle(c8* vm);

// Update all keys
void c8_update_keys(c8* vm, c8_word keys);

// Returns 1 if playing sounds, 0 otherwise
c8_byte c8_is_playing_sound(c8* vm);

// Utility functions
c8_word c8_util_file_to_byte_array(const char* filePath, c8_byte** buffer);

// Host functions, must be implemented for each host platform
int c8_host_init(const wchar_t* title, int width, int height, int scale);
void c8_host_cleanup(void);
uint64_t c8_host_get_100nanoseconds(void);
c8_word c8_host_get_keys(void);
void c8_host_make_sound(void);
int c8_host_is_not_quit(void);
int c8_host_is_speed_up(void);
int c8_host_is_speed_down(void);
int c8_host_is_speed_reset(void);
int c8_host_is_paused(void);
int c8_host_is_program_reset(void);
void c8_host_render(c8_byte* buf);
void c8_host_sleep(unsigned long milliseconds);

#endif
