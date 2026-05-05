CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LIBS = $(shell sdl2-config --cflags --libs) -lSDL2_image

TARGET = kirby_land

SRC = src/main.cpp \
      src/Game.cpp \
      src/Player.cpp \
      src/Texture.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)