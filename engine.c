#include <math.h>
#include <stddef.h>

#include "engine.h"

int map_width = 0;
int map_height = 0;
int **map = NULL;
vec2_t origin = {0.0f, 0.0f};
vec2_t view_vec = {1.0f, 0.0f};

void set_map(int w, int h, int **arr)
{
    map_width = w;
    map_height = h;
    map = arr;
}

void set_camera(vec_t x, vec_t y, vec_t angle)
{
    origin[0] = x;
    origin[1] = y;

    view_vec[0] = cosf(angle);
    view_vec[1] = sinf(angle);
}
