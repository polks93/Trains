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

build/init.o: 	src/init.c src/init.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@

build/trains.o: 	src/trains.c src/trains.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@

build/graphics.o: 	src/graphics.c src/graphics.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@

build/user.o: 	src/user.c src/user.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@	

build/station.o: 	src/station.c src/station.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@	

build/ptask.o: 	src/ptask.c src/ptask.h
	mkdir -p build/
	$(CC) -c $(CONF) $< -o $@		


# Executables #

bin/main: build/main.o build/init.o build/trains.o build/graphics.o build/ptask.o build/user.o build/station.o
	mkdir -p bin/
	$(CC) $^ -o $@ $(ALLEG) $(PTHREAD) $(MATH)