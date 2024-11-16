build: 
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lSDL2_image -lSDL2_ttf -o game

run:
	./game

clean:
	rm game
	clear
