INCLUDES= ./src/includes

OBJ_FILES= ./build/main.o ./build/data_structures/priority_queue.o ./build/huffman.o ./build/compress/vcl.o ./build/compress/text/cst.o ./build/compress/audio/csw.o ./build/fft/fft.o

OBJ_TEST_FILES= ./build/main.ot ./build/data_structures/priority_queue.ot ./build/huffman.ot ./build/compress/vcl.ot ./build/compress/text/cst.ot ./build/compress/audio/csw.ot ./build/fft/fft.ot

# opcije za optimizaciju kompleksnog mnozenja
COMPLEX_OPT_FLAGS= -fcx-limited-range -fno-cx-fortran-rules

FLAGS= -O2 -Wall 
TEST_FLAGS= -pg
.PHONY = all clean

all: ./bin/csp.exe

test: ./bin/program.exet

./bin/csp.exe: $(OBJ_FILES)
	gcc $(FLAGS) -o $@ $(OBJ_FILES) -lm

./bin/program.exet: $(OBJ_TEST_FILES)
	gcc $(FLAGS) $(TEST_FLAGS) -o $@ $(OBJ_TEST_FILES) -lm


./build/fft/fft.ot: ./src/fft/fft.c
	gcc -I $(INCLUDES) $(TEST_FLAGS) $(COMPLEX_OPT_FLAGS) -c $< -o $@

./build/%.ot: ./src/%.c
	gcc -I $(INCLUDES) $(TEST_FLAGS) -c $< -o $@

./build/fft/fft.o: ./src/fft/fft.c
	gcc -I $(INCLUDES) $(COMPLEX_OPT_FLAGS) -c $< -o $@


./build/%.o: ./src/%.c
	gcc -I $(INCLUDES) -c $< -o $@


clean:
	rm -f ./build/*/*/*.o
	rm -f ./build/*/*.o
	rm -f ./build/*.o
	rm -f ./bin/*.exe

	

test_clean: 
	rm -f ./build/*/*/*.ot
	rm -f ./build/*/*.ot
	rm -f ./build/*.ot
	rm -f ./bin/*.exet
	rm ./gmon.out
	rm ./time_test_info.txt 