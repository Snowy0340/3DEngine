#include <assert.h>
#include <math.h>
#include <stddef.h>

#include "backend.h"
#include "engine.h"
#include "intersect.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

/* Угол обзора камеры (60 градусов) */
#define FOV 1.0471976f

/* Константа C: высота линии = C / dist */
#define LINE_CONST 1000.0f

/* Допуск при проверке, что точка лежит на стороне клетки */
#define CELL_EPS 1e-4f

/* Цвета клеток карты (0xRRGGBB): индекс - значение клетки */
static const int cell_colors[] = {0x000000, 0xd04040, 0x4060d0};
#define CELL_COLORS_COUNT ((int)(sizeof(cell_colors) / sizeof(cell_colors[0])))

int map_width = 0;
int map_height = 0;
int **map = NULL;
vec2_t origin = {0.0f, 0.0f};
vec2_t view_vec = {1.0f, 0.0f};

void set_map(int w, int h, int **arr)
{
    map_width = w;
    map_height = h;
    map = arr;
}

void set_camera(vec_t x, vec_t y, vec_t angle)
{
    origin[0] = x;
    origin[1] = y;

    view_vec[0] = cosf(angle);
    view_vec[1] = sinf(angle);
}

/*
 * ray_cell_intersect - пересечение луча с одной клеткой карты.
 *
 * Клетка (cx, cy) - квадрат [cx, cx+1] x [cy, cy+1] (размер клетки 1x1,
 * координаты камеры и клеток в одних и тех же единицах).
 *
 * Параметры:
 *   o    - начало луча (позиция камеры), не NULL
 *   ray  - направление луча (не обязательно единичной длины, но не нулевое)
 *   cx   - номер клетки по X (левая граница клетки)
 *   cy   - номер клетки по Y (верхняя граница клетки)
 *   hit  - сюда записывается ближайшая к o точка пересечения, не NULL;
 *          если функция вернула 0, hit не изменяется
 *
 * Возвращает 1, если луч пересекает границу клетки впереди по лучу,
 * иначе 0.
 *
 * Как работает: луч может войти в квадрат только через одну из четырёх
 * сторон. Для каждой стороны берём готовое пересечение луча с ПРЯМОЙ
 * (intersect_h / intersect_v) и проверяем, что точка лежит именно на
 * стороне клетки, а не на продолжении прямой. Из подходящих точек выбираем
 * ближайшую к началу луча - это точка, где луч входит в клетку.
 *
 * Допуск CELL_EPS нужен, чтобы луч, попадающий ровно в угол клетки, не
 * проскакивал между двумя соседними сторонами из-за ошибки округления float.
 */
int ray_cell_intersect(const vec2_t o, const vec2_t ray, int cx, int cy, vec2_t hit)
{
    vec2_t p;           /* точка пересечения луча с очередной прямой */
    vec2_t d;           /* вектор от начала луча до точки p */
    vec_t best = -1.0f; /* расстояние до ближайшей найденной точки */
    vec_t dist;
    int found = 0;      /* найдено ли хоть одно пересечение */
    int i;

    /* Входные данные: указатели не NULL, луч имеет направление.
       (массивы vec2_t передаются как указатели) */
    assert(o != NULL);
    assert(ray != NULL);
    assert(hit != NULL);
    assert(ray[0] != 0.0f || ray[1] != 0.0f);

    /* две горизонтальные стороны: y = cy и y = cy + 1, x должен быть в [cx, cx+1] */
    for (i = 0; i < 2; i++) {
        if (intersect_h(o, ray, (vec_t)(cy + i), p) &&
            p[0] >= cx - CELL_EPS && p[0] <= cx + 1 + CELL_EPS) {
            vec2_create(d, p[0] - o[0], p[1] - o[1]);
            dist = vec2_length(d);
            if (!found || dist < best) {
                best = dist;
                vec2_create(hit, p[0], p[1]);
                found = 1;
            }
        }
    }

    /* две вертикальные стороны: x = cx и x = cx + 1, y должен быть в [cy, cy+1] */
    for (i = 0; i < 2; i++) {
        if (intersect_v(o, ray, (vec_t)(cx + i), p) &&
            p[1] >= cy - CELL_EPS && p[1] <= cy + 1 + CELL_EPS) {
            vec2_create(d, p[0] - o[0], p[1] - o[1]);
            dist = vec2_length(d);
            if (!found || dist < best) {
                best = dist;
                vec2_create(hit, p[0], p[1]);
                found = 1;
            }
        }
    }

    /* Если пересечение найдено, расстояние не может быть отрицательным */
    assert(!found || best >= 0.0f);

    return found;
}

