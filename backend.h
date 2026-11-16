#ifndef SDL_WRAPPER_H
#define SDL_WRAPPER_H

int create_window(int width, int height);

//освобождение ресурсов SDL
void destroy_window(void);

void update_window(void);

//нарисовать вертикальную линию в столбце position,
//height - высота линии, color - цвет 0xRRGGBB (например 0xff0000 - красный)
void draw_vline(int height, int position, int color);

int window_closed();

//1, если клавиша key сейчас нажата, иначе 0 (регистр не важен)
//состояние обновляется в update_window()
int key_pressed(char key);

#endif
