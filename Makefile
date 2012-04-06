IDIR = ./include
ODIR = obj
BDIR = bin

CC = gcc
CFLAGS = -Wall -I$(IDIR) -g
LIBS = #Ex.: -lm

EXE = tourdefrance

_DEPS = ciclista.h queue.h terreno.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o queue.o ciclista.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BDIR)/$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(BDIR)/$(EXE) *~ core $(INCDIR)/*~ 
