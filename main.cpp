#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <cmath>

#if 0
	#define WIDTH 1920
	#define HEIGHT 1080
#else
	#define WINDOWED
	#define WIDTH 1200
	#define HEIGHT 700
#endif

#if USING_SDL1
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
	#ifdef WINDOWED
		#define FLAGS SDL_ANYFORMAT
	#else
		#define FLAGS SDL_ANYFORMAT | SDL_FULLSCREEN
	#endif
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>
	#ifdef WINDOWED
		#define FLAGS SDL_WINDOW_SHOWN
	#else
		#define FLAGS SDL_WINDOW_FULLSCREEN_DESKTOP
	#endif
#endif

#include "SU.h"

void DIE(const char* reason)
{
	#if USING_SDL1
		std::cerr << std::endl << "[FATAL ERROR]: " << reason << std::endl << std::endl;
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

#if USING_SDL1
std::list<int> pressed_down_keys;
#else
std::list<SDL_Keycode> pressed_down_keys;
#endif







int main( int argc, char* args[] )
{
	Uint32 currentSec = 0;
	Uint32 currentSecFPS = 0;
	Uint8 previousSecFPS = 0;

	SDL_Init(SDL_INIT_VIDEO);
	if(TTF_Init() == -1)
		DIE(TTF_GetError());
	atexit(SDL_Quit);

	bool running = true;
	bool fps_visible = true;
	bool debug_text_visible = true;
	std::stringstream text;


	#if USING_SDL1
		SDL_Surface *surface = SDL_SetVideoMode(WIDTH, HEIGHT, 32, FLAGS);
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

	SDL_ShowCursor(SDL_DISABLE);
	#if USING_SDL1
	SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
	#else
	SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
	#endif

	SU::init(surface);
	SU::setFlag(SU::Flags::LIGHTING);


	TTF_Font *font = TTF_OpenFont("./Dina_r400-10.bdf", 0);
	if (font == NULL)
		DIE(TTF_GetError());



	while (running)
	{
		if (SDL_GetTicks() / 1000 != currentSec)
		{
			currentSec++;
			previousSecFPS = currentSecFPS;
			currentSecFPS = 0;
		}

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
						switch (e.key.keysym.sym)
						{
							case SDLK_ESCAPE:
							case SDLK_q:
								running = false;
							break;

							case SDLK_F12:
								debug_text_visible = !debug_text_visible;
							break;

							case SDLK_F1:
								SU::toggleFlag(SU::Flags::DEBUG_WIREFRAMING);
							break;

							case SDLK_F2:
								SU::toggleFlag(SU::Flags::DEBUG_TRANSFORMATIONS);
							break;

							case SDLK_F3:
								SU::toggleFlag(SU::Flags::DEBUG_TRANSLATIONS);
							break;

							case SDLK_F4:
								SU::toggleFlag(SU::Flags::ONLY_FACING_TRIANGLES);
							break;

							case SDLK_F5:
								SU::toggleFlag(SU::Flags::DEPTH_SORT);
							break;

							case SDLK_F6:
								SU::toggleFlag(SU::Flags::LIGHTING);
							break;

							default:
								pressed_down_keys.push_back(e.key.keysym.sym);
							break;
						}
					}
				}
				break;

				case SDL_MOUSEMOTION:
				{
					int xdiff = e.motion.x - WIDTH / 2;
					int ydiff = e.motion.y - HEIGHT / 2;

					if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1))
					{
						SU::Camera::position -= SU::Camera::upDirection / 100 * ydiff;
						SU::Camera::position += SU::Camera::rightDirection / 100 * xdiff;
					}
					else
					{
						SU::Camera::yaw(xdiff * 0.001);
						SU::Camera::pitch(ydiff * 0.0001);
					}

					#if USING_SDL1
					SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
					#else
					SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
					#endif
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
				default: break;
			}
		}

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

		SU::render();
		currentSecFPS++;

		SDL_Color c = {150, 150, 150};
		if (fps_visible)
		{
			text.str(std::string());
			text.clear();

			text << (int)previousSecFPS;

			SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.str().c_str(), c);

			if (textSurface != NULL)
			{
				SDL_Rect r;
				r.x = 10;
				r.y = 10;
				SDL_BlitSurface(textSurface, NULL, surface, &r);
				SDL_FreeSurface(textSurface);
			}
		}

		if(debug_text_visible)
		{
			text.str(std::string());
			text.clear();

			text << "F1 [" << (SU::hasFlag(SU::Flags::DEBUG_WIREFRAMING) ? '+' : ' ' ) << "] DEBUG_WIREFRAMING" << std::endl <<
					"F2 [" << (SU::hasFlag(SU::Flags::DEBUG_TRANSFORMATIONS) ? '+' : ' ' ) << "] DEBUG_TRANSFORMATIONS" << std::endl <<
					"F3 [" << (SU::hasFlag(SU::Flags::DEBUG_TRANSLATIONS) ? '+' : ' ' ) << "] DEBUG_TRANSLATIONS" << std::endl <<
					"F4 [" << (SU::hasFlag(SU::Flags::ONLY_FACING_TRIANGLES) ? '+' : ' ' ) << "] ONLY_FACING_TRIANGLES" << std::endl <<
					"F5 [" << (SU::hasFlag(SU::Flags::DEPTH_SORT) ? '+' : ' ' ) << "] DEPTH_SORT" << std::endl <<
					"F6 [" << (SU::hasFlag(SU::Flags::LIGHTING) ? '+' : ' ' ) << "] LIGHTING" << std::endl <<
					"F7 [" << (SU::hasFlag(SU::Flags::DEBUG_BUMPMAP) ? '+' : ' ' ) << "] BUMPMAP";

			std::string line;
			SDL_Rect r;
			r.x = 10;
			r.y = surface->h - 40;
			while(std::getline(text, line, '\n'))
			{
				SDL_Surface *textSurface = TTF_RenderText_Solid(font, line.c_str(), c);
				if (textSurface != NULL)
				{
					SDL_BlitSurface(textSurface, NULL, surface, &r);
					SDL_FreeSurface(textSurface);
				}

				r.y -= 30;
			}
		}

		#if USING_SDL1
			SDL_Flip(surface);
		#else
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		#endif

		SDL_Delay(1);
	}

	return 0;
}