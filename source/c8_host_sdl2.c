#ifndef NO_SDL

#include "c8_host.h"

#include <SDL2/SDL.h>
#include <stdlib.h>

void graphics_draw_pixels();
int graphics_init();
void graphics_cleanup();
typedef struct graphics_control
{
  int w, h, s;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *target;
  c8_byte *pixels;
  c8_byte on, off;
} graphics_control;
graphics_control graphics;

void controller_update();
int controller_init();
typedef struct input_control
{
  c8_word c8_keys;
  int isQuit;
  int isSpeedUp;
  int isSpeedDown;
  int isReset;
  int isPaused;
  int isProgramReset;
} input_control;
input_control controller;

void beeper_beep();
int beeper_init();
void beeper_cleanup();
void beeper_callback(void *user_data, Uint8 *raw_buffer, int bytes);
typedef struct beep_control
{
  int sample_rate;
  int amplitude;
  int sample_number;
  SDL_AudioSpec desired_spec;
  SDL_AudioSpec obtained_spec;
} beep_control;
beep_control beeper;

int c8_host_init(const wchar_t* title, int width, int height, int scale)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  if (!graphics_init(width, height, scale)) return 0;
  if (!controller_init()) return 0;
  if (!beeper_init()) return 0;
  return 1;
}

void c8_host_cleanup(void)
{
  beeper_cleanup();
  graphics_cleanup(); 
  SDL_Quit();
}

uint64_t c8_host_get_100nanoseconds(void)
{
  controller_update();
  return SDL_GetTicks() * 10000;
}

c8_word c8_host_get_keys(void)
{
  controller_update();
  return controller.c8_keys;
}

void c8_host_make_sound(void)
{
  beeper_beep(30);
}

int c8_host_is_not_quit(void)
{
  controller_update();
  return !controller.isQuit;
}

int c8_host_is_speed_up(void)
{
  controller_update();
  return controller.isSpeedUp;
}

int c8_host_is_speed_down(void)
{
  controller_update();
  return controller.isSpeedDown;
}

int c8_host_is_speed_reset(void)
{
  controller_update();
  return controller.isReset;
}

int c8_host_is_paused(void)
{
  controller_update();
  return controller.isPaused;
}

int c8_host_is_program_reset(void)
{
  controller_update();
  return controller.isProgramReset;
}

void c8_host_render(c8_byte* buf)
{
  controller_update();
  for (int i = 0, j = 0; i < (graphics.w / 8 * graphics.h); i++, j+=8)
  {
    graphics.pixels[j+0] = ((buf[i] & 0b10000000) >> 7) ? graphics.on : graphics.off;
    graphics.pixels[j+1] = ((buf[i] & 0b01000000) >> 6) ? graphics.on : graphics.off;
    graphics.pixels[j+2] = ((buf[i] & 0b00100000) >> 5) ? graphics.on : graphics.off;
    graphics.pixels[j+3] = ((buf[i] & 0b00010000) >> 4) ? graphics.on : graphics.off;
    graphics.pixels[j+4] = ((buf[i] & 0b00001000) >> 3) ? graphics.on : graphics.off;
    graphics.pixels[j+5] = ((buf[i] & 0b00000100) >> 2) ? graphics.on : graphics.off;
    graphics.pixels[j+6] = ((buf[i] & 0b00000010) >> 1) ? graphics.on : graphics.off;
    graphics.pixels[j+7] = ((buf[i] & 0b00000001) >> 0) ? graphics.on : graphics.off;
  }
  graphics_draw_pixels();  
}

void c8_host_sleep(unsigned long milliseconds)
{
  controller_update();
  SDL_Delay(milliseconds);
}

void graphics_draw_pixels()
{
  SDL_UpdateTexture(graphics.target, NULL, graphics.pixels, graphics.w);
  SDL_RenderCopy(graphics.renderer, graphics.target, NULL, NULL);
  SDL_RenderPresent(graphics.renderer);
}

int graphics_init(int width, int height, int scale)
{
  // RGB332
  graphics.on  = 0b00011100;
  graphics.off = 0b00000100;

  graphics.w = width;
  graphics.h = height;
  graphics.s = scale;
  
  if (SDL_CreateWindowAndRenderer(
    width * scale, height * scale, 0, &graphics.window, &graphics.renderer) 
    != 0) return 0;
  SDL_Delay(100);
  
  if (SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 0) != 0) return 0;
  if (SDL_RenderClear(graphics.renderer) != 0) return 0;
  SDL_RenderPresent(graphics.renderer);
  
  graphics.target = SDL_CreateTexture(graphics.renderer,
    SDL_PIXELFORMAT_RGB332,
    SDL_TEXTUREACCESS_STREAMING,
    width,
    height);
  if (graphics.target == NULL) return 0;

  graphics.pixels = malloc(width * height);
  if (graphics.pixels == NULL) return 0;

  return 1;
}

void graphics_cleanup()
{
  free(graphics.pixels);
  SDL_DestroyTexture(graphics.target);
  SDL_DestroyRenderer(graphics.renderer);
  SDL_DestroyWindow(graphics.window); 
}

