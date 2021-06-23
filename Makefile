CC := g++
CFLAGS := -Wall -Wextra
LIBS := -lSDL2 -lGL -lGLU
OBJ := main.o
SRC := main.cpp car.cpp states.cpp
main:
	$(CC) $(SRC) $(LIBS) $(CFLAGS) -o $(OBJ)
run:
	./$(OBJ)
