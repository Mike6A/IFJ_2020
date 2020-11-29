CC=gcc
CFLAGS=-Wall -std=c99 -pedantic
PROJ=IFJ20
SOURCES= $(shell find . -path ./cmake-build-debug -prune -false -o -name '*.c')
OBJECTS= $(SOURCES:%.c=%.o)

all: $(PROJ)

$(PROJ): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)
