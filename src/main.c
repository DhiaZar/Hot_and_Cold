#include <math.h>

#include "./includes.h"
#include "./constants.h"



int game_is_running = FALSE;
int WINDOW_WIDTH = (800);
int WINDOW_HEIGHT = (600);
int textWidth,textHeight,textPadding,textSeparation;
int windowed =1 ;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Texture* timerTexture = NULL;
SDL_Texture* triesTexture = NULL;
Mix_Music* bgMusic = NULL;
Mix_Chunk* digSound = NULL;
Mix_Chunk* foundSound = NULL;
Mix_Chunk* nextSound = NULL;
Mix_Chunk* winSound = NULL;
Mix_Chunk* loseSound = NULL;
SDL_Texture* characterTexture = NULL;
SDL_Texture* game_back = NULL;
SDL_Texture* levelTexture = NULL;
SDL_Texture* chest_texture = NULL;
SDL_Rect chestsRect[3];

int last_frame_time = 0;
int move_hori = 0;
int move_vert = 0;
int in_menu = 1;
int timer = 0;
int minutes,seconds;
int level = 1;
int tries = 0;
int win,passing;
int scalingProgress1,scalingProgress2;
int characterSelected = 1;
unsigned end_time,start_time;
char timerText[16];
char levelText[16];
char triesText[16];
char winText[16];
float progress;
int progressBarWidth,temp,dist;

const char* menuItems[NUM_MENU_ITEMS] = {"Start Game", "Controls", "Quit"} ;
const char* pauseItems[NUM_PAUSE_ITEMS] = {"Resume", "Controls", "Return to Menu", "Quit"};
const char* controlsItems[NUM_CONTROLS_ITEMS] = {"Escape - Quit Game", "Backspace - Pause Game","Enter - Select","Arrows - Movement","F - Dig","F11 - Fullscreen"};
const char* characterNames[3] = {"Vex","Nova","Ryder"};
const char* backgrounds[4] = {"C:/Coding/untitled2/img/underwater.png","C:/Coding/untitled2/img/space.png","C:/Coding/untitled2/img/forest.png","C:/Coding/untitled2/img/house.png"};
int selectedItem= 0;
char backgroundSelected[50];

SDL_Color colorNormal = {255,255,255,255};
SDL_Color colorSelected = {255,0,0,255};
SDL_Color timerColor = {255,255,255,255};
SDL_Color controlsColor = {0,0,0,255};

void nextLevel();
void setup();
float max(float a,float b){
	if(a>b){
		return a;
	}else{
		return b;}
}
int distance(int x1,int y1,int x2,int y2) {
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}


struct ball {
	float x;
	float y;
	float width;
	float height;
} ball;
struct chest {
	float x;
	float y;
	float width;
	float height;
	int state;
} chest;

struct chest chests[3];
void showChests();
int initialize_window(void){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		fprintf(stderr, "Error Initializing SDL. \n");
		return FALSE;
	}
	window = SDL_CreateWindow(
		"Chasse aux Tresors",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE
		);
	if(!window){
		fprintf(stderr, "Error creating SDL Window \n");
		return FALSE;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!renderer){
		fprintf(stderr, "Error creating SDL Renderer \n");
		return FALSE;
	}
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags ) & imgFlags)){
		fprintf(stderr,"SDL_image could not initliaze! \n");
		return FALSE;
	}
	if(TTF_Init() == -1){
		fprintf(stderr,"SDL_TTF could not initialize ");
		return FALSE;
	}
	font = TTF_OpenFont("C:/Coding/untitled2/font/joystix.otf",FONT_SIZE);
	if(!font){
		fprintf(stderr, "Failed to load font! \n ");
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		return FALSE;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
        return FALSE;
    }
	bgMusic = Mix_LoadMUS("C:/Coding/untitled2/audio/theme.mp3");
	if (!bgMusic) {
        printf("Failed to load background music! Mix_Error: %s\n", Mix_GetError());
        return FALSE;
    }
	digSound = Mix_LoadWAV("C:/Coding/untitled2/audio/shovel.wav");
	if (!digSound) {
		printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
		return FALSE;
	}
	winSound = Mix_LoadWAV("C:/Coding/untitled2/audio/win.wav");
	if(!winSound) {
		printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
		return FALSE;
	}
	foundSound = Mix_LoadWAV("C:/Coding/untitled2/audio/success.wav");
	if (!foundSound) {
		printf("Failed to load background music! Mix_Error: %s\n", Mix_GetError());
		return FALSE;
	}
	loseSound = Mix_LoadWAV("C:/Coding/untitled2/audio/lost.wav");
	if (!loseSound) {
		printf("Failed to load background music! Mix_Error: %s\n", Mix_GetError());
		return FALSE;
	}
	nextSound = Mix_LoadWAV("C:/Coding/untitled2/audio/nextlevel.wav");
	if (!nextSound) {
		printf("Failed to load background music! Mix_Error: %s\n", Mix_GetError());
		return FALSE;
	}
	return TRUE;

}

