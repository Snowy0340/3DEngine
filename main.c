#include <stdio.h>

#include "engine.h"

#define WIDTH 10
#define HEIGHT 10

int main(void)
{
    int level[HEIGHT][WIDTH] = {0};
    int *rows[HEIGHT];
    int x;
    int y;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 ||
                x == 0 || x == WIDTH - 1) {
                level[y][x] = 1;
            }
        }
    }

    level[3][3] = 2;
    level[3][4] = 2;
    level[4][3] = 2;
    level[6][7] = 2;

    for (y = 0; y < HEIGHT; y++)
        rows[y] = level[y];

    set_map(WIDTH, HEIGHT, rows);
    set_camera(5.0f, 5.0f, 0.0f);

    printf("Generated map:\n");
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++)
            printf("%d ", level[y][x]);
        printf("\n");
    }

    return 0;
}
