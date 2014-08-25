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


SU::Model cube;

class Floating
{
public:
	static std::list<Floating*> all;

	SU::Object obj;

	SU::Vector direction, destination;


	Floating(bool distort = false)
	{
		direction = SU::Vector(randDouble(), randDouble(), randDouble()).getNormalized();

		newDestination();
		obj.position = destination;
		newDestination();

		obj.model = &cube;

		obj.transforming = distort;
		obj.X = SU::Vector(1, randDouble(1) - 0.5, randDouble(1) - 0.5).getNormalized();
		obj.Y = SU::Vector(randDouble(1) - 0.5, 1, randDouble(1) - 0.5).getNormalized();
		obj.Z = SU::Vector(randDouble(1) - 0.5, randDouble(1) - 0.5, 1).getNormalized();

		all.push_back(this);
	}

	void newDestination()
	{
		destination = SU::Vector(randDouble(50.0) - 25.0, randDouble(20.0) - 10.0, randDouble(25) + 1);
	}
};
std::list<Floating*> Floating::all;


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

	SU::init(surface, SU::Flags::DEBUG_TRANSFORMATIONS);


	TTF_Font *font = TTF_OpenFont("./Deltoid-sans.ttf", 32);
	if (font == NULL)
		DIE(TTF_GetError());






	cube.add(new SU::Line(0, -0.5, 0,		0.5, 0, 0));
	cube.add(new SU::Line(0, -0.5, 0,		-0.5, 0, 0));
	cube.add(new SU::Line(0, -0.5, 0,		0, 0, 0.5));
	cube.add(new SU::Line(0, -0.5, 0,		0, 0, -0.5));

	cube.add(new SU::Line(-0.5, 0, 0,		0, 0, -0.5));
	cube.add(new SU::Line(-0.5, 0, 0,		0, 0, +0.5));
	cube.add(new SU::Line(0.5, 0, 0,		0, 0, -0.5));
	cube.add(new SU::Line(0.5, 0, 0,		0, 0, +0.5));

	cube.add(new SU::Line(0, 0.5, 0,		0.5, 0, 0));
	cube.add(new SU::Line(0, 0.5, 0,		-0.5, 0, 0));
	cube.add(new SU::Line(0, 0.5, 0,		0, 0, 0.5));
	cube.add(new SU::Line(0, 0.5, 0,		0, 0, -0.5));


	SU::Object aobject;
	aobject.model = &cube;
	aobject.position = SU::Vector(-0.5, -0.5, 1.5);

	aobject.transforming = true;
	aobject.X = SU::Vector(1, 0.25, 0);
	aobject.Y = SU::Vector(-0.25, 1, 0);

	SU::Object second = SU::Object();
	second.model = &cube;
	second.position = SU::Vector(2, 1, 0);

	second.transforming = true;
	second.X = SU::Vector(-0.5, 0, 0);
	second.Y = SU::Vector(0, -0.5, 0);
	second.Z = SU::Vector(0, 0, 0.5);

	aobject.addChild(&second);

	for (int i = 0; i < 300; i++)
	{
//		new Floating();
	}














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
						{
							running = false;
						}

						case SDLK_q:
						{
							running = false;
						}
						break;

						case SDLK_SPACE:
						{
							aobject.enabled = !aobject.enabled;
						}
						break;

						case SDLK_d:
						{
							SU::toggleFlag(SU::Flags::DEBUG_TRANSFORMATIONS);
						}
						break;

						case SDLK_LEFT:
						{
							SU::Camera::position.x -= 0.1;
						}
						break;

						case SDLK_RIGHT:
						{
							SU::Camera::position.x += 0.1;
						}
						break;

						case SDLK_DOWN:
						{
							SU::Camera::position.y -= 0.1;
						}
						break;

						case SDLK_UP:
						{
							SU::Camera::position.y += 0.1;
						}
						break;

						case SDLK_s:
						{
							SU::Camera::position.z -= 0.1;
						}
						break;

						case SDLK_w:
						{
							SU::Camera::position.z += 0.1;
						}
						break;

						case SDLK_KP_MINUS:
						{
							SU::Camera::FOV--;
						}
						break;

						case SDLK_KP_PLUS:
						{
							SU::Camera::FOV++;
						}

						default: break;
					}

				break;

				default: break;
			}
		}


		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));


		for (Floating* f : Floating::all)
		{
			f->direction = (f->direction + (f->destination - f->obj.position).getNormalized()).getNormalized() * 3;

			f->obj.position += f->direction * 0.01;

			if ((f->destination - f->obj.position).getLength() < 1)
				f->newDestination();
		}

		SU::render();

		SDL_Color c = {128, 128, 128};
		std::stringstream fps;
		// this FPS counter display the total average, not the current
		// will be changed
		fps << " " << int(1000.0 / (double(SDL_GetTicks()) / count)) << " Camera::FOV=" << SU::Camera::FOV;
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

		SDL_Delay(1);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}