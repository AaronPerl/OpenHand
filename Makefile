GLM = lib/glm
SDL2 = lib/SDL2-2.0.3/x86_64-w64-mingw32
SDL2_32 = $(SDL2)/../i686-w64-mingw32
GLEW = lib/glew-1.11.0

LIB_PATHS = -L$(GLEW)/lib -L$(SDL2)/lib
LIB_PATHS_32 = -L$(GLEW)/lib32 -L$(SDL2_32)/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32
INC_PATHS = -I$(GLEW)/include -I$(SDL2)/include -I$(GLM)
INC_PATHS_32 = -I$(GLEW)/include -I$(SDL2_32)/include -I$(GLM)
FLAGS = -Wall
#FLAGS += -mwindows #Release mode

SRC = src/*.cpp src/libmodel/*.c

all: 64bit 32bit

64bit: GameLibTest.exe
32bit: 32bit/GameLibTest.exe

GameLibTest.exe: src/* src/libmodel/*
	g++ $(FLAGS) $(SRC) $(LIB_PATHS) $(INC_PATHS) $(LIBS) -o $@
	
32bit/GameLibTest.exe: src/* src/libmodel/*
	i686-w64-mingw32-g++ $(FLAGS) $(SRC) $(LIB_PATHS_32) $(INC_PATHS_32) $(LIBS) -o $@
	
clean:
	rm -f GameLibTest.exe 32bit/GameLibTest.exe