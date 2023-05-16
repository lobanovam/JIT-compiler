CC = g++
IFLAGS = -I./includes/
FLAGS = -fno-omit-frame-pointer

SRC = ./src/JIT.cpp ./src/log.cpp ./src/translator.cpp ./src/main.cpp

OBJ_TEMP = $(subst ./src/, ./src/build/, $(SRC))
OBJ      = $(subst .cpp,.o, $(OBJ_TEMP))

all: $(OBJ)
	$(CC) $(FLAGS) $(IFLAGS) $^ -o main        # $^ - всё что справа от all:

./src/build/%.o: ./src/%.cpp
	mkdir -p ./src/build
	$(CC) $(FLAGS) $(IFLAGS) -c  $^ -o $@	   # $@ - всё что слева 

clear: 
	rm -f ./src/build/*.o main

PNGclear:
	rm -f ./graphs/*.png

DOTclear:
	rm -f ./dotFiles/*.dot