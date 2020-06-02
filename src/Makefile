COMPILER = g++

SRC = main.cpp chip8.cpp tinyfiledialogs/tinyfiledialogs.c

LINKERS = -lSDL2 -lSDL2_mixer 

OBJ = chip_oct

DIR = bin

all: $(SRC)
	$(COMPILER)	$(SRC) $(LINKERS) -o $(OBJ) 
	cd .. && mkdir $(DIR) && mv src/$(OBJ) ${DIR}