SDL_Texture* RenderText(const char* message, TTF_Font* font, SDL_Color color, SDL_Renderer *render){
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, message, color);
    if (!textSurface) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!texture) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }

    return texture;
}
SDL_Texture* RenderTextStroke(const char* message, TTF_Font* font, SDL_Color color,SDL_Color color2, SDL_Renderer *render){
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, message, color,color2);
	if (!textSurface) {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return NULL;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	if (!texture) {
		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
	}

	return texture;
}

SDL_Texture* LoadTexture(const char* file, SDL_Renderer* renderer){
	SDL_Texture* texture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(file);

	if(!loadedSurface){
		fprintf(stderr, " Unable to load image ! \n");
	}else{
		texture = SDL_CreateTextureFromSurface(renderer,loadedSurface);
		SDL_FreeSurface(loadedSurface);
		if(!texture){
			fprintf(stderr,"Unable to create texture ! \n");
		}
	
	}
	return texture;
}



void process_input(){
	//check for input
	SDL_Event event;
	SDL_PollEvent(&event);

	switch(event.type){
		//SDL_QUIT is the X button on the title window
		case SDL_QUIT:
			game_is_running = FALSE;
			break;
		case SDL_WINDOWEVENT:
			if(event.window.event == SDL_WINDOWEVENT_MAXIMIZED || event.window.event == SDL_WINDOWEVENT_RESTORED || event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
				SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
				SDL_RenderPresent(renderer);
			}
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
			// WINDOW_HEIGHT = event.window.data1;
			// WINDOW_WIDTH = event.window.data2;
			SDL_RenderPresent(renderer);
			break;
		case SDL_KEYDOWN:
			if( event.key.keysym.sym == SDLK_ESCAPE){
				game_is_running = FALSE;
			}
			if(in_menu==0){
				if(event.key.keysym.sym == SDLK_f) {
					Mix_PlayChannel(-1,digSound,0);
					if(temp < 128) {
						Mix_PlayChannel(-1,foundSound,0);
						for (int i = 0; i < level; ++i) {
							if(distance(ball.x,ball.y,chestsRect[i].x,chestsRect[i].y) <= 128) {
								chests[i].state = 2;
								temp = MAXIMUM_DISTANCE;
								break;
							}

						}

					}
					tries--;
				}
			if( event.key.keysym.sym == SDLK_RIGHT){
				move_hori = 1;
			}
			if(event.key.keysym.sym == SDLK_LEFT){
				move_hori = -1;
			}
			if( event.key.keysym.sym == SDLK_UP){
				move_vert = -1;
			}
			if(event.key.keysym.sym == SDLK_DOWN){
				move_vert = 1;
			}
			if(event.key.keysym.sym == SDLK_BACKSPACE){
				in_menu=2;
				Mix_PauseMusic();
			}
			}else{
				if( event.key.keysym.sym == SDLK_LEFT && in_menu == 5){
					selectedItem= (selectedItem- 1 + 4) % 4;
				}
				if(event.key.keysym.sym == SDLK_RIGHT && in_menu == 5){
					selectedItem= (selectedItem + 1) % 4;
				}
				if( event.key.keysym.sym == SDLK_LEFT && in_menu == 4){
					selectedItem= (selectedItem- 1 + 3) % 3;
				}
				if(event.key.keysym.sym == SDLK_RIGHT && in_menu == 4){
					selectedItem= (selectedItem + 1) % 3;
				}
				if( event.key.keysym.sym == SDLK_UP && in_menu == 1){
				selectedItem= (selectedItem- 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
			}
			if(event.key.keysym.sym == SDLK_DOWN && in_menu == 1){
				selectedItem= (selectedItem + 1) % NUM_MENU_ITEMS;
			}
			if( event.key.keysym.sym == SDLK_UP && in_menu == 2){
				selectedItem= (selectedItem- 1 + NUM_PAUSE_ITEMS) % NUM_PAUSE_ITEMS;
			}
			if(event.key.keysym.sym == SDLK_DOWN && in_menu == 2) {
				selectedItem= (selectedItem+ 1) % NUM_PAUSE_ITEMS;
			}
				if(event.key.keysym.sym == SDLK_BACKSPACE ||in_menu==3) {
					if(timer < end_time) {
						in_menu = 2;
					}else {
						in_menu=1;
					}
			}
			if(event.key.keysym.sym == SDLK_RETURN){
				if(in_menu == 1){
					if (selectedItem== 0) {
                        //printf("Start Game selected\n");
						//start_time = SDL_GetTicks();
                        in_menu = 4;
						//Mix_PlayMusic(bgMusic,-1);
	                    } else if (selectedItem== 1) {
	                        in_menu = 3;
	                    } else if (selectedItem== 2) {
	                        game_is_running = FALSE;
	                    }
				break;}
				if(in_menu == 2){
					if (selectedItem== 0) {
	                        //printf("Start Game selected\n");
	                        in_menu = 0;
							Mix_ResumeMusic();
	                    } else if (selectedItem== 1) {
	                        in_menu = 3;
	                    } else if (selectedItem== 2) {
	                        in_menu =1;
							Mix_HaltMusic();
							setup();
	                    }else if(selectedItem==3){
							game_is_running = FALSE;
						}}
				if(in_menu == 4) {
					characterSelected = selectedItem +1;
					in_menu = 5;
					break;
				}
				if(in_menu == 5) {
					sprintf(backgroundSelected,backgrounds[selectedItem]);
					game_back = LoadTexture(backgroundSelected,renderer);
					start_time = SDL_GetTicks();
					nextLevel();
					Mix_PlayMusic(bgMusic,0);
					in_menu = 0;
				}
			}
			}
			break;
		case SDL_KEYUP:
			if(!in_menu){
			if(event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT){
				move_hori = 0;
			}
			if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN){
				move_vert = 0;
			}}
			if(event.key.keysym.sym == SDLK_F11){
				windowed = !windowed;
				if(windowed){
					SDL_SetWindowFullscreen(window, SDL_FALSE);
				}else{
					SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				}
				SDL_GetWindowSize(window,&WINDOW_WIDTH,&WINDOW_HEIGHT);
			}
			break;

	}

}

