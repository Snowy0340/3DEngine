#include "backend.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define INDENT 50
#define LINE_HEIGHT 500

/*
 * Ручной тест backend.
 * W / S - сделать линии выше / ниже
 * Пробел - все линии белые (иначе три полосы: красная, зеленая, синяя)
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    int height = LINE_HEIGHT;

    if (!create_window(SCREEN_WIDTH, SCREEN_HEIGHT))
        return 1;

    // Главный цикл программы
    while (!window_closed())
    {
        // клавиатура
        if (key_pressed('w') && height < SCREEN_HEIGHT)
            height += 4;
        if (key_pressed('s') && height > 0)
            height -= 4;

        for (int x = 0; x<SCREEN_WIDTH; x++)
        {
            int color;

            if (key_pressed(' '))
                color = 0xffffff;
            else if (x < SCREEN_WIDTH / 3)
                color = 0xff0000;
            else if (x < 2 * SCREEN_WIDTH / 3)
                color = 0x00ff00;
            else
                color = 0x0000ff;

            draw_vline(height, x, color);
        }
        update_window();
    }
    // Освобождаем ресурсы SDL
    destroy_window();

    return 0;
}
