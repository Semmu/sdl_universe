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



const double CAMERA_ROTATION_AMOUNT = 0.01;
const double CAMERA_MOVEMENT_AMOUNT = 0.1;

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


SU::Model cube, house;

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

		obj.model = &house;

		obj.transforming = distort;
		obj.X = SU::Vector(1, randDouble(1) - 0.5, randDouble(1) - 0.5).getNormalized();
		obj.Y = SU::Vector(randDouble(1) - 0.5, 1, randDouble(1) - 0.5).getNormalized();
		obj.Z = SU::Vector(randDouble(1) - 0.5, randDouble(1) - 0.5, 1).getNormalized();

		all.push_back(this);
	}

	void newDestination()
	{
		const double radius = 20;
		destination = SU::Vector(randDouble(2.0) - 1.0, randDouble(2.0) - 1.0, randDouble(2.0) - 1.0).getNormalized() * radius;
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
/*	std::ofstream of;
	of.open("randomvectors");
	#define NUM 3
	SU::Vector vs[NUM];
	for(int i = 0; i < NUM; i++)
	{
		vs[i] = SU::Vector(randDouble(), randDouble(), randDouble());
		vs[i].save(of);
	}

	of.close();

	std::ifstream iff;
	iff.open("randomvectors");
	SU::Vector ivs[NUM];
	for(int i = 0; i < NUM; i++)
		ivs[i] = SU::Vector::load(iff);

	for(int i = 0; i < NUM; i++)
	{
		std::cout << vs[i] << std::endl << ivs[i] << std::endl;
	}*/


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
		SDL_ShowCursor(SDL_DISABLE);
		SDL_Surface *surface = SDL_SetVideoMode(WIDTH, HEIGHT, 32, FLAGS);
		SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
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

	SU::init(surface);
	SU::setFlag(SU::Flags::LIGHTING);


	TTF_Font *font = TTF_OpenFont("./Instruction.ttf", 20);
	if (font == NULL)
		DIE(TTF_GetError());






	cube.add(new SU::Segment(0, -0.5, 0,	0.5, 0, 0));
	cube.add(new SU::Segment(0, -0.5, 0,	-0.5, 0, 0));
	cube.add(new SU::Segment(0, -0.5, 0,	0, 0, 0.5));
	cube.add(new SU::Segment(0, -0.5, 0,	0, 0, -0.5));

	cube.add(new SU::Segment(-0.5, 0, 0,	0, 0, -0.5));
	cube.add(new SU::Segment(-0.5, 0, 0,	0, 0, +0.5));
	cube.add(new SU::Segment(0.5, 0, 0,		0, 0, -0.5));
	cube.add(new SU::Segment(0.5, 0, 0,		0, 0, +0.5));

	cube.add(new SU::Segment(0, 0.5, 0,		0.5, 0, 0));
	cube.add(new SU::Segment(0, 0.5, 0,		-0.5, 0, 0));
	cube.add(new SU::Segment(0, 0.5, 0,		0, 0, 0.5));
	cube.add(new SU::Segment(0, 0.5, 0,		0, 0, -0.5));





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





//	SU::Object origo;
//	origo.model = &cube;

	Floating *f = NULL, *ff = NULL, *fff = NULL, *ffff = NULL, *fffff = NULL;

	for (int i = 0; i < 100; i++)
	{
		f = new Floating(false);
/*		for (int i = 0; i < 4; i++)
		{
			ff = new Floating(true);
			ff->obj.X = SU::Vector(0.5, 0, 0);
			ff->obj.Y = SU::Vector(0, 0.5, 0);
			ff->obj.Z = SU::Vector(0, 0, 0.5);
			f->obj.addChild(&(ff->obj));

			for (int i = 0; i < 4; i++)
			{
				fff = new Floating(true);
				fff->obj.X = SU::Vector(0.5, 0, 0);
				fff->obj.Y = SU::Vector(0, 0.5, 0);
				fff->obj.Z = SU::Vector(0, 0, 0.5);
				ff->obj.addChild(&(fff->obj));

				for (int i = 0; i < 4; i++)
				{
					ffff = new Floating(true);
					ffff->obj.X = SU::Vector(0.5, 0, 0);
					ffff->obj.Y = SU::Vector(0, 0.5, 0);
					ffff->obj.Z = SU::Vector(0, 0, 0.5);
					fff->obj.addChild(&(ffff->obj));

/*					for (int i = 0; i < 3; i++)
					{
						fffff = new Floating(true);
						fffff->obj.X = SU::Vector(0.5, 0, 0);
						fffff->obj.Y = SU::Vector(0, 0.5, 0);
						fffff->obj.Z = SU::Vector(0, 0, 0.5);
						ffff->obj.addChild(&(fffff->obj));
					}*/
//				}
//			}
//		}
	}
	bool move = false;

	SU::Camera::position.z = -30;






	// SHIP HERE

	SU::Model hajtomu;
	const int reszletesseg = 6;
	for (int i = 0; i < reszletesseg; i++)
	{
		SU::Vector a(0, 1, 0), b(-2, 0.5, 0), x(1, 0, 0);
		hajtomu.add(new SU::Quad(b.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 a.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 a.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
								 b.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(100, 100, 100), true));

		SU::Vector aa(-2, 0.5, 0), bb(-1.9, 0.2, 0);
		hajtomu.add(new SU::Quad(bb.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 aa.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 aa.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
								 bb.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(250, 50, 50), false));

		SU::Vector aaa(-1.9, 0.2, 0), bbb(-3, 0, 0);
		hajtomu.add(new SU::Quad(bbb.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 aaa.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 aaa.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
								 bbb.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(250, 250, 50), false));

		SU::Vector c(0, 1, 0), d(1, 0.8, 0);
		hajtomu.add(new SU::Quad(c.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 d.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 d.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
								 c.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(100, 100, 100), true));

		SU::Vector cc(1, 0.8, 0), dd(1.1, 0.7, 0);
		hajtomu.add(new SU::Quad(cc.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 dd.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 dd.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
								 cc.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(50, 100, 220), false));

		SU::Vector ccc(1.1, 0.7, 0), ddd(0.5, 0, 0);
		hajtomu.add(new SU::Quad(ccc.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 ddd.rotated(x, M_PI / reszletesseg * 2 * (i)),
								 ddd.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
								 ccc.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(30, 30, 30), true));
	}

	SU::Model torzs;
	torzs.add(new SU::Quad(SU::Vector(3, 0, 0), SU::Vector(3, 0, 1), SU::Vector(0, 1, 1), SU::Vector(0, 1, 0), SU::mapColor(100, 100, 100), true));
	torzs.add(new SU::Quad(SU::Vector(3, 0, 0), SU::Vector(3.2, -0.5, 0), SU::Vector(3.2, -0.5, 1), SU::Vector(3, 0, 1), SU::mapColor(100, 100, 100), true));
	torzs.add(new SU::Quad(SU::Vector(3, 0, 1), SU::Vector(3.2, -0.5, 1), SU::Vector(0, -0.5, 2), SU::Vector(0, 0, 2), SU::mapColor(100, 100, 100), true));
	torzs.add(new SU::Triangle(SU::Vector(3, 0, 1), SU::Vector(0, 0, 2), SU::Vector(0, 1, 1), SU::mapColor(100, 100, 100), true));


	SU::Object torzsObject;
	torzsObject.model = &torzs;
	SU::Object torzsTukor;
	torzsTukor.model = &torzs;
	torzsTukor.transforming = true;
	torzsTukor.Z.z = -1;




	SU::Object ship, h1, h2, h3, h4;

	h1.model = &hajtomu;
	h1.position = SU::Vector(0, 3, 4);
	h2.model = &hajtomu;
	h2.position = SU::Vector(0, 3, -4);
	h3.model = &hajtomu;
	h3.position = SU::Vector(0, -3, -4);
	h4.model = &hajtomu;
	h4.position = SU::Vector(0, -3, 4);

	ship.addChild(&h1);
	ship.addChild(&h2);
	ship.addChild(&h3);
	ship.addChild(&h4);
	ship.addChild(&torzsObject);







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

							case SDLK_SPACE:
								move = !move;
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

					SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
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
					SU::Camera::position -= SU::Camera::lookDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_UP:
				{
					SU::Camera::position += SU::Camera::lookDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				case SDLK_RSHIFT:
				{
					SU::Camera::position += SU::Camera::upDirection.getNormalized() * CAMERA_MOVEMENT_AMOUNT;
				}
				break;

				#if USING_SDL1
				case SDLK_KP1:
				#else
				case SDLK_KP_1:
				#endif
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