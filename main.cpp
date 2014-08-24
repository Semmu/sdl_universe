#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SU.h"


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
	int count = 0;

	SDL_Init(SDL_INIT_VIDEO);
	if(TTF_Init() == -1)
		DIE(TTF_GetError());
	atexit(SDL_Quit);


	SDL_Window* 	window;
	SDL_Renderer* 	renderer;
	SDL_Texture* 	texture;
	SDL_Surface* 	surface;

	bool running = true;

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

	SU::init(surface);


	TTF_Font *font = TTF_OpenFont("./Deltoid-sans.ttf", 32);
	if (font == NULL)
		DIE(TTF_GetError());





	SU::Model cube;

	cube.add(new SU::Line(0.5, 0, 0.5,		0.5, 0.5, 0));
	cube.add(new SU::Line(0.5, 0, 0.5,		0, 0.5, 0.5));
	cube.add(new SU::Line(0.5, 0, 0.5,		0.5, 0.5, 1));
	cube.add(new SU::Line(0.5, 0, 0.5,		1, 0.5, 0.5));

	cube.add(new SU::Line(0.5, 1, 0.5,		0.5, 0.5, 0));
	cube.add(new SU::Line(0.5, 1, 0.5,		0, 0.5, 0.5));
	cube.add(new SU::Line(0.5, 1, 0.5,		0.5, 0.5, 1));
	cube.add(new SU::Line(0.5, 1, 0.5,		1, 0.5, 0.5));

	cube.add(new SU::Line(0.5, 0.5, 0,		0, 0.5, 0.5));
	cube.add(new SU::Line(0.5, 0.5, 0,		1, 0.5, 0.5));
	cube.add(new SU::Line(0.5, 0.5, 1,		0, 0.5, 0.5));
	cube.add(new SU::Line(0.5, 0.5, 1,		1, 0.5, 0.5));


	SU::Object aobject;
	aobject.model = &cube;

	aobject.position = SU::Vector(1, -0.5, 1);

	aobject.transforming = true;














	while (running)
	{
		SDL_Event e;

		while(SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
				case SDL_QUIT:
					running = false;
				break;

				case SDL_KEYDOWN:

					switch(e.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = false;
						break;

						case SDLK_SPACE:
							aobject.enabled = !aobject.enabled;
						break;

						case SDLK_LEFT:
						{
							aobject.position.x -= 0.1;
						}
						break;

						case SDLK_RIGHT:
						{
							aobject.position.x += 0.1;
						}
						break;

						case SDLK_DOWN:
						{
							aobject.position.y -= 0.1;
						}
						break;

						case SDLK_UP:
						{
							aobject.position.y += 0.1;
						}
						break;

						case SDLK_s:
						{
							aobject.position.z -= 0.1;
						}
						break;

						case SDLK_w:
						{
							aobject.position.z += 0.1;
						}
						break;

						default: break;
					}

				break;

				default: break;
			}
		}


		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));


		SU::render();

		SDL_Color c = {128, 128, 128};
		std::stringstream fps;
		// this FPS counter display the total average, not the current
		// will be changed
		fps << " " << int(1000.0 / (double(SDL_GetTicks()) / count));
		SDL_Surface *text = TTF_RenderText_Solid(font, fps.str().c_str(), c);
		if (text == NULL)
			DIE(TTF_GetError());

		SDL_BlitSurface(text, NULL, surface, NULL);

		count++;



		// TODO:
		// which is faster? this method or SDL_CreateSoftwareRenderer?
		// or CreateTextureFromSurface() ???
		SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
		SDL_FreeSurface(text);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);


	}

	std::cout << SDL_GetTicks() << " ms / " << count << " frames = " << int(double(SDL_GetTicks()) / count) << " ms average per frame" <<std::endl;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}