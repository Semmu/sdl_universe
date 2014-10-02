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
	#define WIDTH 1600
	#define HEIGHT 800
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



const double CAMERA_ROTATION_AMOUNT = 0.005;
const double CAMERA_MOVEMENT_AMOUNT = 1;

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


SU::Model house;


#if USING_SDL1
std::list<int> pressed_down_keys;
#else
std::list<SDL_Keycode> pressed_down_keys;
#endif


/*
	ORIGO KORULI FORGASHOZ
 */

#define AUTO_ROTATION_AMOUNT 0.002

bool cameraAutoRotating = true;
double autoRotationAmount = AUTO_ROTATION_AMOUNT;
double distance = 0.5;
double heightAngle = 0.5;
double rotationAngle = 0;






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
	SU::setFlag(SU::Flags::DEBUG_TRANSFORMATIONS);


	TTF_Font *font = TTF_OpenFont("./Instruction.ttf", 20);
	if (font == NULL)
		DIE(TTF_GetError());





	// front
	house.add(new SU::Triangle(0, 0, 0,		1, 0, 0,	0, 1, 0,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(1, 1, 0,		0, 1, 0,	1, 0, 0,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(0, 1, 0,		1, 1, 0,	0.5, 1.5, 0.5,	SU::mapColor(150, 20, 20), true));

	// right
	house.add(new SU::Triangle(1, 0, 0,		1, 1, 1,	1, 1, 0,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(1, 0, 0,		1, 0, 1,	1, 1, 1,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(1, 1, 0,		1, 1, 1,	0.5, 1.5, 0.5,	SU::mapColor(150, 20, 20), true));

	// left
	house.add(new SU::Triangle(0, 0, 0,		0, 1, 0,	0, 1, 1,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(0, 0, 0,		0, 1, 1,	0, 0, 1,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(0, 1, 1,		0, 1, 0,	0.5, 1.5, 0.5,	SU::mapColor(150, 20, 20), true));

	// back
	house.add(new SU::Triangle(0, 0, 1,		0, 1, 1,	1, 1, 1,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(0, 0, 1,		1, 1, 1,	1, 0, 1,	SU::mapColor(200, 200, 50), true));
	house.add(new SU::Triangle(1, 1, 1,		0, 1, 1,	0.5, 1.5, 0.5,	SU::mapColor(150, 20, 20), true));

	// bottom
	house.add(new SU::Triangle(0, 0, 0,		0, 0, 1,	1, 0, 0,	SU::mapColor(10, 20, 50)));
	house.add(new SU::Triangle(1, 0, 1,		1, 0, 0,	0, 0, 1,	SU::mapColor(10, 20, 50)));


	SU::Object hazak, haz1, haz2, haz3, haz4;

	haz1.model = &house;
	haz2.model = &house;
	haz3.model = &house;
	haz4.model = &house;

	haz1.transforming = true;
	haz2.transforming = true;
	haz2.rotateAroundZ(M_PI / 2);
	haz3.transforming = true;
	haz3.rotateAroundZ(M_PI);
	haz4.transforming = true;
	haz4.rotateAroundZ(M_PI / -2);

	haz2.position = SU::Vector(3, 0, 0);
	haz3.position = SU::Vector(3, 3, 0);
	haz4.position = SU::Vector(0, 3, 0);

	hazak.addChild(&haz1);
	hazak.addChild(&haz2);
	hazak.addChild(&haz3);
	hazak.addChild(&haz4);

	SU::Object global;
	hazak.transforming = true;
	global.addChild(&hazak);
	global.transforming = true;
	hazak.position = SU::Vector(0.1, 0.1, 0.1);
	hazak.scale(0.3);

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
								running = false;
							break;

							case SDLK_0:
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

							case SDLK_LEFT:
							{
								if (cameraAutoRotating)
								{
									autoRotationAmount -= AUTO_ROTATION_AMOUNT;
								}
								else
								{
									pressed_down_keys.push_back(SDLK_LEFT);
								}
							}
							break;

							case SDLK_RIGHT:
							{
								if (cameraAutoRotating)
								{
									autoRotationAmount += AUTO_ROTATION_AMOUNT;
								}
								else
								{
									pressed_down_keys.push_back(SDLK_RIGHT);
								}
							}
							break;

							case SDLK_RETURN:
							{
								if (cameraAutoRotating)
								{
									SU::Camera::position = SU::Vector(0, 0, -30);
									SU::Camera::lookDirection = SU::Vector(0, 0, 1);
									SU::Camera::upDirection = SU::Vector(0, 1, 0);

									cameraAutoRotating = false;
								}
								else
								{
									cameraAutoRotating = true;
								}
							}
							break;

							case SDLK_r:
							{
								global.X = SU::Vector(1, 0, 0).rotated(SU::Vector(0, 1, 0), randDouble(0.8) - 0.4);
								global.Y = SU::Vector(0, 1, 0).rotated(SU::Vector(0, 0, 1), randDouble(0.8) - 0.4);
								global.Z = SU::Vector(0, 0, 1).rotated(SU::Vector(1, 0, 0), randDouble(0.8) - 0.4);
							}
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
						SU::Camera::pitch(ydiff * 0.001);
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
					if (cameraAutoRotating)
					{
						distance += 0.01;
					}
					else
					{
						SU::Camera::position -= SU::Camera::lookDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
					}
				}
				break;

				case SDLK_UP:
				{
					if (cameraAutoRotating)
					{
						distance -= 0.01;
					}
					else
					{
						SU::Camera::position += SU::Camera::lookDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
					}
				}
				break;

				case SDLK_RSHIFT:
				{
					if (cameraAutoRotating)
					{
						heightAngle -= 0.01;
					}
					else
					{
						SU::Camera::position += SU::Camera::upDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
					}
				}
				break;

				#if USING_SDL1
				case SDLK_KP1:
				#else
				case SDLK_KP_1:
				#endif
				{
					if (cameraAutoRotating)
					{
						heightAngle += 0.01;
					}
					else
					{
						SU::Camera::position -= SU::Camera::upDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
					}
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

		if (cameraAutoRotating)
		{
			SU::Camera::position = SU::Vector(0, 0, -10 * distance).rotated(SU::Vector(1, 0, 0), heightAngle).rotated(SU::Vector(0, 1, 0), rotationAngle);
			SU::Camera::upDirection = SU::Vector(0, 1, 0).rotated(SU::Vector(1, 0, 0), heightAngle).rotated(SU::Vector(0, 1, 0), rotationAngle);
			SU::Camera::lookDirection = -SU::Camera::position;
			rotationAngle += autoRotationAmount;
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
					"F6 [" << (SU::hasFlag(SU::Flags::LIGHTING) ? '+' : ' ' ) << "] LIGHTING";

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
			// TODO:
			// which is faster? this method or SDL_CreateSoftwareRenderer?
			// or CreateTextureFromSurface() ???
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		#endif

		//SDL_Delay(1);
	}

	return 0;
}