#ifndef INTERSECT_H
#define INTERSECT_H

#include "vec2.h"

/* Пересечение луча (начало o, направление ray) с горизонтальной прямой y = y0.
   Возвращает 1 и точку hit, если пересечение впереди по лучу, иначе 0. */
int intersect_h(const vec2_t o, const vec2_t ray, vec_t y0, vec2_t hit);

/* То же для вертикальной прямой x = x0. */
int intersect_v(const vec2_t o, const vec2_t ray, vec_t x0, vec2_t hit);

#endif
