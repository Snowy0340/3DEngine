CFLAGS=-lSDL2

# Тест backend
test_backend: test_backend.c backend.c
	gcc test_backend.c backend.c $(CFLAGS) -o /tmp/test_backend

# Основная программа
main: main.c engine.c intersect.c vec2.c backend.c
	gcc main.c engine.c intersect.c vec2.c backend.c -lSDL2 -o program.exe
