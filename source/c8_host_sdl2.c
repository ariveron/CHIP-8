#include "c8.h"

#include <SDL.h>
#include <stdlib.h>

SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;
SDL_Texture *buffer = NULL;
c8_byte *pixels = NULL;
int pitch = 0;
int w, h, s;
SDL_Event event;

c8_word keys = 0;
int isQuit = 0;
int isSpeedUp = 0;
int isSpeedDown = 0;
int isReset = 0;
int isPaused = 0;
int isProgramReset = 0;
void get_events();

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 44100;
int sample_nr = 0;
SDL_AudioSpec want;
SDL_AudioSpec have;
void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes);

int c8_host_init(const wchar_t* title, int width, int height, int scale)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_CreateWindowAndRenderer(width * scale, height * scale, 0, &window, &renderer);
  SDL_Delay(100);
  
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  buffer = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB332,
    SDL_TEXTUREACCESS_STREAMING,
    width,
    height);

  pixels = malloc(width * height);
  pitch = width;

  w = width; h = height; s = scale;
  
  want.freq = SAMPLE_RATE; // number of samples per second
  want.format = AUDIO_S16SYS; // sample type (here: signed short i.e. 16 bit)
  want.channels = 1; // only one channel
  want.samples = 2048; // buffer-size
  want.callback = audio_callback; // function SDL calls periodically to refill the buffer
  want.userdata = &sample_nr; // counter, keeping track of current sample number

  if(SDL_OpenAudio(&want, &have) != 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
  if(want.format != have.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");

  return 1;
}

void c8_host_cleanup(void)
{
  SDL_CloseAudio();
  free(pixels);
  SDL_DestroyTexture(buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

uint64_t c8_host_get_100nanoseconds(void)
{
  get_events();
  return SDL_GetTicks() * 10000;
}

c8_word c8_host_get_keys(void)
{
  get_events();
  return keys;
}

void c8_host_make_sound(void)
{
  SDL_PauseAudio(0); // start playing sound
  SDL_Delay(20); // wait while sound is playing
  SDL_PauseAudio(1); // stop playing sound
}

int c8_host_is_not_quit(void)
{
  get_events();
  return !isQuit;
}

int c8_host_is_speed_up(void)
{
  get_events();
  return isSpeedUp;
}

int c8_host_is_speed_down(void)
{
  get_events();
  return isSpeedDown;
}

int c8_host_is_speed_reset(void)
{
  get_events();
  return isReset;
}

int c8_host_is_paused(void)
{
  get_events();
  return isPaused;
}

int c8_host_is_program_reset(void)
{
  get_events();
  return isProgramReset;
}

void c8_host_render(c8_byte* buf)
{
  get_events();

  // RGB332
  c8_byte on  = 0b00011100;
  c8_byte off = 0b00000100;

  for (int i = 0, j = 0; i < (w / 8 * h); i++, j+=8)
  {
    pixels[j+0] = ((buf[i] & 0b10000000) >> 7) ? on : off;
    pixels[j+1] = ((buf[i] & 0b01000000) >> 6) ? on : off;
    pixels[j+2] = ((buf[i] & 0b00100000) >> 5) ? on : off;
    pixels[j+3] = ((buf[i] & 0b00010000) >> 4) ? on : off;
    pixels[j+4] = ((buf[i] & 0b00001000) >> 3) ? on : off;
    pixels[j+5] = ((buf[i] & 0b00000100) >> 2) ? on : off;
    pixels[j+6] = ((buf[i] & 0b00000010) >> 1) ? on : off;
    pixels[j+7] = ((buf[i] & 0b00000001) >> 0) ? on : off;
  }

  SDL_UpdateTexture(buffer, NULL, pixels, pitch);
  SDL_RenderCopy(renderer, buffer, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void c8_host_sleep(unsigned long milliseconds)
{
  get_events();
  SDL_Delay(milliseconds);
}

void get_events()
{
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
    {
      isQuit = 1;
      return;
    }   

    switch (event.type) {
      case SDL_KEYUP:
      {
        switch (event.key.keysym.sym) {
          case SDLK_1: keys &= ~C8_KEY_1; break;
          case SDLK_2: keys &= ~C8_KEY_2; break;
          case SDLK_3: keys &= ~C8_KEY_3; break;
          case SDLK_4: keys &= ~C8_KEY_C; break;
          case SDLK_q: keys &= ~C8_KEY_4; break;
          case SDLK_w: keys &= ~C8_KEY_5; break;
          case SDLK_e: keys &= ~C8_KEY_6; break;
          case SDLK_r: keys &= ~C8_KEY_D; break;
          case SDLK_a: keys &= ~C8_KEY_7; break;
          case SDLK_s: keys &= ~C8_KEY_8; break;
          case SDLK_d: keys &= ~C8_KEY_9; break;
          case SDLK_f: keys &= ~C8_KEY_E; break;
          case SDLK_z: keys &= ~C8_KEY_A; break;
          case SDLK_x: keys &= ~C8_KEY_0; break;
          case SDLK_c: keys &= ~C8_KEY_B; break;
          case SDLK_v: keys &= ~C8_KEY_F; break;
          case SDLK_ESCAPE: isQuit = 0; break;
          case SDLK_PLUS: isSpeedUp = 0; break;
          case SDLK_MINUS: isSpeedDown = 0; break;
          case SDLK_0: isReset = 0; break;
          case SDLK_SPACE: isPaused = 0; break;
          case SDLK_BACKSPACE: isProgramReset = 0; break;
          default: break;
        }
      } break;
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym) {
          case SDLK_1: keys |= C8_KEY_1; break;
          case SDLK_2: keys |= C8_KEY_2; break;
          case SDLK_3: keys |= C8_KEY_3; break;
          case SDLK_4: keys |= C8_KEY_C; break;
          case SDLK_q: keys |= C8_KEY_4; break;
          case SDLK_w: keys |= C8_KEY_5; break;
          case SDLK_e: keys |= C8_KEY_6; break;
          case SDLK_r: keys |= C8_KEY_D; break;
          case SDLK_a: keys |= C8_KEY_7; break;
          case SDLK_s: keys |= C8_KEY_8; break;
          case SDLK_d: keys |= C8_KEY_9; break;
          case SDLK_f: keys |= C8_KEY_E; break;
          case SDLK_z: keys |= C8_KEY_A; break;
          case SDLK_x: keys |= C8_KEY_0; break;
          case SDLK_c: keys |= C8_KEY_B; break;
          case SDLK_v: keys |= C8_KEY_F; break;
          case SDLK_ESCAPE: isQuit = 1; break;
          case SDLK_PLUS: isSpeedUp = 1; break;
          case SDLK_MINUS: isSpeedDown = 1; break;
          case SDLK_0: isReset = 1; break;
          case SDLK_SPACE: isPaused = 1; break;
          case SDLK_BACKSPACE: isProgramReset = 1; break;
          default: break;
        }
      } break;
      default: break;
    }
  }
}

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int *sample_nr = (int*)user_data;

    for(int i = 0; i < length; i++, (*sample_nr)++)
    {
        double time = (double)(*sample_nr) / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}
