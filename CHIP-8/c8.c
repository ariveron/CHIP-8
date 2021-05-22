#include "c8.h"

#include <string.h>

extern c8_byte c8_fontset[80];

inline c8_word c8_fetch_word(c8_byte* vm, c8_word address);
inline c8_byte c8_rand(c8* vm);
inline void c8_draw_sprite(c8* vm, c8_byte x, c8_byte y, c8_byte n);
inline void c8_reg_dump(c8* vm, c8_byte x);
inline void c8_reg_load(c8* vm, c8_byte x);
inline c8_byte c8_log2(c8_word x);
inline c8_word c8_converse_nonimplication(c8_word x, c8_word y);
inline c8_byte c8_await_key_press(c8* vm);

// **************************
// **   PUBLIC FUNCTIONS   **
// **************************

void c8_initialize(c8* vm)
{
    memset(vm, 0, sizeof(c8));
    memcpy_s(vm->fontset, sizeof(vm->fontset), c8_fontset, sizeof(c8_fontset));
    vm->pc = 0x200;
    vm->s1 = 0xf0;
    vm->s2 = 0x0f;
}

void c8_load_program(c8* vm, c8_byte* program, c8_word program_size)
{
    memcpy_s(vm->program, sizeof(vm->program), program, program_size);
}

