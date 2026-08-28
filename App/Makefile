CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LIBS = $(shell sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf

TARGET = kirby_land

SRC = src/main.cpp \
      src/core/Game.cpp \
      src/graphics/Texture.cpp \
      src/graphics/TextRenderer.cpp \
      src/entities/Player.cpp \
      src/entities/CollectibleItem.cpp \
      src/scenes/StartScene.cpp \
      src/scenes/BedroomScene.cpp \
      src/scenes/HomeScene.cpp \
      src/scenes/CarScene.cpp \
      src/scenes/DrivingScene.cpp \
      src/scenes/E470Scene.cpp \
      src/scenes/GunClubScene.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

run: $(TARGET)
	./$(TARGET) $(if $(SCENE),--scene $(SCENE),)

clean:
	rm -f $(OBJ) $(TARGET)