CC = gcc
SORTIE = principal
SRC = principal.c

# Fichiers générés à supprimer
FICHIERS = vol_max.dat vol_capte.dat vol_traite.dat rendements.dat histo.txt data.dat

.PHONY: all run clean

all: run

$(SORTIE): $(SRC)
	$(CC) $(SRC) -o $(SORTIE)

run: $(TARGET)
	./$(SORTIE)
	$(MAKE) clean

clean:
	rm -f $(SORTIE) *.o $(FICHIERS)
