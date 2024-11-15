#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

int game_is_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;
int move_hori = 0;
int move_vert = 0;

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
		NULL, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS
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
	return TRUE;

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
		case SDL_KEYDOWN:
			if( event.key.keysym.sym == SDLK_ESCAPE){
				game_is_running = FALSE;
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
			
			break;
		case SDL_KEYUP:
			if(event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT){
				move_hori = 0;
			}
			if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN){
				move_vert = 0;
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

	// move the ball 20 pixels per second
	ball.x += 50 * delta_time * move_hori;
	ball.y += 50 * delta_time * move_vert;
}



void render(){
	//SDL_SetRenderDrawColor(renderer, <R> , <G> , <B> , <A>);
	SDL_SetRenderDrawColor(renderer, 0 , 0 , 0 , 255);
	SDL_RenderClear(renderer);

	//TODO : HERE is where we can start drawing our game
	//Draw a rectangle
	SDL_Rect ball_rect = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.width,
		(int)ball.height
	};
	//Fill the Rectangle and render it
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderFillRect(renderer, &ball_rect);

	// swap the double buffer
	SDL_RenderPresent(renderer);
} 

void setup(){
	ball.x = 20;
	ball.y = 20;
	ball.width = 15;
	ball.height = 15;
	//TODO;
}

void destroy_window(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(){

	game_is_running = initialize_window();

	setup();
	while(game_is_running){
		process_input();
		update();
		render();
	}

	destroy_window();


	return 0;}
