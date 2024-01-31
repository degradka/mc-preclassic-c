CC = gcc
CFLAGS = -Wall -g -I/usr/local/include
LDFLAGS = -lglfw -lGLEW -lGL -lGLU

SRC = rubydung.c player.c level/chunk.c level/level.c level/levelrenderer.c level/tessellator.c level/tile.c
OBJ = $(SRC:.c=.o)

all: rubydung

rubydung: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f rubydung $(OBJ)
