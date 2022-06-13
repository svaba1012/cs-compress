INCLUDES= ./src/includes

OBJ_FILES= ./build/main.o ./build/data_structures/priority_queue.o ./build/huffman.o ./build/compress/vcl.o ./build/compress/text/cst.o ./build/compress/audio/csw.o ./build/fft/fft.o

FLAGS= -O2 -Wall
.PHONY = all clean

all: ./bin/program.exe

./bin/program.exe: $(OBJ_FILES)
	gcc $(FLAGS) -o $@ $(OBJ_FILES) -lm


./build/%.o: ./src/%.c
	gcc -I $(INCLUDES) -c $< -o $@


clean:
	rm -f ./build/*/*/*.o
	rm -f ./build/*/*.o
	rm -f ./build/*.o
	rm -f ./bin/*.exe