#ifndef ENGINE_H
#define ENGINE_H

#include "vec2.h"

extern int map_width;
extern int map_height;
extern int **map;
extern vec2_t origin;
extern vec2_t view_vec;

void set_map(int w, int h, int **arr);
void set_camera(vec_t x, vec_t y, vec_t angle);

#endif
