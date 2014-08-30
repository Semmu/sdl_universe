# what to compile
OBJS = main.cpp SU.cpp

# what to compile with
CC = g++

# compile flags
COMPILER_FLAGS = -O -Wall -std=c++11 `sdl-config --cflags` -DUSING_SDL1
COMPILER_FLAGS2 = -O -Wall -std=c++11 `sdl2-config --cflags`

# what to link
LINKER_FLAGS = `sdl-config --libs` -lSDL_ttf
LINKER_FLAGS2 = `sdl2-config --libs` -lSDL2_ttf

# output name
OBJ_NAME = sdl_universe

default: sdl2

sdl2: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS2) $(LINKER_FLAGS2) -o $(OBJ_NAME)
	./$(OBJ_NAME)

sdl1: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
	./$(OBJ_NAME)

# fresh start
clean:
	rm -f $(OBJ_NAME) *.o