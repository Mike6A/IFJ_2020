CC=gcc
CFLAGS=-Wall -std=c99 -pedantic
PROJ=IFJ20
SOURCES= $(shell find . -name '*.c')
OBJECTS= $(SOURCES:%.c=%.o)

all: $(PROJ)

$(PROJ): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS) 



