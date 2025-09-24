# Makefile — build rubydung (Linux/macOS/Windows-MinGW)

# Toolchain
CC      := gcc
CSTD    := -std=c99
WARN    := -Wall -Wextra
INCLUDE := -I.

# Build type: debug (default) or release
BUILD ?= debug

# Sources / Objects
SRC = rubydung.c player.c textures.c timer.c entity.c \
      level/chunk.c level/level.c level/levelrenderer.c \
      level/tessellator.c level/tile.c level/frustum.c \
      phys/aabb.c hitresult.c
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

# Platform detect
UNAME_S := $(shell uname -s)

# Common flags
CFLAGS  := $(CSTD) $(WARN) $(INCLUDE)
DEPFLAGS := -MMD -MP

# Per-build-type flags
ifeq ($(BUILD),release)
    CFLAGS  += -O2 -DNDEBUG
else
    CFLAGS  += -O0 -g
endif

# Linker flags / output name per-OS
ifeq ($(UNAME_S),Linux)
    EXE     := rubydung
    LDFLAGS := -lglfw -lGLEW -lGL -lGLU -lm -lz
endif

ifeq ($(UNAME_S),Darwin)
    EXE     := rubydung
    # Homebrew glfw/glew: -lglfw -lGLEW, OpenGL(+GLU) comes via framework
    LDFLAGS := -lglfw -lGLEW -framework OpenGL -lz
endif

ifeq ($(OS),Windows_NT)
    EXE := rubydung.exe
    # MSYS2 / MinGW (assumes -lglfw3 -lglew32 in your env)
    ifeq ($(BUILD),release)
        LDFLAGS := -lglfw3 -lglew32 -lopengl32 -lglu32 -lz -mwindows
    else
        LDFLAGS := -lglfw3 -lglew32 -lopengl32 -lglu32 -lz
    endif
endif

# Default target
.PHONY: all
all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile C -> OBJ with depgen
%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Convenience targets
.PHONY: debug release run clean
debug:
	$(MAKE) BUILD=debug
release:
	$(MAKE) BUILD=release
run: $(EXE)
	./$(EXE)

clean:
	@rm -f $(EXE) $(OBJ) $(DEP) 2>/dev/null || true

# Include generated deps (if present)
-include $(DEP)
