#include "SU.h"
#include <sstream>

double randDouble(double max) { return double(rand()) / RAND_MAX * max; }

double deg2rad(double d) { return d / 180 * M_PI; }
double rad2deg(double r) { return r / M_PI * 180; }

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

namespace SU
{
	// PUBLICLY ACCESSIBLE VARIABLE **DEFINITIONS** BELONG HERE
	// so only value assignments. they should be "mentioned" in the header as well

	int bgColor = 0;
	double clipNear = 1.0, clipFar = 100.0;
	int FOV = 60;
	int flags;




	namespace
	{
		// PRIVATE THINGS BELONG HERE

		SDL_Surface* surface;
		int width, height;

		std::list<Object*> everyObject;
		std::list<Primitive*> primitivesToRender;
	}



	double Vector::getLength() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	void Vector::setLength(const double l)
	{
		double ratio = l / getLength();

		x *= ratio;		y *= ratio;		z *= ratio;
	}


	Vector Vector::operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	void Vector::operator+=(const Vector& v)
	{
		x += v.x;		y += v.y;		z += v.z;
	}

	Vector Vector::operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	void Vector::operator-=(const Vector& v)
	{
		x -= v.x;		y -= v.y;		z -= v.z;
	}

	Vector Vector::operator-() const		// negate
	{
		return Vector(-x, -y, -z);
	}


	Vector Vector::operator*(const double d) const
	{
		return Vector(x * d, y * d, z * d);
	}
	Vector operator*(const double d, const Vector& v)		// double * Vector
	{
		return Vector(v.x * d, v.y * d, v.z * d);
	}
	void Vector::operator*=(const double d)
	{
		x *= d;		y *= d;		z *= d;
	}

	Vector Vector::operator/(const double d) const				// this is Vector / double operation, double / Vector would be meaningless, it's not implemented
	{
		return Vector(x / d, y / d, z / d);
	}
	void Vector::operator/=(const double d)
	{
		x /= d;		y /= d;		z /= d;
	}


	Vector Vector::getNormalized() const
	{
		return Vector(x, y, z) / getLength();
	}
	void Vector::normalize()
	{
		setLength(1.0);
	}


	double Vector::dotProduct(const Vector& v) const
	{
		return x * v.x  +  y * v.y  +  z * v.z;
	}
	double Vector::operator*(const Vector& v) const
	{
		return dotProduct(v);
	}

	Vector Vector::crossProduct(const Vector& v) const
	{
		return Vector(y * v.z - z * v.y,
					  z * v.x - x * v.z,
					  x * v.y - y * v.x);
	}

	Vector Vector::projectionOn(const Vector& v) const
	{
		return v * (dotProduct(v) / v.getLength());
	}


	double Vector::angleTo(const Vector& v) const
	{
		return acos(dotProduct(v) / getLength() / v.getLength());
	}
	double Vector::operator^(const Vector& v) const
	{
		return angleTo(v);
	}


	Vector Vector::rotated(const Vector& axis, const double angle) const
	{
		// TODO

		exit(13);
	}

	std::string Vector::toString() const
	{
		std::stringstream str;
		str << "SU::Vector( " << x << " , " << y << " , " << z << " )";
		return str.str();
	}
	std::ostream& operator<<(std::ostream& os, const Vector& v)
	{
		return (os << v.toString());
	}



	Vector getTransformed(Vector v, Vector x, Vector y, Vector z)
	{
		return Vector(v.x * x.x + v.y * y.x + v.z * z.x,
					  v.x * x.y + v.y * y.y + v.z * z.y,
					  v.x * x.z + v.y * y.z + v.z * z.z);
	}



	Primitive::Primitive(int c) : color(c) {}
	Primitive::~Primitive() {}



	Point::Point(Vector p1, int c) : Primitive(c), P1(p1) {}

	Point::Point(double x, double y, double z, int c) : Primitive(c)
	{
		P1 = Vector(x, y, z);
	}

	Primitive::Type Point::getType() const
	{
		return Primitive::Type::POINT;
	}

	Vector Point::getCenter() const
	{
		return P1;
	}

	Point::~Point() {}



	Line::Line(Vector p1, Vector p2, int c) : Primitive(c), P1(p1), P2(p2) {}

	Line::Line(double x1, double y1, double z1,
			   double x2, double y2, double z2, int c) : Primitive(c)
	{
		P1 = Vector(x1, y1, z1);
		P2 = Vector(x2, y2, z2);
	}

	Primitive::Type Line::getType() const
	{
		return Primitive::Type::LINE;
	}

	Vector Line::getCenter() const
	{
		return (P1 + P2) / 2;
	}

	Line::~Line() {}



	Triangle::Triangle(Vector p1, Vector p2, Vector p3, int c) : Primitive(c), P1(p1), P2(p2), P3(p3) {}

	Triangle::Triangle(double x1, double y1, double z1,
					   double x2, double y2, double z2,
					   double x3, double y3, double z3, int c) : Primitive(c)
	{
		P1 = Vector(x1, y1, z1);
		P2 = Vector(x2, y2, z2);
		P3 = Vector(x3, y3, z3);
	}

	Primitive::Type Triangle::getType() const
	{
		return Primitive::Type::TRIANGLE;
	}

	Vector Triangle::getCenter() const
	{
		return (P1 + P2 + P3) / 3;
	}

	Triangle::~Triangle() {}


	void Model::add(Primitive* p)
	{
		contents.push_back(p);
	}


	// Object::objects is static
	std::list<Object*> Object::objects;