/*
 * engine_run - главный цикл движка (рисование вида от первого лица).
 *
 * Перед вызовом должны быть вызваны set_map() и set_camera().
 *
 * Что делает за один кадр:
 *   1. Для каждого столбца экрана x (0 .. SCREEN_WIDTH-1) строит луч из
 *      камеры. Луч = view_vec + plane * k, где plane - перпендикуляр к
 *      направлению взгляда длиной tan(FOV/2), а k = от -1 (левый край
 *      экрана) до +1 (правый край). Так лучи равномерно покрывают угол
 *      обзора FOV.
 *   2. Перебирает все клетки карты, отличные от 0, ищет пересечение луча
 *      с клеткой (ray_cell_intersect) и выбирает ближайшую клетку.
 *   3. Считает dist - длину вектора от камеры до точки пересечения, и
 *      высоту линии как LINE_CONST / dist (чем ближе стена, тем выше).
 *      Высота ограничивается высотой экрана.
 *   4. Рисует вертикальную линию в столбце x цветом клетки (1 или 2).
 *   После всех столбцов update_window() выводит кадр и обрабатывает
 *   события (закрытие окна, клавиши).
 *
 * Цикл работает, пока окно не закрыто; потом окно уничтожается.
 *
 * Возвращает 1 при нормальном завершении; 0, если карта не задана или
 * не удалось создать окно (это ошибки использования - они проверяются
 * обычным if, а не assert, чтобы работать и при -DNDEBUG).
 *
 * Assert'ами проверяются внутренние инварианты: корректность строк карты,
 * значения клеток, длина луча и расстояния.
 */
int engine_run(void)
{
    vec2_t plane;     /* вектор плоскости камеры */
    vec2_t ray;       /* луч для текущего столбца */
    vec2_t hit;       /* точка пересечения луча с клеткой */
    vec2_t d;         /* вспомогательный вектор */
    vec_t k;          /* положение столбца на экране: от -1 до +1 */
    vec_t dist;       /* расстояние от камеры до точки пересечения */
    vec_t best_dist;  /* расстояние до ближайшей стены (-1 - стены нет) */
    int best_cell;    /* значение ближайшей клетки (определяет цвет) */
    int height;       /* высота линии в пикселях */
    int cell;
    int x, cx, cy;

    /* Ошибки использования: карта не задана */
    if (map == NULL || map_width <= 0 || map_height <= 0)
        return 0;

    if (!create_window(SCREEN_WIDTH, SCREEN_HEIGHT))
        return 0;

    /* Внутренние инварианты: направление взгляда ненулевое
       (set_camera задаёт единичный вектор), FOV в допустимых пределах.
       Положение камеры не проверяется: она может быть и за пределами карты. */
    assert(view_vec[0] != 0.0f || view_vec[1] != 0.0f);
    assert(FOV > 0.0f && FOV < 3.14159f);
    assert(SCREEN_WIDTH > 0 && SCREEN_HEIGHT > 0);
    assert(LINE_CONST > 0.0f);

    /* плоскость камеры считаем один раз: направление взгляда не меняется
       внутри цикла (если камеру начнут двигать - перенести внутрь while) */
    vec2_create(plane, -view_vec[1], view_vec[0]);
    vec2_scale(plane, plane, tanf(FOV / 2.0f));

    while (!window_closed()) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            /* луч из камеры для столбца x */
            k = 2.0f * x / SCREEN_WIDTH - 1.0f;
            assert(k >= -1.0f && k < 1.0f);
            vec2_scale(d, plane, k);
            vec2_add(ray, view_vec, d);

            best_dist = -1.0f;
            best_cell = 0;

            /* перебираем все клетки, отличные от 0, берём ближайшую */
            for (cy = 0; cy < map_height; cy++) {
                assert(map[cy] != NULL);   /* строка карты должна существовать */
                for (cx = 0; cx < map_width; cx++) {
                    cell = map[cy][cx];
                    assert(cell >= 0);     /* значения клеток: 0, 1, 2, ... */
                    if (cell == 0)
                        continue;

                    if (ray_cell_intersect(origin, ray, cx, cy, hit)) {
                        vec2_create(d, hit[0] - origin[0], hit[1] - origin[1]);
                        dist = vec2_length(d);
                        assert(dist >= 0.0f);
                        if (dist > 0.0f && (best_dist < 0.0f || dist < best_dist)) {
                            best_dist = dist;
                            best_cell = cell;
                        }
                    }
                }
            }

            if (best_dist > 0.0f) {
                height = (int)(LINE_CONST / best_dist);
                if (height > SCREEN_HEIGHT)
                    height = SCREEN_HEIGHT;
                assert(height >= 0 && height <= SCREEN_HEIGHT);
                /* значение клетки вне таблицы цветов - берём последний цвет */
                if (best_cell >= CELL_COLORS_COUNT)
                    best_cell = CELL_COLORS_COUNT - 1;
                assert(best_cell > 0 && best_cell < CELL_COLORS_COUNT);
                draw_vline(height, x, cell_colors[best_cell]);
            }
        }
        update_window();
    }

    destroy_window();
    return 1;
}
