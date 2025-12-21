CC = gcc
CFLAGS = -Wall -Wextra -O2

EXEC = prog

SRC = principal.c Src/avl.c Src/arbre.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

clean:
	rm -f $(OBJ) $(EXEC)

