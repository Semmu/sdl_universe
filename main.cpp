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

	void move()
	{
		direction = direction + (destination - obj.position).getNormalized();

		obj.position += direction.getNormalized() / 20;

		if ((destination - obj.position).getLength() < 1)
			newDestination();
	}
};
std::list<Floating*> Floating::all;

std::list<SDL_Keycode> pressed_down_keys;

int main( int argc, char* args[] )
{
	std::cout << (SU::Vector(0, 1, 0).crossProduct(SU::Vector(0, 0, 1))) << std::endl;

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






	cube.add(new SU::Segment(0, -0.5, 0,		0.5, 0, 0));
	cube.add(new SU::Segment(0, -0.5, 0,		-0.5, 0, 0));
	cube.add(new SU::Segment(0, -0.5, 0,		0, 0, 0.5));
	cube.add(new SU::Segment(0, -0.5, 0,		0, 0, -0.5));

	cube.add(new SU::Segment(-0.5, 0, 0,		0, 0, -0.5));
	cube.add(new SU::Segment(-0.5, 0, 0,		0, 0, +0.5));
	cube.add(new SU::Segment(0.5, 0, 0,		0, 0, -0.5));
	cube.add(new SU::Segment(0.5, 0, 0,		0, 0, +0.5));

	cube.add(new SU::Segment(0, 0.5, 0,		0.5, 0, 0));
	cube.add(new SU::Segment(0, 0.5, 0,		-0.5, 0, 0));
	cube.add(new SU::Segment(0, 0.5, 0,		0, 0, 0.5));
	cube.add(new SU::Segment(0, 0.5, 0,		0, 0, -0.5));


	SU::Object center;
	center.model = &cube;
	center.transforming = true;
	center.X = SU::Vector(0.2, 0, 0);
	center.Y = SU::Vector(0, 0.2, 0);
	center.Z = SU::Vector(0, 0, 0.2);

	SU::Object aobject;
	aobject.model = &cube;
	aobject.position = SU::Vector(0.5, -0.5, 1.5);

	aobject.transforming = true;
	aobject.X = SU::Vector(1, 0.25, 0);
	aobject.Y = SU::Vector(-0.25, 1, 0);

	SU::Object second = SU::Object();
	second.model = &cube;
	second.position = SU::Vector(1, 0.5, 0);

	second.transforming = true;
	second.X = SU::Vector(-0.25, 0, 0);
	second.Y = SU::Vector(0, -0.25, 0);
	second.Z = SU::Vector(0, 0, 0.25);

	aobject.addChild(&second);

	for (int i = 0; i < 300; i++)
	{
		//new Floating();
	}
	bool move = false;


	SU::Camera::position = SU::Vector(0, 0, -10);


	while (running)
	{
		SDL_Event e;

		while(SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
				case SDL_QUIT:
				{
					running = false;
				}
				break;

				case SDL_KEYUP:
				{
					pressed_down_keys.remove(e.key.keysym.sym);
				}
				break;

				case SDL_KEYDOWN:
				{
					if (e.key.repeat == 0)
					{
						if (e.key.keysym.sym == SDLK_SPACE)
							move = !move;
						else
							pressed_down_keys.push_back(e.key.keysym.sym);
					}
				}
				break;

				default: break;
			}
		}

		for(SDL_Keycode k : pressed_down_keys)
		{
			switch(k)
			{
				case SDLK_ESCAPE:
				{
					running = false;
				}
				break;

				case SDLK_h:
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
					SU::Camera::position.z -= 0.1;
				}
				break;

				case SDLK_UP:
				{
					SU::Camera::position.z += 0.1;
				}
				break;

				case SDLK_PAGEUP:
				{
					SU::Camera::position.y += 0.1;
				}
				break;

				case SDLK_PAGEDOWN:
				{
					SU::Camera::position.y -= 0.1;
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
				break;

				case SDLK_i:
				{
					aobject.rotateAroundX(0.1);
				}
				break;

				case SDLK_k:
				{
					aobject.rotateAroundX(-0.1);
				}
				break;

				case SDLK_j:
				{
					aobject.rotateAroundY(0.1);
				}
				break;

				case SDLK_l:
				{
					aobject.rotateAroundY(-0.1);
				}
				break;

				case SDLK_u:
				{
					aobject.rotateAroundZ(0.1);
				}
				break;

				case SDLK_o:
				{
					aobject.rotateAroundZ(-0.1);
				}
				break;

				case SDLK_w:
					SU::Camera::pitch(0.01);
				break;

				case SDLK_s:
					SU::Camera::pitch(-0.01);
				break;

				default: break;
			}
		}

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

		if (move)
			for (Floating* f : Floating::all)
				f->move();

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

		SDL_Delay(10);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}