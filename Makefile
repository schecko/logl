SRCS = $(wildcard src/*.cpp) ./lib/glad/src/glad.c
OBJS = $(SRCS:.c*=.o)
DEPS = $(OBJS:.o=.d)
INC = -Ilib/glad/include
OUT = main
CC = g++
COMPILER_FLAGS = -ggdb -Wall -pedantic -c `sdl2-config --cflags`
LINKER_FLAGS = -lSDL2 -lSDL2_image `sdl2-config --libs`

%.o: %.c
	$(CC) $(INC) $< $(COMPILER_FLAGS) $(OUTPUT_OPTION) $(@:.d=.o) >$@

all: $(OBJS)
	$(CC) $(INC) $(OBJS) $(LINKER_FLAGS) -o ${OUT}


run: all
	./$(OUT)
