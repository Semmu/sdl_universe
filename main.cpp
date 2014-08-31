#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <cmath>


#if USING_SDL1
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>
#endif

#include "SU.h"


#if 0
	#define WIDTH 1920
	#define HEIGHT 1080
	#define FLAGS SDL_WINDOW_FULLSCREEN_DESKTOP
#else
	#define WIDTH 1600
	#define HEIGHT 800
	#define FLAGS SDL_WINDOW_SHOWN
#endif

const double CAMERA_ROTATION_AMOUNT = 0.02;
const double CAMERA_MOVEMENT_AMOUNT = 0.07;

void DIE(const char* reason)
{
	#if USING_SDL1
		std::cerr << "[FATAL ERROR]: " << reason;
	#else
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
							 "FATAL ERROR",
							 reason,
							 NULL);
	#endif

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
		destination = SU::Vector(randDouble(30.0) - 15.0, randDouble(20.0) - 10.0, randDouble(10) + 1);
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

#if USING_SDL1
std::list<int> pressed_down_keys;
#else
std::list<SDL_Keycode> pressed_down_keys;
#endif

int main( int argc, char* args[] )
{
	int currentSec = 0;
	int currentSecFPS = 0;
	int previousSecFPS = 0;

	SDL_Init(SDL_INIT_VIDEO);
	if(TTF_Init() == -1)
		DIE(TTF_GetError());
	atexit(SDL_Quit);

	bool running = true;


	#if USING_SDL1
		SDL_Surface *surface = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_ANYFORMAT);
	#else
		SDL_Window* 	window;
		SDL_Renderer* 	renderer;
		SDL_Texture* 	texture;
		SDL_Surface* 	surface;

		SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, FLAGS, &window, &renderer);

		texture = SDL_CreateTexture(renderer,
								   SDL_PIXELFORMAT_ARGB8888,
								   SDL_TEXTUREACCESS_STREAMING,
								   WIDTH, HEIGHT);

		surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32,
											0x00FF0000,
											0x0000FF00,
											0x000000FF,
											0xFF000000);
	#endif

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
	center.X = SU::Vector(0.1, 0, 0);
	center.Y = SU::Vector(0, 0.1, 0);
	center.Z = SU::Vector(0, 0, 0.1);

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
		new Floating(false);
	}
	bool move = false;


	SU::Camera::position = SU::Vector(1, 0, -20);


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
					#if USING_SDL1
					if (true)
					#else
					if (e.key.repeat == 0)
					#endif
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

		#if USING_SDL1
		for(int k : pressed_down_keys)
		#else
		for(SDL_Keycode k : pressed_down_keys)
		#endif
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
					SU::Camera::position -= SU::Camera::rightDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_RIGHT:
				{
					SU::Camera::position += SU::Camera::rightDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_DOWN:
				{
					SU::Camera::position -= SU::Camera::lookDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_UP:
				{
					SU::Camera::position += SU::Camera::lookDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_PAGEUP:
				{
					SU::Camera::position += SU::Camera::upDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_PAGEDOWN:
				{
					SU::Camera::position -= SU::Camera::upDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
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
					SU::Camera::pitch(CAMERA_ROTATION_AMOUNT);
				break;

				case SDLK_s:
					SU::Camera::pitch(-CAMERA_ROTATION_AMOUNT);
				break;

				case SDLK_a:
					SU::Camera::yaw(-CAMERA_ROTATION_AMOUNT);
				break;

				case SDLK_d:
					SU::Camera::yaw(CAMERA_ROTATION_AMOUNT);
				break;

				case SDLK_q:
					SU::Camera::roll(CAMERA_ROTATION_AMOUNT);
				break;

				case SDLK_e:
					SU::Camera::roll(-CAMERA_ROTATION_AMOUNT);
				break;

				default: break;
			}
		}

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

		if (move)
			for (Floating* f : Floating::all)
				f->move();

		SU::render();

		if (SDL_GetTicks() / 1000 != currentSec)
		{
			currentSec++;
			previousSecFPS = currentSecFPS;
			currentSecFPS = 0;
		}
		currentSecFPS++;

		SDL_Color c = {128, 128, 128};
		std::stringstream fps;
		fps << " " << previousSecFPS << " FPS     " << SU::primitivesRendered;
		SDL_Surface *text = TTF_RenderText_Solid(font, fps.str().c_str(), c);
		if (text == NULL)
			DIE(TTF_GetError());

		SDL_BlitSurface(text, NULL, surface, NULL);




		#if USING_SDL1
			SDL_Flip(surface);
		#else
			// TODO:
			// which is faster? this method or SDL_CreateSoftwareRenderer?
			// or CreateTextureFromSurface() ???
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
			SDL_FreeSurface(text);

			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		#endif

		SDL_Delay(10);
	}

	return 0;
}