CC = g++
IFLAGS = -I./src/includes/
FLAGS = -fno-omit-frame-pointer

SRC = ./src/assembler.cpp ./src/cpu.cpp ./src/log.cpp ./src/stack.cpp   

OBJ_TEMP = $(subst ./src/, ./src/build/, $(SRC))
OBJ      = $(subst .cpp,.o, $(OBJ_TEMP))

all: $(OBJ)
	$(CC) $(FLAGS) $(IFLAGS) ./src/build/assembler.o ./src/build/log.o -o assembler              # $^ - всё что справа от all:
	$(CC) $(FLAGS) $(IFLAGS) ./src/build/cpu.o ./src/build/stack.o ./src/build/log.o -o cpu
 
./src/build/%.o: ./src/%.cpp
	mkdir -p ./src/build
	$(CC) $(FLAGS) $(IFLAGS) -c  $^ -o $@	   # $@ - всё что слева 

clear: 
	rm -f ./src/build/*.o cpu assembler
	