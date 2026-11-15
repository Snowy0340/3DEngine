#include <math.h>

#include "vec2.h"

#define INTERSECT_EPS 1e-6f

int intersect_h(const vec2_t o, const vec2_t ray, vec_t y0, vec2_t hit)
{
    vec_t distance;

    if (fabsf(ray[1]) < INTERSECT_EPS)
        return 0;

    distance = (y0 - o[1]) / ray[1];
    if (distance < 0.0f)
        return 0;

    vec2_create(hit, o[0] + distance * ray[0], y0);
    return 1;
}

int intersect_v(const vec2_t o, const vec2_t ray, vec_t x0, vec2_t hit)
{
    vec_t distance;

    if (fabsf(ray[0]) < INTERSECT_EPS)
        return 0;

    distance = (x0 - o[0]) / ray[0];
    if (distance < 0.0f)
        return 0;

    vec2_create(hit, x0, o[1] + distance * ray[1]);
    return 1;
}
