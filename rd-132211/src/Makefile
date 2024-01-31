CC = gcc
CFLAGS = -Wall -g -I/usr/local/include
LDFLAGS = -lglfw -lGLEW -lGL -lGLU

SRC = main.c

all: rubydung

rubydung: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f rubydung