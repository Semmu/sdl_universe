#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

double deg2rad(double d)
{
	return d / 180 * M_PI;
}

double rad2deg(double r)
{
	return r / M_PI * 180;
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

bool point(SDL_Surface* dst, SDL_Point p, int c)
{
	return point(dst, p.x, p.y, c);
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

bool line(SDL_Surface* dst, SDL_Point p1, SDL_Point p2, int c)
{
	return line(dst, p1.x, p1.y, p2.x, p2.y, c);
}

class SU
{
public:
	static SU& get()
	{
		static SU instance;

		return instance;
	}

	void something()
	{
		std::cout << "thing";
	}


	static int mapColor(int r, int g, int b)
	{
		return SDL_MapRGB(get().surface->format, r, g, b);
	}

private:
	SU() {}

	SU(const SU& su) {}
	void operator=(const SU& su) {}

	SDL_Surface* surface;

	int width;
	int height;
	int bgColor;

	double clipNear, clipFar;
	double FOV;

	void instanceInit(SDL_Surface* dst)
	{
		width = dst->w;
		height = dst->h;
		bgColor = 0;
		clipFar = 100;
		clipNear = 0.1;
		FOV = 60.0;

		surface = dst;
	}

	void instanceDebug()
	{
		std::cout << "w = " << width << std::endl
				  << "h = " << height << std::endl
				  << "surface = " << static_cast<void*>(surface) << std::endl;
	}

public:

	static void init(SDL_Surface* dst)
	{
		get().instanceInit(dst);
	}

	static void debug()
	{
		get().instanceDebug();
	}

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
		static std::list<Primitive*> primitives;

		enum Type
		{
			POINT, LINE, TRIANGLE
		};

		int color;

		Primitive(int c = 0) : color(c)
		{
			primitives.push_back(this);
		}

		virtual Vector getCenter() = 0;
		virtual Type getType() = 0;
	};

	class Point : public Primitive
	{
	public:
		Vector p1;

		Vector getCenter() { return p1; }
		Type getType() { return POINT; }

		Point(Vector P1 = Vector(0, 0, 0), int c = 0) : Primitive(c), p1(P1) {}
	};

	class Line : public Primitive
	{
	public:
		Vector p1, p2;

		Vector getCenter() { return (p1 + p2) / 2; }
		Type getType() { return LINE; }

		Line(Vector P1 = Vector(0, 0, 0), Vector P2 = Vector(1, 1, 1), int c = 0) : Primitive(c), p1(P1), p2(P2) {}
	};

	class Triangle : public Primitive
	{
	public:
		Vector p1, p2, p3;

		Vector getCenter() { return (p1 + p2 + p3) / 3; }
		Type getType() { return TRIANGLE; }

		Triangle(Vector v1 = Vector(0, 0, 0), Vector v2 = Vector(1, 0, 1), Vector v3 = Vector(0, 1, 0), int c = 0) :
			Primitive(c), p1(v1), p2(v2), p3(v3) {}
	};

	class Model
	{
	public:
		std::list<Primitive*> contents;

		void add(Primitive* p)
		{
			contents.push_back(p);
		}
	};


	// TODO: maybe the Transformation and Object class should be merged
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
		static std::list<Object*> objects;

		Model* model;
		Transformation transformation;

		Object() : model(NULL)
		{
			objects.push_back(this);
		}
	};


private:
	SDL_Point instancePositionOnScreen(const Vector& v)
	{
		SDL_Point p;

		double left = clipNear / sin(deg2rad(FOV / 2)) * sin(deg2rad(90 - FOV / 2));
		double up = left / width * height;

		Vector leftVector(-left, 0, 0);
		Vector rightVector(left, 0, 0);
		Vector upVector(0, up, 0);
		Vector downVector(0, -up, 0);

		Vector vectorToPointOnScreen;

		vectorToPointOnScreen.x = v.x * clipNear / v.z;
		vectorToPointOnScreen.y = v.y * clipNear / v.z;

		Vector vectorToPointFromCorner = vectorToPointOnScreen + rightVector + downVector;

		p.x = vectorToPointFromCorner.x / (2 * left) * width;
		p.y = - vectorToPointFromCorner.y / (2 * up) * height;

		return p;

		/*
			xk = +d * x/(z + d) + screen->w/2;
			yk = -d * y/(z + d) + screen->h/2;

			https://infoc.eet.bme.hu/advent.php?v=18
		 */
	}

	bool instanceIsVectorOnScreen(const Vector& v)
	{
		// FIXME: if we compare to 0, the program will freeze in certain situations, where something is too close to the projection plane
		return (v.z > 0.01);
	}

	bool instanceIsPrimitiveOnScreen(Primitive* p)
	{
		switch(p->getType())
		{
			case SU::Primitive::Type::POINT:
			{
				Point* pt = static_cast<Point*>(p);
				return instanceIsVectorOnScreen(pt->p1);
			}
			break;

			case SU::Primitive::Type::LINE:
			{
				Line* l = static_cast<Line*>(p);
				return (instanceIsVectorOnScreen(l->p1) && instanceIsVectorOnScreen(l->p2));
			}

			default: break;
		}

		return false;
	}

	void instanceRender()
	{
		SDL_FillRect(surface, NULL, bgColor);

		std::list<Primitive*> primitives;

		for (Object* o : Object::objects)
		{
			for (Primitive* p : o->model->contents)
			{
				switch(p->getType())
				{
					case SU::Primitive::Type::POINT:
					{
						primitives.push_back(new SU::Point(static_cast<SU::Point*>(p)->p1 + o->transformation.position, p->color));
					}
					break;

					case SU::Primitive::Type::LINE:
					{
						primitives.push_back(new SU::Line(static_cast<SU::Line*>(p)->p1 + o->transformation.position, static_cast<SU::Line*>(p)->p2 + o->transformation.position, p->color));
					}
					break;

					default: break;
				}
			}
		}

		for (Primitive* p : primitives)
		{
			if (true)
			if (instanceIsPrimitiveOnScreen(p))
			{
				switch (p->getType())
				{
					case SU::Primitive::Type::POINT:
					{
						Point* pt = static_cast<Point*>(p);
						Vector v = pt->p1;
						SDL_Point sp = SU::instancePositionOnScreen(v);
						point(surface, sp, pt->color);
					}
					break;

					case SU::Primitive::Type::LINE:
					{
						Line* l = static_cast<Line*>(p);
						SDL_Point p1 = SU::instancePositionOnScreen(l->p1);
						SDL_Point p2 = SU::instancePositionOnScreen(l->p2);
						line(surface, p1, p2, p->color);
					}
					break;

					default: break;
				}
			}
		}
	}

public:
	static void render()
	{
		get().instanceRender();
	}

	static SDL_Point positionOnScreen(const Vector& v)
	{
		return get().instancePositionOnScreen(v);
	}


};

