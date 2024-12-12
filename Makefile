build: 
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o game

run:
	./game

clean:
	rm game
	clear
publish:
	cd ..
	zip -r game.zip ./Project_C/
	cd Project_C