	Object::Object() : enabled(true), transforming(false), X(Vector(1, 0, 0)), Y(Vector(0, 1, 0)), Z(Vector(0, 0, 1)), parent(NULL)
	{
		everyObject.push_back(this);
	}

	void Object::addChild(Object* o)
	{
		o->parent = this;
		children.push_back(o);
	}



	bool init(SDL_Surface* s, int f)
	{
		surface = s;
		width = s->w;
		height = s->h;

		flags = f;

		// it may come handy
		return true;
	}

	void setFlag(int f)
	{
		flags |= f;
	}

	void unsetFlag(int f)
	{
		flags = (flags & ~f);
	}

	void toggleFlag(int f)
	{
		flags ^= f;
	}

	int mapColor(int r, int g, int b)
	{
		return SDL_MapRGB(surface->format, r, g, b);
	}



	bool isOnScreen(const Vector& v)
	{
		// FIXME: if we compare to 0, the program will freeze in certain situations, where something is too close to the projection plane
		return (v.z > 0.0001);
	}

	bool isOnScreen(const Primitive* p)
	{
		switch(p->getType())
		{
			case SU::Primitive::Type::POINT:
			{
				const Point* pt = static_cast<const Point*>(p);
				return isOnScreen(pt->P1);
			}
			break;

			case SU::Primitive::Type::LINE:
			{
				const Line* l = static_cast<const Line*>(p);
				return (isOnScreen(l->P1) && isOnScreen(l->P2));
			}

			default: break;
		}

		return false;
	}

	SDL_Point positionOnScreen(const Vector& v)
	{

		SDL_Point p;

		// FIXME: something is wrong with FOV, should get a verification
		double left = clipNear / sin(deg2rad(90 - FOV / 2)) * sin(deg2rad(FOV / 2));
		// this line was:
		// double left = clipNear / sin(deg2rad(FOV / 2)) * sin(deg2rad(90 - FOV / 2));


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
		p.y = -vectorToPointFromCorner.y / (2 * up) * height;

		return p;
	}

	void processObject(Object* o)
	{
		if (o->parent != NULL)
		{
			o->resultantPosition = o->parent->position + getTransformed(o->position, o->parent->X, o->parent->Y, o->parent->Z);
			o->resultantX = getTransformed(o->X, o->parent->X, o->parent->Y, o->parent->Z);
			o->resultantY = getTransformed(o->Y, o->parent->X, o->parent->Y, o->parent->Z);
			o->resultantZ = getTransformed(o->Z, o->parent->X, o->parent->Y, o->parent->Z);
		}
		else
		{
			// camera position and direction should affect these

			o->resultantPosition = o->position;
			o->resultantX = o->X;
			o->resultantY = o->Y;
			o->resultantZ = o->Z;
		}

		if (flags & Flags::DEBUG_TRANSFORMATIONS)
		{
			primitivesToRender.push_back(new SU::Line(o->resultantPosition, o->resultantX + o->resultantPosition, SU::mapColor(255, 0, 0)));
			primitivesToRender.push_back(new SU::Line(o->resultantPosition, o->resultantY + o->resultantPosition, SU::mapColor(0, 255, 0)));
			primitivesToRender.push_back(new SU::Line(o->resultantPosition, o->resultantZ + o->resultantPosition, SU::mapColor(0, 0, 255)));
		}

		for (Primitive* p : o->model->contents)
		{
			switch(p->getType())
			{
				case SU::Primitive::Type::POINT:
				{
					SU::Point *pt;

					if (o->transforming)
					{
						pt = new SU::Point(getTransformed(static_cast<SU::Point*>(p)->P1, o->resultantX, o->resultantY, o->resultantZ) + o->resultantPosition, p->color);
					}
					else
					{
						pt = new SU::Point(static_cast<SU::Point*>(p)->P1 + o->resultantPosition);
					}

					primitivesToRender.push_back(pt);
				}
				break;

				case SU::Primitive::Type::LINE:
				{
					SU::Line *l;

					if (o->transforming)
					{
						l = new SU::Line(getTransformed(static_cast<SU::Line*>(p)->P1, o->resultantX, o->resultantY, o->resultantZ) + o->resultantPosition,
										 getTransformed(static_cast<SU::Line*>(p)->P2, o->resultantX, o->resultantY, o->resultantZ) + o->resultantPosition,
										 p->color);
					}
					else
					{
						l = new SU::Line(static_cast<SU::Line*>(p)->P1 + o->resultantPosition,
										 static_cast<SU::Line*>(p)->P2 + o->resultantPosition,
										 p->color);
					}

					primitivesToRender.push_back(l);
				}
				break;

				default: break;
			}
		}

		if (o->children.size() > 0)
			for(Object *ch : o->children)
				processObject(ch);
	}

	void render()
	{
		SDL_FillRect(surface, NULL, bgColor);

		for (Object* o : everyObject)
		{
			if (o->enabled && o->parent == NULL)
			{
				processObject(o);
			}
		}

		for (Primitive* p : primitivesToRender)
		{
			if (isOnScreen(p))
			{
				switch (p->getType())
				{
					case SU::Primitive::Type::POINT:
					{
						Point* pt = static_cast<Point*>(p);
						Vector v = pt->P1;
						SDL_Point sp = positionOnScreen(v);
						point(surface, sp, pt->color);
					}
					break;

					case SU::Primitive::Type::LINE:
					{
						Line* l = static_cast<Line*>(p);
						SDL_Point p1 = positionOnScreen(l->P1);
						SDL_Point p2 = positionOnScreen(l->P2);
						line(surface, p1, p2, p->color);
					}
					break;

					default: break;
				}
			}
		}

		primitivesToRender.clear();
	}
}