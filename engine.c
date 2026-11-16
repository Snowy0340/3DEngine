#include <math.h>
#include <stddef.h>

#include "backend.h"
#include "engine.h"
#include "intersect.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

/* Угол обзора камеры (60 градусов) */
#define FOV 1.0471976f

/* Константа C: высота линии = C / dist */
#define LINE_CONST 1000.0f

/* Допуск при проверке, что точка лежит на стороне клетки */
#define CELL_EPS 1e-4f

/* Цвета клеток карты (0xRRGGBB): индекс - значение клетки */
static const int cell_colors[] = {0x000000, 0xd04040, 0x4060d0};
#define CELL_COLORS_COUNT ((int)(sizeof(cell_colors) / sizeof(cell_colors[0])))

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

int ray_cell_intersect(const vec2_t o, const vec2_t ray, int cx, int cy, vec2_t hit)
{
    vec2_t p;
    vec2_t d;
    vec_t best = -1.0f;
    vec_t dist;
    int found = 0;
    int i;

    /* две горизонтальные стороны: y = cy и y = cy + 1, x в [cx, cx+1] */
    for (i = 0; i < 2; i++) {
        if (intersect_h(o, ray, (vec_t)(cy + i), p) &&
            p[0] >= cx - CELL_EPS && p[0] <= cx + 1 + CELL_EPS) {
            vec2_create(d, p[0] - o[0], p[1] - o[1]);
            dist = vec2_length(d);
            if (!found || dist < best) {
                best = dist;
                vec2_create(hit, p[0], p[1]);
                found = 1;
            }
        }
    }

    /* две вертикальные стороны: x = cx и x = cx + 1, y в [cy, cy+1] */
    for (i = 0; i < 2; i++) {
        if (intersect_v(o, ray, (vec_t)(cx + i), p) &&
            p[1] >= cy - CELL_EPS && p[1] <= cy + 1 + CELL_EPS) {
            vec2_create(d, p[0] - o[0], p[1] - o[1]);
            dist = vec2_length(d);
            if (!found || dist < best) {
                best = dist;
                vec2_create(hit, p[0], p[1]);
                found = 1;
            }
        }
    }

    return found;
}

int engine_run(void)
{
    vec2_t plane;
    vec2_t ray;
    vec2_t hit;
    vec2_t d;
    vec_t k;
    vec_t dist;
    vec_t best_dist;
    int best_cell;
    int height;
    int cell;
    int x, cx, cy;

    if (map == NULL || map_width <= 0 || map_height <= 0)
        return 0;

    if (!create_window(SCREEN_WIDTH, SCREEN_HEIGHT))
        return 0;

    /* плоскость камеры: перпендикуляр к направлению взгляда,
       длиной tan(FOV/2); луч = view_vec + plane * k, k от -1 до 1 */
    vec2_create(plane, -view_vec[1], view_vec[0]);
    vec2_scale(plane, plane, tanf(FOV / 2.0f));

    while (!window_closed()) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            /* луч из камеры для столбца x */
            k = 2.0f * x / SCREEN_WIDTH - 1.0f;
            vec2_scale(d, plane, k);
            vec2_add(ray, view_vec, d);

            best_dist = -1.0f;
            best_cell = 0;

            /* перебираем все клетки, отличные от 0, берём ближайшую */
            for (cy = 0; cy < map_height; cy++) {
                for (cx = 0; cx < map_width; cx++) {
                    cell = map[cy][cx];
                    if (cell == 0)
                        continue;

                    if (ray_cell_intersect(origin, ray, cx, cy, hit)) {
                        vec2_create(d, hit[0] - origin[0], hit[1] - origin[1]);
                        dist = vec2_length(d);
                        if (dist > 0.0f && (best_dist < 0.0f || dist < best_dist)) {
                            best_dist = dist;
                            best_cell = cell;
                        }
                    }
                }
            }

            if (best_dist > 0.0f) {
                height = (int)(LINE_CONST / best_dist);
                if (height > SCREEN_HEIGHT)
                    height = SCREEN_HEIGHT;
                if (best_cell < 0 || best_cell >= CELL_COLORS_COUNT)
                    best_cell = CELL_COLORS_COUNT - 1;
                draw_vline(height, x, cell_colors[best_cell]);
            }
        }
        update_window();
    }

    destroy_window();
    return 1;
}
