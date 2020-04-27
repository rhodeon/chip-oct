COMPILER = g++

SRC = main.cpp

LINKERS = -lSDL2

OBJ = chip_oct

all: $(SRC)
	$(COMPILER)	$(SRC) $(LINKERS) -o $(OBJ) 