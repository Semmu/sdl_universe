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

bool line(SDL_Surface* dst, int x1, int y1, int x2, int y2, int color)
{
	SDL_Rect rect;
	rect.h = 1;
	rect.w = 1;

	if (x1 == x2)
	{
		rect.x = x1;

		for (int c = (y1 > y2 ? y2 : y1); c < (y1 > y2 ? y1 : y2) + 1; c++)
		{
			rect.y = c;
			SDL_FillRect(dst, &rect, color);
		}

		return true;
	}

	if (y1 == y2)
	{
		rect.y = y1;

		for (int c = (x1 > x2 ? x2 : x1); c < (x1 > x2 ? x1 : x2) + 1; c++)
		{
			rect.x = c;
			SDL_FillRect(dst, &rect, color);
		}

		return true;
	}


	double meredekseg = ( double(y1) - double(y2) ) / (double(x2) - double(x1));

	bool xdir = (meredekseg <= 1 && meredekseg >= -1 ? true : false);
	bool ydir = !xdir;

	if (xdir)
	{
		if (x1 > x2)
		{
			int temp;
			temp = x2;
			x2 = x1;
			x1 = temp;

			temp = y2;
			y2 = y1;
			y1 = temp;
		}

		for (int c = x1 ; c < x2 ; c++)
		{
			rect.x = c;
			rect.y = y1 - meredekseg * (c - x1);
			SDL_FillRect(dst, &rect, color);
		}
	}
	else
	{
		if (y1 > y2)
		{
			int temp;
			temp = x2;
			x2 = x1;
			x1 = temp;

			temp = y2;
			y2 = y1;
			y1 = temp;
		}

		for (int c = y1 ; c < y2 ; c++)
		{
			rect.y = c;
			rect.x = x1 - 1 / meredekseg * (c - y1);
			SDL_FillRect(dst, &rect, color);
		}
	}
}


int main( int argc, char* args[] )
{
	int count = 0, db = 200;

	SDL_Init(SDL_INIT_VIDEO);
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

						default: break;
					}

				break;

				default: break;
			}
		}


		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

		for (int j = 0; j < db; j++)
			line(surface, rand() % WIDTH, rand() % HEIGHT, rand() % WIDTH, rand() % HEIGHT, 0xffffffff);

		count++;


		// TODO:
		// which is faster? this method or SDL_CreateSoftwareRenderer?
		SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);


	}

	std::cout << SDL_GetTicks() << " / " << count << " = " << double(SDL_GetTicks()) / count << std::endl;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}