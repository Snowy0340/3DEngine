#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "backend.h"
#include "engine.h"
#include "vec2.h"


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define INDENT 50
#define LINE_HEIGHT 500

/* ---------- Тесты функций engine ---------- */

#define TEST_EPS 1e-4f

/* Равны ли два числа с допуском TEST_EPS */
static int near_eq(vec_t a, vec_t b)
{
    return fabsf(a - b) < TEST_EPS;
}

/* Вызывает ray_cell_intersect для луча из (ox, oy) в направлении (dx, dy)
   и клетки (cx, cy). Если пересечение есть, записывает точку в hx, hy.
   Возвращает результат ray_cell_intersect. */
static int cast(vec_t ox, vec_t oy, vec_t dx, vec_t dy, int cx, int cy,
                vec_t *hx, vec_t *hy)
{
    vec2_t o, ray, hit;
    int r;

    vec2_create(o, ox, oy);
    vec2_create(ray, dx, dy);
    vec2_create(hit, -1000.0f, -1000.0f);   /* метка "не изменялось" */

    r = ray_cell_intersect(o, ray, cx, cy, hit);
    *hx = hit[0];
    *hy = hit[1];
    return r;
}

/*
 * Тест ray_cell_intersect.
 * Во всех проверках клетка (2, 3), то есть квадрат [2,3] x [3,4].
 */
static void test_ray_cell_intersect(void)
{
    vec_t hx, hy;

    /* 1. Луч слева направо попадает в левую сторону (x = 2).
          Правая сторона (x = 3) дальше - должна быть выбрана ближайшая. */
    assert(cast(0.0f, 3.5f, 1.0f, 0.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 2.0f) && near_eq(hy, 3.5f));

    /* 2. Луч справа налево - правая сторона (x = 3) */
    assert(cast(5.0f, 3.5f, -1.0f, 0.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 3.0f) && near_eq(hy, 3.5f));

    /* 3. Луч сверху вниз (y растёт) - верхняя сторона (y = 3) */
    assert(cast(2.5f, 0.0f, 0.0f, 1.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 2.5f) && near_eq(hy, 3.0f));

    /* 4. Луч снизу вверх (y убывает) - нижняя сторона (y = 4) */
    assert(cast(2.5f, 10.0f, 0.0f, -1.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 2.5f) && near_eq(hy, 4.0f));

    /* 5. Направление не обязательно единичное: тот же результат,
          что и в пункте 1, при длине луча 2 */
    assert(cast(0.0f, 3.5f, 2.0f, 0.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 2.0f) && near_eq(hy, 3.5f));

    /* 6. Наклонный луч, попадает в верхнюю сторону:
          из (2.5, 0) с направлением (0.1, 1) в y = 3 будет x = 2.8 */
    assert(cast(2.5f, 0.0f, 0.1f, 1.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 2.8f) && near_eq(hy, 3.0f));

    /* 7. Луч ровно в угол клетки (2, 3) не должен проскочить
          (проверка допуска CELL_EPS) */
    assert(cast(0.0f, 1.0f, 1.0f, 1.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 2.0f) && near_eq(hy, 3.0f));

    /* 8. Луч параллелен оси и проходит мимо клетки (y = 0, а клетка на y = 3..4) */
    assert(cast(0.0f, 0.0f, 1.0f, 0.0f, 2, 3, &hx, &hy) == 0);
    assert(near_eq(hx, -1000.0f) && near_eq(hy, -1000.0f));   /* hit не изменён */

    /* 9. Клетка позади камеры: луч идёт от неё */
    assert(cast(5.0f, 3.5f, 1.0f, 0.0f, 2, 3, &hx, &hy) == 0);
    assert(near_eq(hx, -1000.0f) && near_eq(hy, -1000.0f));

    /* 10. Наклонный луч проходит мимо клетки (при x = 2..3 он на y = 1..1.5) */
    assert(cast(0.0f, 0.0f, 1.0f, 0.5f, 2, 3, &hx, &hy) == 0);

    /* 11. Камера внутри клетки: возвращается сторона, через которую
           луч выходит (впереди по лучу), а не сторона позади */
    assert(cast(2.5f, 3.5f, 1.0f, 0.0f, 2, 3, &hx, &hy) == 1);
    assert(near_eq(hx, 3.0f) && near_eq(hy, 3.5f));

    /* 12. Клетка с нулевыми номерами (граница карты) */
    assert(cast(-2.0f, 0.5f, 1.0f, 0.0f, 0, 0, &hx, &hy) == 1);
    assert(near_eq(hx, 0.0f) && near_eq(hy, 0.5f));
}

/*
 * Тест engine_run: проверяем только ошибочные ситуации, при которых
 * функция должна вернуть 0 ещё до создания окна. Нормальную работу
 * (рисование) проверяет интерактивная часть - глазами.
 */
static void test_engine_run(void)
{
    int *rows[1];
    int cells[1] = {1};

    rows[0] = cells;

    /* карта не задана */
    set_map(0, 0, NULL);
    assert(engine_run() == 0);

    /* указатель на карту NULL при ненулевых размерах */
    set_map(10, 10, NULL);
    assert(engine_run() == 0);

    /* нулевая ширина */
    set_map(0, 1, rows);
    assert(engine_run() == 0);

    /* нулевая высота */
    set_map(1, 0, rows);
    assert(engine_run() == 0);

    /* отрицательный размер */
    set_map(-1, -1, rows);
    assert(engine_run() == 0);

    /* вернуть "карту не задана", чтобы тесты не влияли на остальной код */
    set_map(0, 0, NULL);
}

/* Запуск всех тестов engine */
static void run_engine_tests(void)
{
    test_ray_cell_intersect();
    test_engine_run();
    printf("engine tests: OK\n");
}

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

    /* сначала автоматические тесты engine (без окна) */
    run_engine_tests();

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