void update(){
	//Keeping a fixed timestep
	//if()
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
		SDL_Delay(time_to_wait);
	}
	//while(!SDL_TICKS_PASSED(SDL_GetTicks(),last_frame_time+FRAME_TARGET_TIME));
	//using while uses the processor without actually doing something
	// difference of ticks from last frame converted to seconds
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	for(int i = 0; i < 3; i++) {
		if(chests[i].state != 0){
		chestsRect[i].x = chests[i].x;
		chestsRect[i].y = chests[i].y;
		}
	}
	last_frame_time = SDL_GetTicks();
	timer = end_time+start_time - last_frame_time;
	minutes = timer / 60000;
	seconds = (timer % 60000) / 1000;
	snprintf(timerText,sizeof(timerText),"%02d:%02d",minutes,seconds);
	sprintf(levelText,"Level : %d",level);
	sprintf(triesText,"Tries : %d",tries);
	// move the ball 20 pixels per second
	ball.x += 200 * delta_time * move_hori;
	ball.y += 200 * delta_time * move_vert;
	if(in_menu == 0) {
		int iteration = 0;
		for (int i = 0; i < level; ++i) {
			if(chests[i].state == 1){
			dist = distance(chests[i].x,chests[i].y,ball.x,ball.y);
			//printf("chest %d dist %d ",i,dist);
			if(iteration==0) {
				temp=dist;
			}else{
				if(dist < temp) {
					temp = dist;
				}
			}
			iteration++;
			//printf("chest %d dist %d\n",i,temp);
		}
	}
		if(tries==0) {
			in_menu = 6;
			for(int i = 0; i < level; ++i) {
				if(chests[i].state != 2) {
					win = 0;
					break;
				}else {
					win = 1;
				}
			}
		}else {
			passing = 0;
			switch(level){
				case 3:
					for(int i = 0; i < level; ++i) {
						//printf("Chest %d State %d   ",i,chests[i].state);
						if(chests[i].state != 2) {
							win = 0;
							break;
						}else {
							win = 1;
						}
					}
					if(win) {
						in_menu = 6;
					}
					break;
				case 2:
					for(int i = 0; i < level; ++i) {
						if(chests[i].state != 2) {
							passing = 0;
							break;
						}else {
							passing = 1;
						}
					}
					if(passing) {
						level=3;
						printf("Progressing to level 3");
						nextLevel();
					}
					break;
				case 1:
					for(int i = 0; i < level; ++i) {
						if(chests[i].state != 2) {
							passing = 0;
							break;
						}else {
							passing = 1;
						}
					}
					if(passing) {
						level=2;
						printf("Progressing to level 2");
						nextLevel();
					}
					break;
		}
		}
	}
	scalingProgress1 = WINDOW_WIDTH / 800;
	scalingProgress2 = WINDOW_HEIGHT / 400;
	progress = max(1.0f - ((float)temp / MAXIMUM_DISTANCE),0.0f);
	progressBarWidth = (int)(progress * 200 * scalingProgress1);
}


