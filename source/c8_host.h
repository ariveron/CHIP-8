#ifndef c8_HOST_H
#define c8_HOST_H

#include "c8.h"

// Host functions, must be implemented for each host platform
int c8_host_init(const char* title, int width, int height, int scale);
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