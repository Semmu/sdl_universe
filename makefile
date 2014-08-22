# what to compile
OBJS = main.cpp

# what to compile with
CC = g++

# compile flags
COMPILER_FLAGS = -Wall

# what to link
LINKER_FLAGS = -lSDL2

# output name
OBJ_NAME = SU


# always run, i like it
all: $(OBJ_NAME)
	./$(OBJ_NAME)

# magic
$(OBJ_NAME): $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# fresh start
clean:
	rm $(OBJ_NAME)