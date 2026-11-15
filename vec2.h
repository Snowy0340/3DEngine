#ifndef VEC2_H
#define VEC2_H

#define VEC2_SIZE 2

typedef float vec_t;
typedef vec_t vec2_t[VEC2_SIZE];

void vec2_create(vec2_t r, vec_t x, vec_t y);
void vec2_add(vec2_t r, const vec2_t a, const vec2_t b);
void vec2_scale(vec2_t r, const vec2_t a, vec_t s);
vec_t vec2_dot(const vec2_t a, const vec2_t b);
vec_t vec2_length(const vec2_t a);
void vec2_normalize(vec2_t r, const vec2_t a);
void vec2_print(const vec2_t a);

#endif
