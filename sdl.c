#include "sdl_wrapper.h"
#include <stdio.h>
#include "config.h"

//Указатель на окно
static SDL_Window* window = NULL;

//Указатель на рендерер
static SDL_Renderer* renderer = NULL;

static SDL_Event event;
static bool is_quit = false;

//инициализация sDL и соллздание окна
int sdl_init(const char* title)
{
    //инициализируем видеоподсистему SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL init error: %s\n", SDL_GetError());
        return 0;
    }

    //создаем окно
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    //проверяем создание окна
    if (window == NULL)
    {
        printf("Window creation error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL)
    {
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_SOFTWARE
        );
    }

    //проверяем создание рендерера
    if (renderer == NULL)
    {
        printf("renderer creation error: %s\n", SDL_GetError());

        SDL_DestroyWindow(window);
        window = NULL;

        SDL_Quit();
        return 0;
    }

    return 1;
}

//освобождение ресурсов SDL
void sdl_shutdown(void)
{
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }


    SDL_Quit();
}

void update_frame(void)
{
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_lines(LINE_HEIGHT);

        SDL_RenderPresent(renderer);
}

void draw_line(int height, int position)
{
    if (height > SCREEN_HEIGHT)
    {
        printf("Error! height bigger then screen!");
        return;
    }
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    int ypos = (SCREEN_HEIGHT - height) / 2;

    SDL_RenderDrawLine(renderer, position, ypos, position, ypos+height);
}

void draw_lines(int height)
{
    int position = 50;
    while(position < SCREEN_WIDTH)
    {
        draw_line(height, position);
        position += INDENT;
    }
}

bool sdl_process_events(void)
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            is_quit = true;
    }

    return is_quit;
}