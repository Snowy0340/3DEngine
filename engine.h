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

/* Главный цикл: открывает окно, для каждого столбца экрана пускает луч
   из камеры, находит ближайшую непустую клетку карты и рисует вертикальную
   линию высотой C / dist. Работает до закрытия окна.
   Перед вызовом нужно вызвать set_map() и set_camera().
   Возвращает 1 при нормальном завершении, 0 при ошибке (нет карты или окна). */
int engine_run(void);

/* Пересечение луча (начало o, направление ray) с клеткой (cx, cy),
   клетка - квадрат [cx, cx+1] x [cy, cy+1]. Возвращает 1 и ближайшую
   к началу луча точку пересечения hit, если луч пересекает границу клетки. */
int ray_cell_intersect(const vec2_t o, const vec2_t ray, int cx, int cy, vec2_t hit);

#endif
