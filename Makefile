OBJS = src/*
OUT = hello_world.out
CC = g++
COMPILER_FLAGS = -w -Wall -pedantic `sdl2-config --cflags --libs`
LINKER_FLAGS = -lSDL2

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o ${OUT}
	./$(OUT)
