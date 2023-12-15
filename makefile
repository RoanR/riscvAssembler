CC     = g++
CFLAGS = -g -Wall -I.
DEPS   = instructions.hpp
OBJ    = objects/assembler.o objects/instructions.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

assembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	./assembler