void render(){
	SDL_RenderClear(renderer);
	//SDL_SetRenderDrawColor(renderer, <R> , <G> , <B> , <A>);
	SDL_Texture* menu_back = LoadTexture("C:/Coding/untitled2/img/main_menu.png",renderer);
	//characterTexture = LoadTexture("C:/Coding/untitled2/img/charac.png",renderer);

	SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
	//SDL_Texture* grassTexture = LoadTexture("C:/Coding/untitled2/img/grass.png",renderer);
	//TODO : HERE is where we can start drawing our game
	//Draw a rectangle
	SDL_Rect characSe = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.width,
		(int)ball.height
	};



	if(in_menu==0){
		SDL_RenderCopy(renderer, game_back, NULL, NULL);
		char pathtocharacter[100];
		sprintf(pathtocharacter,"C:/Coding/untitled2/img/charac%d.png",characterSelected);
		SDL_Texture* characterTexture = LoadTexture(pathtocharacter,renderer);
		SDL_Rect charac = {
			(int)ball.x,
			(int)ball.y,
			(int)ball.width,
			(int)ball.height
		};
		SDL_RenderCopy(renderer, characterTexture, NULL, &charac);
		SDL_DestroyTexture(characterTexture);
		for (int i = 0; i < 3; ++i) {
			if(chests[i].state == 2) {
				SDL_RenderCopy(renderer, chest_texture, NULL, &chestsRect[i]);
			}
		}
		if(passing){
		SDL_RenderPresent(renderer);
		SDL_Delay(2000);
		passing =0 ;
		}
	timerTexture = RenderText(timerText,font,timerColor,renderer);
	SDL_QueryTexture(timerTexture,NULL,NULL,&textWidth,&textHeight);
	levelTexture = RenderText(levelText,font,timerColor,renderer);
	SDL_QueryTexture(levelTexture,NULL,NULL,&textWidth,&textHeight);
	triesTexture = RenderText(triesText,font,timerColor,renderer);
	SDL_QueryTexture(triesTexture,NULL,NULL,&textWidth,&textHeight);
	SDL_Rect textRect = {10,10,textWidth,textHeight};
	SDL_Rect triesRect = {10,20+textHeight,textWidth,textHeight};
	SDL_Rect levelRect = {WINDOW_WIDTH - textWidth,10,textWidth,textHeight};
	//SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, triesTexture,NULL,&triesRect);
	SDL_DestroyTexture(triesTexture);
	SDL_RenderCopy(renderer, timerTexture,NULL,&textRect);
	SDL_DestroyTexture(timerTexture);
	SDL_RenderCopy(renderer, levelTexture,NULL,&levelRect);
	SDL_DestroyTexture(levelTexture);
		SDL_Rect backgroundBar = {WINDOW_WIDTH/2-100, 20, 200*scalingProgress1, 10*scalingProgress2};
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 128);
		SDL_RenderFillRect(renderer, &backgroundBar);

		// Draw the progress bar foreground (green)
		SDL_Rect progressBar = {WINDOW_WIDTH/2-100, 20, progressBarWidth, 10*scalingProgress2};
		SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
		SDL_RenderFillRect(renderer, &progressBar);
	
	}else
		if(in_menu == 1){
		SDL_RenderCopy(renderer, menu_back, NULL, NULL);
		SDL_DestroyTexture(menu_back);
		for(int i=0;i < NUM_MENU_ITEMS;i++){
			SDL_Color colorMenu = (i == selectedItem) ? colorSelected : colorNormal;
			SDL_Texture* textTextureMenu = RenderText(menuItems[i],font,colorMenu,renderer);
			if(textTextureMenu){
				
				SDL_QueryTexture(textTextureMenu, NULL, NULL, &textWidth, &textHeight);
				textSeparation = WINDOW_HEIGHT / 8 ;
				textPadding = 0.5 * (WINDOW_HEIGHT - (2*textSeparation) - (textHeight * 3));
				SDL_Rect renderQuad = { (WINDOW_WIDTH - textWidth )/2,textPadding+ i * textSeparation, textWidth,textHeight};
				SDL_RenderCopy(renderer, textTextureMenu, NULL, &renderQuad);
				SDL_DestroyTexture(textTextureMenu);
			}

		}
	}else
		if(in_menu == 2){
		SDL_RenderCopy(renderer, menu_back, NULL, NULL);
		SDL_DestroyTexture(menu_back);
		for(int i=0;i < NUM_PAUSE_ITEMS;i++){
			SDL_Color colorPause = (i == selectedItem) ? colorSelected : colorNormal;
			SDL_Texture* textTexturePause = RenderText(pauseItems[i],font,colorPause,renderer);
			if(textTexturePause){

				SDL_QueryTexture(textTexturePause, NULL, NULL, &textWidth, &textHeight);
				textPadding = 0.5 * (WINDOW_HEIGHT - (3*textSeparation) - (textHeight * 3));
				SDL_Rect renderQuad = { (WINDOW_WIDTH - textWidth )/2,textPadding+ i * textSeparation, textWidth,textHeight};
				SDL_RenderCopy(renderer, textTexturePause, NULL, &renderQuad);
				SDL_DestroyTexture(textTexturePause);
			}

		}
	}else
		if(in_menu == 3) {
		SDL_RenderCopy(renderer, menu_back, NULL, NULL);
		SDL_DestroyTexture(menu_back);
		for(int i=0;i < NUM_CONTROLS_ITEMS;i++){
			SDL_Texture* textTextureControl = RenderTextStroke(controlsItems[i],font,colorNormal,controlsColor,renderer);
			if(textTextureControl){

				SDL_QueryTexture(textTextureControl, NULL, NULL, &textWidth, &textHeight);
				textPadding = 0.5 * (WINDOW_HEIGHT - (3*textSeparation) - (textHeight * 3));
				SDL_Rect renderQuad1 = { (WINDOW_WIDTH - textWidth )/2 ,textPadding+ i * textSeparation, textWidth,textHeight};
				SDL_RenderCopy(renderer, textTextureControl, NULL, &renderQuad1);
				SDL_DestroyTexture(textTextureControl);
			}

		}
	}else
		if(in_menu == 4) {
			float scale = 1.5;
			SDL_Texture* menu_back = LoadTexture("C:/Coding/untitled2/img/main_menu.png",renderer);
			SDL_RenderCopy(renderer, menu_back, NULL, NULL);
			SDL_DestroyTexture(menu_back);
			SDL_Texture* textTextureS = RenderText("Select your character ",font,colorNormal,renderer);
			SDL_QueryTexture(textTextureS, NULL, NULL, &textWidth, &textHeight);
			SDL_Rect renderQuad2 = { WINDOW_WIDTH/2 - textWidth/2	 , 100, textWidth,textHeight};
			SDL_RenderCopy(renderer, textTextureS, NULL, &renderQuad2);
			SDL_DestroyTexture(textTextureS);
			for(int i=1;i <= 3;i++){
				SDL_Color colorPause = (i-1 == selectedItem) ? colorSelected : colorNormal;
				SDL_Texture* textTextureNames = RenderText(characterNames[i-1],font,colorPause,renderer);
				SDL_QueryTexture(textTextureNames, NULL, NULL, &textWidth, &textHeight);
				SDL_Rect renderQuad2 = { WINDOW_WIDTH/2 - 0.5*ball.width + scale*ball.width*((2*i)-5) + ball.width*scale*(i-1), WINDOW_HEIGHT / 2 - ball.height, textWidth,textHeight};
				SDL_RenderCopy(renderer, textTextureNames, NULL, &renderQuad2);
				SDL_DestroyTexture(textTextureNames);

				char pathtocharacter[100];
				sprintf(pathtocharacter,"C:/Coding/untitled2/img/charac%d.png",i);
				SDL_Texture* charac3 = LoadTexture(pathtocharacter,renderer);
				characSe.y = WINDOW_HEIGHT / 2 - ball.height*0.5;

				characSe.x = WINDOW_WIDTH/2 - 0.5*ball.width + scale*ball.width*((2*i)-5) + ball.width*scale*(i-1);
				SDL_RenderCopy(renderer, charac3, NULL, &characSe);
				SDL_DestroyTexture(charac3);
		}
		//SDL_RenderPresent(renderer);
	}else
		if(in_menu == 5) {
			SDL_RenderCopy(renderer, menu_back, NULL, NULL);
			SDL_Texture* textTextureS2 = RenderText("Select the Map ",font,colorNormal,renderer);
			SDL_QueryTexture(textTextureS2, NULL, NULL, &textWidth, &textHeight);
			SDL_Rect renderQuad2 = { WINDOW_WIDTH/2 - textWidth/2	 , 100, textWidth,textHeight};
			SDL_RenderCopy(renderer, textTextureS2, NULL, &renderQuad2);
			SDL_DestroyTexture(textTextureS2);
			int ww4 = WINDOW_WIDTH/4;
			SDL_Rect items[4] = {
				{0, 0, ww4, WINDOW_HEIGHT},
				{ww4+5, 0, ww4, WINDOW_HEIGHT},
				{2*ww4+10, 0, ww4, WINDOW_HEIGHT},
				{3*ww4+10, 0, ww4, WINDOW_HEIGHT},
			};
			for (int i = 0; i < 4; i++) {
				if (i == selectedItem) {
					// Highlight the selected rectangle in red
					SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
				} else {
					// Use a default grey color for unselected items
					SDL_SetRenderDrawColor(renderer, 100, 100, 100, 0);
				}
				SDL_RenderFillRect(renderer, &items[i]);

				// Draw the outline for all rectangles
				//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderDrawRect(renderer, &items[i]);
			}
		}else
			if(in_menu == 6) {
				SDL_RenderCopy(renderer, menu_back, NULL, NULL);
				if(Mix_PlayingMusic()) {
					Mix_HaltMusic();
				}
				if(!win) {
					sprintf(winText,"You Lose !");
					Mix_PlayChannel(-1, loseSound, 0);
				}else {
					sprintf(winText,"You Win !");
					Mix_PlayChannel(-1, winSound, 0);
				}
				SDL_Texture* textTextureS2 = RenderTextStroke(winText,font,colorNormal,controlsColor,renderer);
				SDL_QueryTexture(textTextureS2, NULL, NULL, &textWidth, &textHeight);
				SDL_Rect renderQuad2 = { WINDOW_WIDTH/2 - textWidth/2 , WINDOW_HEIGHT/2-textHeight, textWidth,textHeight};
				SDL_RenderCopy(renderer, textTextureS2, NULL, &renderQuad2);
				SDL_DestroyTexture(textTextureS2);
				SDL_RenderPresent(renderer);
				SDL_Delay(3000);
				setup();
				in_menu = 1;

			}
	
	// swap the double buffer

	SDL_RenderPresent(renderer);
}
void nextLevel() {
	switch(level){
		case 1:
			tries = 2;
			break;
		case 2:
			Mix_PlayChannel(-1,nextSound,0);
			tries = 3;
			break;
		case 3:
			Mix_PlayChannel(-1,nextSound,0);
			tries = 5;
			break;
	}
	for (int i = 0; i < level; ++i) {
		chests[i].state = 1;
		chests[i].x = rand() % (int)(WINDOW_WIDTH - 2*chest.width);
		chests[i].y = rand() % (int)(WINDOW_HEIGHT - 2*chest.height);
	}
}

