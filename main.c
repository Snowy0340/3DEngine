#include "sdl_wrapper.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (!sdl_init("My 3D Engine"))
        return 1;

    // Главный цикл программы
    while (!is_quit)
    {
        int position = 50;
        draw_vline(LINE_HEIGHT, position);
        position+=INDENT;
        
        
        
        window_update()
    }
    // Освобождаем ресурсы SDL
    destroy_window();

    return 0;
}
