all: tetris4h

tetris4h: ../src/main.c
	gcc -o tetris4h ../src/main.c -Wall -lSDL -lSDL_image -std=c99 -D_BSD_SOURCE -g -lm -lSDL_ttf

clean:
	rm tetris4h
