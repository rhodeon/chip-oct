COMPILER = g++

SRC = main.cpp

LINKERS = -lSDL2 -lSDL2_mixer

OBJ = chip_oct

all: $(SRC)
	$(COMPILER)	$(SRC) $(LINKERS) -o $(OBJ) 