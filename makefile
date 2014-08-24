# what to compile
OBJS = main.cpp SU.cpp

# what to compile with
CC = g++

# compile flags
COMPILER_FLAGS = -Wall -std=c++11 `sdl2-config --cflags`

# what to link
LINKER_FLAGS = `sdl2-config --libs` -lSDL2_ttf

# output name
OBJ_NAME = sdl_universe


# always run, i like it
all: $(OBJ_NAME)
	./$(OBJ_NAME)

# magic
$(OBJ_NAME): $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# fresh start
clean:
	rm -f $(OBJ_NAME) *.o