SU::Vector operator*(const double d, const SU::Vector v)
{
	return SU::Vector(v.x*d, v.y*d, v.z*d);
}

std::list<SU::Primitive*> SU::Primitive::primitives;
std::list<SU::Object*> SU::Object::objects;








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
	int count = 0, db = 50;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
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

	SDL_Surface *png = IMG_Load("tetris_pe.png");

	SU::init(surface);


	TTF_Font *font = TTF_OpenFont("./Deltoid-sans.ttf", 32);
	if (font == NULL)
		DIE(TTF_GetError());





	SU::Model amodel;
	amodel.add(new SU::Point(SU::Vector(0, 0, 1), SU::mapColor(0, 0, 255)));
	amodel.add(new SU::Point(SU::Vector(0, 1, 1), SU::mapColor(0, 255, 255)));
	amodel.add(new SU::Point(SU::Vector(0, 0, 0), SU::mapColor(64, 64, 64)));
	amodel.add(new SU::Point(SU::Vector(0, 1, 0), SU::mapColor(0, 255, 0)));

	amodel.add(new SU::Point(SU::Vector(1, 0, 1), SU::mapColor(255, 0, 255)));
	amodel.add(new SU::Point(SU::Vector(1, 1, 1), SU::mapColor(255, 255, 255)));
	amodel.add(new SU::Point(SU::Vector(1, 0, 0), SU::mapColor(255, 0, 0)));
	amodel.add(new SU::Point(SU::Vector(1, 1, 0), SU::mapColor(255, 255, 0)));

	amodel.add(new SU::Line(SU::Vector(0, 0, 0), SU::Vector(1, 0, 0), SU::mapColor(255, 0, 0)));
	amodel.add(new SU::Line(SU::Vector(0, 0, 0), SU::Vector(0, 1, 0), SU::mapColor(0, 255, 0)));
	amodel.add(new SU::Line(SU::Vector(0, 0, 0), SU::Vector(0, 0, 1), SU::mapColor(0, 0, 255)));

	SU::Object aobject;
	aobject.model = &amodel;

	aobject.transformation.position = SU::Vector(1, -0.5, 1);















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

						case SDLK_LEFT:
						{
							aobject.transformation.position.x -= 0.1;
						}
						break;

						case SDLK_RIGHT:
						{
							aobject.transformation.position.x += 0.1;
						}
						break;

						case SDLK_DOWN:
						{
							aobject.transformation.position.y -= 0.1;
						}
						break;

						case SDLK_UP:
						{
							aobject.transformation.position.y += 0.1;
						}
						break;

						case SDLK_s:
						{
							aobject.transformation.position.z -= 0.1;
						}
						break;

						case SDLK_w:
						{
							aobject.transformation.position.z += 0.1;
						}
						break;

						default: break;
					}

				break;

				default: break;
			}
		}


		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

/*		for (int j = 0; j < db; j++)
		{
			line(surface, rand() % WIDTH, rand() % HEIGHT, rand() % WIDTH, rand() % HEIGHT, 0xffffffff);
			SDL_Rect r;
			r.x = rand() % WIDTH;
			r.y = rand() % HEIGHT;
			SDL_BlitSurface(png, NULL, surface, &r);
		}*/

		SU::render();

		SDL_Color c = {128, 128, 128};
		std::stringstream fps;
		// this FPS counter display the total average, not the current
		// will be changed
		fps << " " << int(1000.0 / (double(SDL_GetTicks()) / count)) << " fps";
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