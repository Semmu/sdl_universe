#include <SDL2/SDL.h>
#include <string>
#include <iostream>

#define WIDTH 1600
#define HEIGHT 800

void DIE(const char* reason)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                         "FATAL ERROR",
                         reason,
                         NULL);

	exit(1);
}

void DIE()
{
	DIE(SDL_GetError());
}

int main( int argc, char* args[] )
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);


	SDL_Window* 	window;
	SDL_Renderer* 	renderer;
	SDL_Texture* 	texture;
	SDL_Surface* 	surface;

	SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);

	texture = SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               WIDTH, HEIGHT);

	surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);

	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, rand(), rand(), rand()));

	SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);


	if (window == NULL)
		DIE();

	SDL_Delay(1000);



	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}