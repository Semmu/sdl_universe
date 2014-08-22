#include <SDL2/SDL.h>
#include <string>
#include <iostream>

#define DIE _DIE();
void _DIE()
{
	std::cerr << "[FATAL ERROR]: " << SDL_GetError();
	exit(13);
}

int main( int argc, char* args[] )
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);

	SDL_Window* window = SDL_CreateWindow("First SDL2 try",
										  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										  640, 480,
										  SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

	if (window == NULL)
		DIE

	SDL_Delay(1000);



	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}