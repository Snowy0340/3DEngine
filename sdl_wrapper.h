#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

#include <SDL.h>

int create_window(void);
void destroy_window(void);
void draw_line(int height, int position);
// void draw_lines(int height);
void update_frame(void);
// bool sdl_process_events(void);

#endif
