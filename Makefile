#Makefile 1:

#prog est l'executable
all : prog 

prog : main.o 
    gcc -o prog main.o

main.o : main.c 
    gcc -o main.o -c main.c 
    
clean :
    em -rf *.o
    
# Makefile 2:

all : prog 

main.o : main.c 
    gcc -o main.o -c main.c 
    
prog : main.o 
    gcc -o prog main.o    
    
clean :
    em -rf *.o
    
# Makefile 3: plutot generale 

CC = gcc
EXEC = prog
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all : $(EXEC)

%.o : %.c 
    $(CC) -o $@ -c $<
    
$(EXEC) : $(OBJ)
    $(CC) -o $@ $^

clean :
    rm -rf *.o
