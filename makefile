CC     = g++
CFLAGS = -g -Wall -I.
DEPS   = instructions.hpp assembler.hpp test.S
OBJ    = objects/assembler.o objects/instructions.o objects/preprocessing.o 


objects/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

assembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	./assembler