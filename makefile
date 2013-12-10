all: maitre coordinator slave

maitre: maitre.c
	mpicc maitre.c -o maitre

coordinator: coordinator.c
	mpicc coordinator.c -o coordinator `sdl-config --cflags --libs` -lSDL_ttf

slave: slave.c
	mpicc slave.c -o slave

test : test.c
	gcc test.c -o test `sdl-config --cflags --libs` -lSDL_ttf

.PHONY: clean

clean: 
	rm coordinator
	rm slave
	rm maitre
	rm test
