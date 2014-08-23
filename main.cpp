#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


class SU
{
public:

	class Vector
	{
	public:
		double x, y, z;
		Vector(double xx = 0, double yy = 0, double zz = 0) : x(xx), y(yy), z(zz) {}

		double getLength() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		void setLength(const double l)
		{
			double r = l / getLength();
			x *= r; y *= r; z *= r;
		}

		Vector operator+(const Vector v) const
		{
			return Vector(x+v.x, y+v.y, z+v.z);
		}

		void operator+=(const Vector v)
		{
			x+=v.x; y+=v.y; z+=v.z;
		}

        Vector operator-(const Vector v) const
        {
            return Vector(x-v.x, y-v.y, z-v.z);
        }

        void operator-=(const Vector v)
        {
            x-=v.x; y-=v.y; z-=v.z;
        }



		Vector operator*(const double d) const
		{
			return Vector(x*d, y*d, z*d);
		}
		// double * Vector implemented after SU class

		void operator*=(const double d)
		{
			x*=d; y*=d; z*=d;
		}


        Vector operator/(const double d) const
        {
            return Vector(x/d, y/d, z/d);
        }

        // double

        void operator/=(const double d)
        {
            x/=d; y/=d; z/=d;
        }

        Vector getNormalized() const
        {
        	return Vector(x, y, z) / getLength();
        }

        void normalize()
        {
        	double l = getLength();
        	x /= l; y /= l; z /= l;
        }


		Vector operator-() const
		{
			return Vector(-x, -y, -z);
		}


		// skalár szorzat
		//double operator*(Vector v)
		double dotProduct(Vector v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		// bezárt szög radiánban
		//double operator^(Vector v)
		double angleBetween(Vector v) const
		{
			// TODO: TEST
			return acos(dotProduct(v) / getLength() / v.getLength());
		}

		// vetítés
		Vector projectionOn(Vector v) const
		{
			// TODO: TEST
			return v.getNormalized() * dotProduct(v) / v.getLength();
		}

		// vektoriális szorzat
		Vector crossProduct(Vector v) const
		{
			return Vector(y * v.z - z * v.y,
						  z * v.x - x * v.z,
						  x * v.y - y * v.x).getNormalized();
		}

		// forgatás tengely körül
		Vector rotate(const Vector axis, double angle) const
		{
			// FIXME: asszem ez bugos, lehet csak

			Vector projection = projectionOn(axis);
			Vector notProjection = operator-(projection);
			Vector notProjectionOrthogonal = notProjection.getNormalized().crossProduct(axis.getNormalized());

			return projection + (notProjection.getNormalized() * cos(angle) + notProjectionOrthogonal.getNormalized() * sin(angle)) * notProjection.getLength();
		}

		//
		void rotateSelf(const Vector axis, double angle)
		{
			*this = this->rotate(axis, angle);
		}

		std::string toString() const
		{
			std::stringstream str;

			str << "Vector(" << x << ", " << y << ", " << z << ") = " << getLength();

			return str.str();
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector v)
		{
			return (os << v.toString());
		}
	};

	class Primitive
	{
	public:
		enum PrimitiveType
		{
			POINT, LINE, TRIANGLE
		};

		int color;

		Primitive(int c = 0) : color(c) {}

		virtual Vector getCenter() = 0;
		virtual PrimitiveType getType() = 0;
	};

	class Point : public Primitive
	{
	public:
		Vector p1;

		Vector getCenter() { return p1; }
		PrimitiveType getType() { return POINT; }

		Point(Vector P1 = Vector(0, 0, 0), int c = 0) : Primitive(c), p1(P1) {}
	};

	class Line : public Primitive
	{
	public:
		Vector p1, p2;

		Vector getCenter() { return (p1 + p2) / 2; }
		PrimitiveType getType() { return LINE; }

		Line(Vector P1 = Vector(0, 0, 0), Vector P2 = Vector(1, 1, 1), int c = 0) : Primitive(c), p1(P1), p2(P2) {}
	};

	class Triangle : public Primitive
	{
	public:
		Vector p1, p2, p3;

		Vector getCenter() { return (p1 + p2 + p3) / 3; }
		PrimitiveType getType() { return TRIANGLE; }

		Triangle(Vector v1 = Vector(0, 0, 0), Vector v2 = Vector(1, 0, 1), Vector v3 = Vector(0, 1, 0), int c = 0) :
			Primitive(c), p1(v1), p2(v2), p3(v3) {}
	};

	class Model
	{
	public:
		std::list<Primitive*> contents;
	};

	class Transformation
	{
	public:
		Vector position;

		bool advancedTransformation;
		Vector x, y, z;

		Transformation(Vector pos = Vector(0, 0, 0)) : position(pos), advancedTransformation(false) {}
		Transformation(Vector pos, Vector X, Vector Y, Vector Z) : position(pos), advancedTransformation(true), x(X), y(Y), z(Z) {}
	};

	class Object
	{
	public:
		Model* model;
		Transformation* transformation;
	};
};

SU::Vector operator*(const double d, const SU::Vector v)
{
	return SU::Vector(v.x*d, v.y*d, v.z*d);
}









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

bool point(SDL_Surface* dst, int x1, int y1, int color)
{
	SDL_Rect r;
	r.w = r.h = 1;
	r.x = x1;
	r.y = y1;

	SDL_FillRect(dst, &r, color);

	return true;
}

bool line(SDL_Surface* dst, int x1, int y1, int x2, int y2, int color)
{
	// TODO: lock surface pixels and manually access them

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

		return true;
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

		return true;
	}
}


int main( int argc, char* args[] )
{
	int count = 0, db = 50;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
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

	SDL_Surface *png = IMG_Load("tetris_pe.png");

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
							SDL_Delay(1000);
						break;

						default: break;
					}

				break;

				default: break;
			}
		}
/*

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

		for (int j = 0; j < db; j++)
		{
			line(surface, rand() % WIDTH, rand() % HEIGHT, rand() % WIDTH, rand() % HEIGHT, 0xffffffff);
			SDL_Rect r;
			r.x = rand() % WIDTH;
			r.y = rand() % HEIGHT;
			SDL_BlitSurface(png, NULL, surface, &r);
		}*/

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