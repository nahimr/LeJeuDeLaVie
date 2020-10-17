.PHONY: main
main:
	gcc -Wall -std=c11 -Wextra -pedantic-errors main.c -L /usr/lib -lSDL2  -I /usr/local/include -o main && ./main
debug:
	gdb --args -Wall -std=c11 -Wextra -pedantic-errors main.c -L /usr/lib -lSDL2  -I /usr/local/include -o main && ./main
clean:
	rm main
run:
	./main
