CC = gcc
CFLAGS = -Wall -g -I/usr/local/include -I.
LDFLAGS = -lglfw -lGLEW -lGL -lGLU -lm -lz

SRC = rubydung.c player.c textures.c timer.c level/chunk.c level/level.c level/levelrenderer.c level/tessellator.c level/tile.c level/frustum.c phys/aabb.c hitresult.c
OBJ = $(SRC:.c=.o)

all: rubydung

rubydung: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f rubydung $(OBJ)
