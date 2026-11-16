#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>

//Указатель на окно
static SDL_Window* window = NULL;

//Указатель на рендерер
static SDL_Renderer* renderer = NULL;

static SDL_Event event;
static int is_quit = 0;

//Таблица клавиш: индекс - код клавиши, значение - 1 (нажата) или 0 (нет)
static int keys[256];

//запомнить состояние клавиши (коды вне таблицы, например стрелки, пропускаем)
static void set_key(SDL_Keycode sym, int pressed)
{
    if (sym >= 0 && sym < 256)
        keys[sym] = pressed;
}

int window_height;

//инициализация sDL и соллздание окна
int create_window(int width, int height)
{
    char *title = "title";
    
    //инициализируем видеоподсистему SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL init error: %s\n", SDL_GetError());
        return 0;
    }
    window_height = height;

    //создаем окно
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    return 1;
}

//освобождение ресурсов SDL
void destroy_window(void)
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

void update_window(void)
{
    SDL_RenderPresent(renderer);

    //очищаем экран перед следующим кадром
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //обрабатываем события окна и клавиатуры
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            is_quit = 1;
        else if (event.type == SDL_KEYDOWN)
            set_key(event.key.keysym.sym, 1);
        else if (event.type == SDL_KEYUP)
            set_key(event.key.keysym.sym, 0);
        else if (event.type == SDL_WINDOWEVENT &&
                 event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
            memset(keys, 0, sizeof(keys));   //окно потеряло фокус - отпусканий не увидим
    }

    //ограничиваем частоту кадров (около 60 в секунду)
    SDL_Delay(16);
}

//color - цвет 0xRRGGBB (например 0xff0000 - красный)
void draw_vline(int height, int position, int color)
{
    /*if (height > SCREEN_HEIGHT)
	{
	    printf("Error! height bigger then screen!");
	    return;
	    }*/
    
    SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, 255);
    
    int ypos = (window_height - height) / 2;

    SDL_RenderDrawLine(renderer, position, ypos, position, ypos+height);
}

int window_closed()
{
    return is_quit;
}

//1, если клавиша сейчас нажата (нажата и еще не отпущена), иначе 0
//регистр не важен: 'A' и 'a' - одна клавиша
//состояние обновляется в update_window()
int key_pressed(char key)
{
    unsigned char k = (unsigned char)tolower((unsigned char)key);

    return keys[k];
}