void controller_update()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
    {
      controller.isQuit = 1;
      return;
    }   

    switch (event.type) {
      case SDL_KEYUP:
      {
        switch (event.key.keysym.sym) {
          case SDLK_1: controller.c8_keys &= ~C8_KEY_1; break;
          case SDLK_2: controller.c8_keys &= ~C8_KEY_2; break;
          case SDLK_3: controller.c8_keys &= ~C8_KEY_3; break;
          case SDLK_4: controller.c8_keys &= ~C8_KEY_C; break;
          case SDLK_q: controller.c8_keys &= ~C8_KEY_4; break;
          case SDLK_w: controller.c8_keys &= ~C8_KEY_5; break;
          case SDLK_e: controller.c8_keys &= ~C8_KEY_6; break;
          case SDLK_r: controller.c8_keys &= ~C8_KEY_D; break;
          case SDLK_a: controller.c8_keys &= ~C8_KEY_7; break;
          case SDLK_s: controller.c8_keys &= ~C8_KEY_8; break;
          case SDLK_d: controller.c8_keys &= ~C8_KEY_9; break;
          case SDLK_f: controller.c8_keys &= ~C8_KEY_E; break;
          case SDLK_z: controller.c8_keys &= ~C8_KEY_A; break;
          case SDLK_x: controller.c8_keys &= ~C8_KEY_0; break;
          case SDLK_c: controller.c8_keys &= ~C8_KEY_B; break;
          case SDLK_v: controller.c8_keys &= ~C8_KEY_F; break;
          case SDLK_ESCAPE: controller.isQuit = 0; break;
          case SDLK_EQUALS: controller.isSpeedUp = 0; break;
          case SDLK_MINUS: controller.isSpeedDown = 0; break;
          case SDLK_0: controller.isReset = 0; break;
          case SDLK_SPACE: controller.isPaused = 0; break;
          case SDLK_BACKSPACE: controller.isProgramReset = 0; break;
          default: break;
        }
      } break;
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym) {
          case SDLK_1: controller.c8_keys |= C8_KEY_1; break;
          case SDLK_2: controller.c8_keys |= C8_KEY_2; break;
          case SDLK_3: controller.c8_keys |= C8_KEY_3; break;
          case SDLK_4: controller.c8_keys |= C8_KEY_C; break;
          case SDLK_q: controller.c8_keys |= C8_KEY_4; break;
          case SDLK_w: controller.c8_keys |= C8_KEY_5; break;
          case SDLK_e: controller.c8_keys |= C8_KEY_6; break;
          case SDLK_r: controller.c8_keys |= C8_KEY_D; break;
          case SDLK_a: controller.c8_keys |= C8_KEY_7; break;
          case SDLK_s: controller.c8_keys |= C8_KEY_8; break;
          case SDLK_d: controller.c8_keys |= C8_KEY_9; break;
          case SDLK_f: controller.c8_keys |= C8_KEY_E; break;
          case SDLK_z: controller.c8_keys |= C8_KEY_A; break;
          case SDLK_x: controller.c8_keys |= C8_KEY_0; break;
          case SDLK_c: controller.c8_keys |= C8_KEY_B; break;
          case SDLK_v: controller.c8_keys |= C8_KEY_F; break;
          case SDLK_ESCAPE: controller.isQuit = 1; break;
          case SDLK_EQUALS: controller.isSpeedUp = 1; break;
          case SDLK_MINUS: controller.isSpeedDown = 1; break;
          case SDLK_0: controller.isReset = 1; break;
          case SDLK_SPACE: controller.isPaused = 1; break;
          case SDLK_BACKSPACE: controller.isProgramReset = 1; break;
          default: break;
        }
      } break;
      default: break;
    }
  }
}

int controller_init(){
  controller.c8_keys = 0;
  controller.isQuit = 0;
  controller.isSpeedUp = 0;
  controller.isSpeedDown = 0;
  controller.isReset = 0;
  controller.isReset = 0;
  controller.isProgramReset = 0;
  return 1;
}

void beeper_beep(int ms)
{
  SDL_PauseAudio(0);
  SDL_Delay(ms);
  SDL_PauseAudio(1);
}

int beeper_init()
{
  beeper.sample_rate = 44100;
  beeper.amplitude = 28000;

  beeper.sample_number = 0;
  beeper.desired_spec.format = AUDIO_S16;
  beeper.desired_spec.channels = 1;
  beeper.desired_spec.samples = 2048;
  beeper.desired_spec.freq = beeper.sample_rate;
  beeper.desired_spec.userdata = &beeper.sample_number;
  beeper.desired_spec.callback = beeper_callback;
  
  if (SDL_OpenAudio(&beeper.desired_spec, &beeper.obtained_spec) != 0) return 0;
  if (beeper.desired_spec.format != beeper.obtained_spec.format) return 0;
  return 1;
}

void beeper_cleanup()
{
  SDL_CloseAudio();
}

void beeper_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
  // AUDIO_S16SYS requies 2 BPS
  Sint16 *buf16 = (Sint16*)raw_buffer;
  int buf16_length = bytes / 2; 

  int *sample_number = (int*)user_data;
  
  // 441 HZ sine wive
  for (int i = 0; i < buf16_length; i++, (*sample_number)++)
  {
    double time = (double)(*sample_number) / (double)beeper.sample_rate;
    buf16[i] = (Sint16)(beeper.amplitude * sin(2.0f * M_PI * beeper.sample_rate / 100.0f * time));
  }
}

#endif
