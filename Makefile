CC = gcc
ALLEG = `allegro-config --libs`
PTHREAD = -lpthread -lrt 
MATH = -lm

all: bin/main

clean:
	rm -f bin/* build/*
	rmdir bin/ build/

# Object files #

build/main.o: src/main.c
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@

build/%.o: 	src/%.c src/%.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@
			
# Executables #

bin/main: build/main.o build/functions.o build/ptask.o 
	mkdir -p bin/
	$(CC) $^ -o $@ $(ALLEG) $(PTHREAD) $(MATH)