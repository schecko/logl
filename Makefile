SRCS = $(wildcard src/*.cpp) lib/glad/src/glad.c
OBJS = $(addsuffix .o,$(basename $(SRCS)))
DEPS = $(OBJS:.o=.d)
INC = -Ilib/glad/include
OUT = main
CC = g++
COMPILER_FLAGS = -ggdb -Wall -pedantic -c `sdl2-config --cflags`
LINKER_FLAGS = -lSDL2 -lSDL2_image -ldl `sdl2-config --libs`

# glad depends on libdl

%.o: %.c
	$(CC) $(INC) $(COMPILER_FLAGS) $< $(OUTPUT_OPTION)

%.o: %.cpp
	$(CC) $(INC) $(COMPILER_FLAGS) $< $(OUTPUT_OPTION)

all: $(OBJS)
	$(CC) $(OBJS) $(LINKER_FLAGS) -o ${OUT}


run: all
	./$(OUT)

.PHONY: clean

clean:
	rm $(OBJS) $(OUT)
