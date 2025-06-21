CC = gcc
CFLAGS = -g -Wall -I/opt/homebrew/include/
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm
OBJ = gameLoop.o player.o weapon.o sprites.o render.o level.o genericDynamicList.o physics.o gameMode.o menu.o bar.o audio.o
TARGET = game


$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

gameLoop.o: gameLoop.c gameLoop.h player.h render.h level.h globalConsts.h gameMode.h physics.h menu.h audio.h sprites.h
	$(CC) $(CFLAGS) -c gameLoop.c

player.o: player.c player.h gameLoop.h weapon.h vector.h level.h globalConsts.h physics.h genericDynamicList.h bar.h audio.h sprites.h
	$(CC) $(CFLAGS) -c player.c

sprites.o: sprites.c sprites.h
	$(CC) $(CFLAGS) -c sprites.c

weapon.o: weapon.c weapon.h vector.h 
	$(CC) $(CFLAGS) -c weapon.c

render.o: render.c render.h level.h globalConsts.h sprites.h menu.h
	$(CC) $(CFLAGS) -c render.c

level.o: level.c level.h vector.h genericDynamicList.h player.h globalConsts.h render.h physics.h sprites.h
	$(CC) $(CFLAGS) -c level.c

physics.o: physics.c physics.h level.h globalConsts.h player.h
	$(CC) $(CFLAGS) -c physics.c

gameMode.o: gameMode.c gameMode.h
	$(CC) $(CFLAGS) -c gameMode.c

bar.o: bar.c bar.h player.h globalConsts.h menu.h
	$(CC) $(CFLAGS) -c bar.c

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c

audio.o: audio.c audio.h globalConsts.h weapon.h menu.h
	$(CC) $(CFLAGS) -c audio.c

genericDynamicList.o: genericDynamicList.c genericDynamicList.h
	$(CC) $(CFLAGS) -c genericDynamicList.c

clean:
	rm -f *.o $(TARGET)