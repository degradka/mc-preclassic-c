CXX = g++
CXXFLAGS = -Wall -g -I/usr/local/include -I.
LDFLAGS = -lglfw -lGLEW -lGL -lGLU -lm

SRC = rubydung.cpp player.cpp window/window.cpp window/events.cpp level/chunk.cpp level/level.cpp level/levelrenderer.cpp level/tessellator.cpp level/tile.cpp
OBJ = $(SRC:.cpp=.o)
OUTPUT_NAME = rubydung

all: $(OUTPUT_NAME)

$(OUTPUT_NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OUTPUT_NAME) $(OBJ)

run: all
	./$(OUTPUT_NAME)