void c8_cpu_cyle(c8* vm)
{
    // Fetch
    vm->opcode = c8_fetch_word((c8_byte*)vm, vm->pc);
    vm->pc += 2;

    c8_byte x = (vm->opcode & 0x0f00) >> 8;
    c8_byte y = (vm->opcode & 0x00f0) >> 4;
    c8_byte n = vm->opcode & 0x000f;
    c8_byte nn = vm->opcode & 0x00ff;
    c8_word nnn = vm->opcode & 0x0fff;

    c8_byte* mem = (c8_byte*)vm;

    // Decode & Execute
    switch (vm->opcode & 0xf000)
    {
    case 0x0000:
        switch (nn)
        {
        case 0xe0:			// CLS
            memset(vm->display_buffer, 0, sizeof(vm->display_buffer));
            break;
        case 0xee:			// RET 
            vm->sp--;
            vm->pc = vm->stack[vm->sp];
            break;
        default:			// SYS addr
            break;
        }
        break;
    case 0x1000:			// JP addr
        vm->pc = nnn;
        break;
    case 0x2000:			// CALL addr
        vm->stack[vm->sp] = vm->pc;
        vm->sp++;
        vm->pc = nnn;
        break;
    case 0x3000:			// SE Vx, byte
        vm->pc += 2 * (vm->v[x] == nn);
        break;
    case 0x4000:			// SNE Vx, byte
        vm->pc += 2 * (vm->v[x] != nn);
        break;
    case 0x5000:
        switch (n)
        {
        case 0x0:   		// SE Vx, Vy
            vm->pc += 2 * (vm->v[x] == vm->v[y]);
            break;
        default:
            break;
        }
        break;
    case 0x6000:			// LD Vx, byte
        vm->v[x] = nn;
        break;
    case 0x7000:			// ADD Vx, byte
        vm->v[x] += nn;
        break;
    case 0x8000:
        switch (n)
        {
        case 0x0:			// LD Vx, Vy
            vm->v[x] = vm->v[y];
            break;
        case 0x1:			// OR Vx, Vy
            vm->v[x] |= vm->v[y];
            break;
        case 0x2:			// AND Vx, Vy
            vm->v[x] &= vm->v[y];
            break;
        case 0x3:			// XOR Vx, Vy
            vm->v[x] ^= vm->v[y];
            break;
        case 0x4:			// ADD Vx, Vy
            vm->v[x] += vm->v[y];
            vm->v[0xf] = vm->v[x] < vm->v[y];
            break;
        case 0x5:			// SUB Vx, Vy
            vm->v[0xf] = vm->v[x] >= vm->v[y];
            vm->v[x] -= vm->v[y];
            break;
        case 0x6:			// SHR Vx {, Vy}
            vm->v[0xf] = vm->v[x] & 0b00000001;
            vm->v[x] >>= 1;
            break;
        case 0x7:			// SUBN Vx, Vy
            vm->v[0xf] = vm->v[y] >= vm->v[x];
            vm->v[x] = vm->v[y] - vm->v[x];
            break;
        case 0xe:			// SHL Vx {, Vy}
            vm->v[0xf] = !!(vm->v[x] & 0b10000000);
            vm->v[x] <<= 1;
            break;
        default:
            break;
        }
        break;
    case 0x9000:
        switch (n)
        {
        case 0x0:			// SNE Vx, Vy
            vm->pc += 2 * (vm->v[x] != vm->v[y]);
            break;
        default:
            break;
        }
        break;
    case 0xa000:			// LD I, addr
        vm->i = nnn;
        break;
    case 0xb000:			// JP V0, addr
        vm->pc = vm->v[0] + nnn;
        break;
    case 0xc000:			// RND Vx, byte
        vm->v[x] = c8_rand(vm) & nn;
        break;
    case 0xd000:			// DRW Vx, Vy, nibble
        c8_draw_sprite(vm, vm->v[x], vm->v[y], n);
        break;
    case 0xe000:
        switch (vm->opcode & 0xff)
        {
        case 0x9e:			// SKP Vx
            vm->pc += 2 * !!((1 << vm->v[x]) & vm->keys);
            break;
        case 0xa1:			// SKNP Vx
            vm->pc += 2 * !((1 << vm->v[x]) & vm->keys);
            break;
        default:
            break;
        }
        break;
    case 0xf000:
        switch (vm->opcode & 0xff)
        {
        case 0x07:		// LD Vx, DT
            vm->v[x] = vm->delay_timer;
            break;
        case 0x0a:		// LD Vx, K
            vm->v[x] = c8_await_key_press(vm);
            break;
        case 0x15:		// LD DT, Vx
            vm->delay_timer = vm->v[x];
            break;
        case 0x18:		// LD ST, Vx
            vm->sound_timer = vm->v[x];
            break;
        case 0x1e:		// ADD I, Vx
            vm->i += vm->v[x];
            break;
        case 0x29:		// LD F, Vx
            vm->i = 0x050 + (vm->v[x] * 5);
            break;
        case 0x33:		// LD B, Vx
            mem[vm->i + 0] = vm->v[x] / 100;
            mem[vm->i + 1] = vm->v[x] % 100 / 10;
            mem[vm->i + 2] = vm->v[x] % 10;
            break;
        case 0x55:		// LD [I], Vx
            c8_reg_dump(vm, x);
            break;
        case 0x65:		// LD Vx, [I]
            c8_reg_load(vm, x);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    vm->prev_keys = vm->keys;
}

// ***************************
// **   PRIVATE FUNCTIONS   **
// ***************************

inline c8_word c8_fetch_word(c8_byte* vm, c8_word address)
{
    // CHIP-8 memory is supposed to be Big Endian
    //return (c8_word)vm->program[address] << 8 | vm->program[address + 1];
    return (c8_word)vm[address] << 8 | vm[address + 1];
}

inline c8_byte c8_rand(c8* vm)
{
    // Simple PRNG algorithm that can be seeded
    vm->s1 = 15 * (vm->s1 & 0xf) + (vm->s1 >> 4);
    vm->s2 = 12 * (vm->s2 & 0xf) + (vm->s2 >> 4);
    return (vm->s1 << 4) + vm->s2;
}

inline void c8_draw_sprite(c8* vm, c8_byte x, c8_byte y, c8_byte n)
{
    // Wrap around past 63
    x %= 64;

    // Wrap around past 31
    y %= 32;

    // Get starting address of "8 by n" sprite
    c8_word source = vm->i;

    // CHIP-8 RAM to be able to use address correctly
    c8_byte* mem = (c8_byte*)vm;

    // Set collision flag to 0, will be set if a pixel is unset
    vm->v[0xf] = 0;

    // Find the bit shift, sprite pixels my not align to every byte, display buffer is a bit field
    c8_byte dspBitShft = x % 8;

    while (n > 0)
    {
        // Copy the pixels as they will be written to the display buffer
        c8_byte left = mem[source] >> dspBitShft;
        c8_byte right = mem[source] << (8 - dspBitShft);

        // Find the first byte in the destination
        c8_byte dspByteL = ((x / 8) % 8) + (y * 8);
        // Find the second byte in the destination
        c8_byte dspByteR = (((x / 8) + 1) % 8) + (y * 8);

        // Find the result of OR'ing the sprite pixels with the display pixels
        c8_byte leftOR = vm->display_buffer[dspByteL] | left;
        c8_byte rightOR = vm->display_buffer[dspByteR] | right;

        // Find the result of XOR'ing the sprite pixels with the display pixels
        c8_byte leftXOR = vm->display_buffer[dspByteL] ^ left;
        c8_byte rightXOR = vm->display_buffer[dspByteR] ^ right;

        // If XOR and OR give the same result then no pixels were unset, otherwise set collision flag
        vm->v[0xf] |= (leftXOR != leftOR) || (rightXOR != rightOR);

        // Write result to display buffer
        vm->display_buffer[dspByteL] = leftXOR;
        vm->display_buffer[dspByteR] = rightXOR;

        // Go to next row
        y = (y + 1) % 32;
        source++;
        n--;
    }
}

inline void c8_reg_dump(c8* vm, c8_byte x)
{
    c8_byte* mem = (c8_byte*)vm;

    for (c8_byte i = 0; i <= x && i < 16; i++)
    {
        mem[vm->i + i] = vm->v[i];
    }
}

inline void c8_reg_load(c8* vm, c8_byte x)
{
    c8_byte* mem = (c8_byte*)vm;

    for (c8_byte i = 0; i <= x && i < 16; i++)
    {
        vm->v[i] = mem[vm->i + i];
    }
}

inline c8_byte c8_log2(c8_word x)
{
    c8_byte log2 = 1;
    while ((x >> (log2)) > 0) log2++;
    return log2 - 1;
}

inline c8_word c8_converse_nonimplication(c8_word x, c8_word y)
{
    // x y -> y AND(x XOR y)
    // 1 1 -> 0
    // 1 0 -> 0
    // 0 1 -> 1
    // 0 0 -> 0
    return y & (x ^ y);
}

inline c8_byte c8_await_key_press(c8* vm)
{
    // If no key is pressed, repeat the last instruction until it is, and return key num
    c8_word keyPressed = c8_converse_nonimplication(vm->prev_keys, vm->keys);
    vm->pc -= 2 * !keyPressed;
    return c8_log2(keyPressed);
}
