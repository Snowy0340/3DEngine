#include <math.h>
#include <stdio.h>

#include "vec2.h"

#define VEC2_EPS 1e-8f

void vec2_create(vec2_t r, vec_t x, vec_t y)
{
    r[0] = x;
    r[1] = y;
}

void vec2_add(vec2_t r, const vec2_t a, const vec2_t b)
{
    int i;

    for (i = 0; i < VEC2_SIZE; i++)
        r[i] = a[i] + b[i];
}

void vec2_scale(vec2_t r, const vec2_t a, vec_t s)
{
    int i;

    for (i = 0; i < VEC2_SIZE; i++)
        r[i] = a[i] * s;
}

vec_t vec2_dot(const vec2_t a, const vec2_t b)
{
    vec_t sum = 0.0f;
    int i;

    for (i = 0; i < VEC2_SIZE; i++)
        sum += a[i] * b[i];

    return sum;
}

vec_t vec2_length(const vec2_t a)
{
    return sqrtf(vec2_dot(a, a));
}

void vec2_normalize(vec2_t r, const vec2_t a)
{
    vec2_t scaled;
    vec_t max_component = 0.0f;
    vec_t length;
    int i;

    for (i = 0; i < VEC2_SIZE; i++) {
        vec_t component = fabsf(a[i]);

        if (component > max_component)
            max_component = component;
    }

    if (max_component < VEC2_EPS) {
        vec2_create(r, 0.0f, 0.0f);
        return;
    }

    for (i = 0; i < VEC2_SIZE; i++)
        scaled[i] = a[i] / max_component;

    length = vec2_length(scaled);
    for (i = 0; i < VEC2_SIZE; i++)
        r[i] = scaled[i] / length;
}

void vec2_print(const vec2_t a)
{
    printf("(%g, %g)", a[0], a[1]);
}
