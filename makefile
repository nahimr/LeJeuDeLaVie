.PHONY: main
main:
	
	gcc -Wall main.c -L /usr/lib -lSDL2 -lSDL2main -I /usr/local/include -o main && ./main
clean:
	rm main
run:
	./main