void setup(){
	srand(time(NULL));
	start_time = SDL_GetTicks();
	chest_texture = LoadTexture("C:/Coding/untitled2/img/chest.png",renderer);
	/*SDL_SetWindowFullscreen(window, SDL_TRUE);
	SDL_GetWindowSize(window,&WINDOW_HEIGHT,&WINDOW_WIDTH);
	SDL_SetWindowFullscreen(window, SDL_FALSE);*/
	end_time = 121000;
	timer = 0;
	Mix_VolumeMusic(MIX_MAX_VOLUME * 0.70);
	ball.x = rand() % WINDOW_WIDTH + 1;
	ball.y = rand() % WINDOW_HEIGHT + 1;
	ball.width = 64;
	ball.height = 128;
	for(int i = 0; i < 3; i++) {
		chests[i].state = 0;
		chests[i].width = 64;
		chests[i].height = 64;
		chestsRect[i].w = chests[i].width;
		chestsRect[i].h = chests[i].height;
	}
}

void destroy_window(){
	SDL_DestroyTexture(chest_texture);
	SDL_DestroyTexture(game_back);
	Mix_HaltMusic();
	Mix_FreeMusic(bgMusic);
	Mix_FreeChunk(digSound);
	Mix_FreeChunk(winSound);
	Mix_FreeChunk(loseSound);
	Mix_FreeChunk(foundSound);
	Mix_FreeChunk(nextSound);
    Mix_CloseAudio();
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void showChests() {
	for (int i = 0; i < 3; i++) {
		printf(" Chest %d Coordinates(%f,%f)",i,chests[i].x,chests[i].y);
	}
	printf("\n");
}

int main(int argc, char* argv[]){

	game_is_running = initialize_window();

	setup();
	while(game_is_running){
		process_input();
		update();
		render();
		//showChests();
	}

	destroy_window();


	return 0;}
