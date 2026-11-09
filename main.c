#include "sdl_wrapper.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (!sdl_init("My 3D Engine"))
        return 1;

    // Главный цикл программы
    while (!sdl_process_events())
    {
        update_frame();
    }
    // Освобождаем ресурсы SDL
    sdl_shutdown();

    return 0;
}
