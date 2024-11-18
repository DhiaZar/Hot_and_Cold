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


int last_frame_time = 0;
int move_hori = 0;
int move_vert = 0;
int in_menu = 1;
int timer = 0;
int minutes,seconds;
unsigned end_time;
char timerText[16];

const char* menuItems[NUM_MENU_ITEMS] = {"Start Game", "Controls", "Quit"} ;
const char* pauseItems[NUM_PAUSE_ITEMS] = {"Resume", "Controls", "Return to Menu", "Quit"};
int selectedItem= 0;

SDL_Color colorNormal = {255,255,255,255};
SDL_Color colorSelected = {255,0,0,255};
SDL_Color timerColor = {255,255,255,255};

void setup();

struct ball {
	float x;
	float y;
	float width;
	float height;
} ball ;


int initialize_window(void){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		fprintf(stderr, "Error Initializing SDL. \n");
		return FALSE;
	}
	window = SDL_CreateWindow(
		"Game Window", 
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
	renderer = SDL_CreateRenderer(window, -1, 0);
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
	font = TTF_OpenFont("./font/joystix.otf",FONT_SIZE);
	if(!font){
		fprintf(stderr, "Failed to load font! \n ");
		printf("TTF_OpenFont: %s\n", TTF_GetError());
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


void showControls(){
	//TODO;
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
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			WINDOW_HEIGHT = event.window.data1;
			WINDOW_WIDTH = event.window.data2;
			SDL_RenderPresent(renderer);
		case SDL_KEYDOWN:
			if( event.key.keysym.sym == SDLK_ESCAPE){
				game_is_running = FALSE;
			}
			if(in_menu==0){
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
			}
			}else{
				if( event.key.keysym.sym == SDLK_UP && in_menu == 1){
				selectedItem= (selectedItem- 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
			}
			if(event.key.keysym.sym == SDLK_DOWN && in_menu == 1){
				selectedItem= (selectedItem + 1) % NUM_MENU_ITEMS;
			}
			if( event.key.keysym.sym == SDLK_UP && in_menu == 2){
				selectedItem= (selectedItem- 1 + NUM_PAUSE_ITEMS) % NUM_PAUSE_ITEMS;
			}
			if(event.key.keysym.sym == SDLK_DOWN && in_menu == 2){
				selectedItem= (selectedItem+ 1) % NUM_PAUSE_ITEMS;
			}
			if(event.key.keysym.sym == SDLK_RETURN){
				if(in_menu == 1){
				if (selectedItem== 0) {
                        //printf("Start Game selected\n");
                        in_menu = 0;
                    } else if (selectedItem== 1) {
                        showControls();
                    } else if (selectedItem== 2) {
                        game_is_running = FALSE;
                    }}
				if(in_menu == 2){
				if (selectedItem== 0) {
                        //printf("Start Game selected\n");
                        in_menu = 0;
                    } else if (selectedItem== 1) {
                        showControls();
                    } else if (selectedItem== 2) {
                        in_menu =1;
						setup();
                    }else if(selectedItem==3){
						game_is_running = FALSE;
					}}
			
			
			}}
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
					SDL_SetWindowFullscreen(window, SDL_TRUE);
				}
				SDL_GetWindowSize(window,&WINDOW_WIDTH,&WINDOW_HEIGHT);
			}
			break;
	
	}

}



void update(){
	//Keeping a fixed timestep
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
		SDL_Delay(time_to_wait);
	}
	//while(!SDL_TICKS_PASSED(SDL_GetTicks(),last_frame_time+FRAME_TARGET_TIME));
	//using while uses the processor without actually doing something
	// difference of ticks from last frame converted to seconds
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
 
	last_frame_time = SDL_GetTicks();
	timer = end_time - last_frame_time;
	minutes = timer / 60000;
	seconds = (timer % 60000) / 1000;
	snprintf(timerText,sizeof(timerText),"%02d:%02d",minutes,seconds);
	// move the ball 20 pixels per second
	ball.x += 200 * delta_time * move_hori;
	ball.y += 200 * delta_time * move_vert;
}



void render(){
	//SDL_SetRenderDrawColor(renderer, <R> , <G> , <B> , <A>);
	SDL_SetRenderDrawColor(renderer, 0 , 0 , 0 , 255);
	SDL_RenderClear(renderer);
	SDL_Texture* characterTexture = LoadTexture("./img/charac.png",renderer);
	//TODO : HERE is where we can start drawing our game
	//Draw a rectangle
	SDL_Rect charac = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.width,
		(int)ball.height
	};
	//Fill the Rectangle and render it
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);

	//SDL_RenderFillRect(renderer, &charac);
	if(in_menu==0){
	SDL_RenderCopy(renderer, characterTexture, NULL, &charac);
	timerTexture = RenderText(timerText,font,timerColor,renderer);
	SDL_QueryTexture(timerTexture,NULL,NULL,&textWidth,&textHeight);
	SDL_Rect textRect = {10,10,textWidth,textHeight};
	//SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, timerTexture,NULL,&textRect);
	SDL_DestroyTexture(timerTexture);
	
	}else if(in_menu == 1){
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
	}else{
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
	}
	
	
	// swap the double buffer

	SDL_RenderPresent(renderer);
} 

void setup(){
	srand(time(NULL));
	end_time = 120000;
	timer = 0;
	ball.x = rand() % WINDOW_WIDTH + 1;
	//ball.x = 20;
	ball.y = rand() % WINDOW_HEIGHT + 1;
	//ball.y = 20;
	ball.width = 128;
	ball.height = 128;
	//TODO;
}

void destroy_window(){
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[]){

	game_is_running = initialize_window();

	setup();
	while(game_is_running){
		process_input();
		update();
		render();
	}

	destroy_window();


	return 0;}
