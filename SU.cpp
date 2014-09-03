#include <iostream>
#include <sstream>
#include <cmath>
#include "SU.h"

// ==================================================================================
//
// 		SMALL HELPER FUNCTION IMPLEMENTATIONS
//
// ==================================================================================

void swap(int& a, int& b)
{
	int aa = a;
	a = b;
	b = aa;
}

double randDouble(double max) { return double(rand()) / RAND_MAX * max; }

double deg2rad(double d) { return d / 180 * M_PI; }
double rad2deg(double r) { return r / M_PI * 180; }

void putpixel_nolock(SDL_Surface* s, int x, int y, int color)
{
	if (x < 0 || x >= s->w || y < 0 || y >= s->h)
		return;

	Uint32 *pixels = (Uint32 *)s->pixels;
	pixels[ ( y * s->w ) + x ] = color;
}
void putpixel_nolock(SDL_Surface* s, SDL_Point p, int c)
{
	putpixel_nolock(s, p.x, p.y, c);
}

int getpixel_nolock(SDL_Surface* s, int x, int y)
{
	Uint32 *pixels = (Uint32 *)s->pixels;
	return pixels[ ( y * s->w ) + x ];
}

void putpixel(SDL_Surface* s, int x, int y, int color)
{
	if (SDL_MUSTLOCK(s))
		SDL_LockSurface(s);

	putpixel_nolock(s, x, y, color);

	if (SDL_MUSTLOCK(s))
		SDL_UnlockSurface(s);
}


void point(SDL_Surface* dst, int x1, int y1, int color)
{
	putpixel(dst, x1, y1, color);
}
void point(SDL_Surface* dst, SDL_Point p, int c)
{
	return point(dst, p.x, p.y, c);
}

void line(SDL_Surface* s, int x0, int y0, int x1, int y1, int color)
{
	// Bresenham's line algorithm
	// 		http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	int dx = abs(x1-x0);
	int dy = abs(y1-y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	int err = dx - dy;

	bool loop = true;


	if (SDL_MUSTLOCK(s))
		SDL_LockSurface(s);

	while(loop)
	{
		putpixel_nolock(s, x0, y0, color);
		if (x0 == x1 && y0 == y1)
			loop = false;

		int e2 = 2 * err;

		if (e2 > -dy)
		{
			err = err - dy;
			x0 = x0 + sx;
		}

		if (e2 < dx)
		{
			err = err + dx;
			y0 = y0 + sy;
		}
	}

	if (SDL_MUSTLOCK(s))
		SDL_UnlockSurface(s);
}
void line(SDL_Surface* dst, SDL_Point p1, SDL_Point p2, int c)
{
	line(dst, p1.x, p1.y, p2.x, p2.y, c);
}

void tri(SDL_Surface* s, int x0, int y0, int x1, int y1, int x2, int y2, int c)
{
	// snippet from:
	// 		 http://www.codeproject.com/Tips/86354/draw-triangle-algorithm-D

    int width = s->w;
    int height = s->h;

    if (y1 > y2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }
    if (y0 > y1)
    {
        swap(x0, x1);
        swap(y0, y1);
    }
    if (y1 > y2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    double dx_far = double(x2 - x0) / (y2 - y0 + 1);
    double dx_upper = double(x1 - x0) / (y1 - y0 + 1);
    double dx_low = double(x2 - x1) / (y2 - y1 + 1);
    double xf = x0;
    double xt = x0 + dx_upper;
    for (int y = y0; y <= (y2 > height-1 ? height-1 : y2); y++)
    {
        if (y >= 0)
        {
            for (int x = (xf > 0 ? int(xf) : 0); x <= (xt < width ? xt : width-1) ; x++)
            	putpixel_nolock(s, x, y, c);
            for (int x = (xf < width ? int(xf) : width-1); x >= (xt > 0 ? xt : 0); x--)
            	putpixel_nolock(s, x, y, c);
        }
        xf += dx_far;
        if (y < y1)
            xt += dx_upper;
        else
            xt += dx_low;
    }
}
void tri(SDL_Surface* s, SDL_Point p1, SDL_Point p2, SDL_Point p3, int c)
{
	tri(s, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c);
}

bool sortPrimitivesNearToFar(const SU::Primitive* p1, const SU::Primitive* p2)
{
	return (p1->getCenter() - SU::Camera::position).getLength() > (p2->getCenter() - SU::Camera::position).getLength();
}

namespace SU
{
	// ==============================================================================
	//
	// 		PUBLICLY ACCESSIBLE VARIABLE DEFINITIONS
	//
	// ==============================================================================

	namespace Camera
	{
		int FOV = 60;

		double viewDistanceMin = 0.1,
			   viewDistanceMax = 100.0;

		Vector position = Vector(0, 0, 0),
			   lookDirection = Vector(0, 0, 1),
			   upDirection = Vector(0, 1, 0),
			   rightDirection = Vector(1, 0, 0);

		void roll(double a)
		{
			upDirection = upDirection.rotated(lookDirection, a);
			rightDirection = rightDirection.rotated(lookDirection, a);
		}
		void pitch(double a)
		{
			upDirection = upDirection.rotated(rightDirection, a);
			lookDirection = lookDirection.rotated(rightDirection, a);
		}
		void yaw(double a)
		{
			rightDirection = rightDirection.rotated(upDirection, a);
			lookDirection = lookDirection.rotated(upDirection, a);
		}
	}

	int bgColor = 0;
	int flags = 0;

	// FIXME: this is temporary
	int primitivesRendered = 0;


	// ==============================================================================
	//
	// 		PRIVATE VARIABLE DECLARATIONS AND DEFINITIONS
	//
	// ==============================================================================

	namespace
	{
		SDL_Surface* surface;
		int width, height;

		std::list<Object*> everyObject;
		std::list<Primitive*> primitivesToRender;

		Plane cameraPlane, cameraBottomPlane, cameraLeftPlane, cameraRightPlane, cameraTopPlane;
		Vector cameraPlaneWidth, cameraPlaneHeight, cameraPlaneBottomLeft;
	}





	// ==============================================================================
	//
	// 		CUSTOM TYPE AND CLASS IMPLEMENTATIONS
	//
	// ==============================================================================

	Vector::Vector(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}

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
		Vector selfAxisComponent = projectionOn(axis);
		Vector selfOrthogonalComponent = operator-(selfAxisComponent);

		Vector orthogonalOrthogonal = axis.crossProduct(selfOrthogonalComponent);

		Vector result = sin(angle) * orthogonalOrthogonal +
						cos(angle) * selfOrthogonalComponent +
						selfAxisComponent;

		// sadly, doubles are inaccurate, we have to set the length of the result vector manually
		// without this, the inaccury would distort the vectors
		result.setLength(getLength());

		return result;
	}


	Vector Vector::transform(const Vector& xx, const Vector& yy, const Vector& zz, const Vector& pp) const
	{
		return Vector(x * xx.x + y * yy.x + z * zz.x + pp.x,
					  x * xx.y + y * yy.y + z * zz.y + pp.y,
					  x * xx.z + y * yy.z + z * zz.z + pp.z);
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



	Line::Line(Vector p, Vector d) : position(p), direction(d) {}

	double Line::distanceFrom(const Vector& v) const
	{
		// TODO
		return 0.666;
	}

	double Line::distanceFrom(const Line& l) const
	{
		// TODO
		return 0.666;
	}



	Plane::Plane(Vector p, Vector d) : position(p), direction(d) {}
	Plane::Plane(Vector a, Vector b, Vector c) : position(a), direction((a-b).crossProduct(c-b)) {}

	double Plane::distanceFrom(const Vector& v) const
	{
		// some easy math
		return ( direction.x * (v.x - position.x) +
				 direction.y * (v.y - position.y) +
				 direction.z * (v.z - position.z ) ) / ( direction.getLength() );
	}

	bool Plane::doesIntersect(const Line& l) const
	{
		// they do not intersect only if they are parrallell
		// but i think we should compare it to an (-EPSILON, +EPSILON) interval
		// TODO
		return ((direction * l.direction) != 0);
	}

	Vector Plane::getIntersection(const Line& l) const
	{
		// these are to shorten the above equations
		double s1 = position.x;			// "s" is a point of the plane
		double s2 = position.y;
		double s3 = position.z;

		double n1 = direction.x;		// "n" is the directional vector (diagonal) of the plane
		double n2 = direction.y;
		double n3 = direction.z;

		double p1 = l.position.x;		// "p" is a point of the line
		double p2 = l.position.y;
		double p3 = l.position.z;

		double v1 = l.direction.x;		// "v" is the direction of the line
		double v2 = l.direction.y;
		double v3 = l.direction.z;


		// some even more shorteners
		double sumNV = n1 * v1 + n2 * v2 + n3 * v3;
		double sumSN = s1 * n1 + s2 * n2 + s3 * n3;

		// the hardcore equations
		double x = ( v1 * (sumSN - p2 * n2 - p3 * n3) + p1 * v2 * n2 + p1 * v3 * n3 ) / sumNV;
		double y = ( v2 * (sumSN - p1 * n1 - p3 * n3) + p2 * v1 * n1 + p2 * v3 * n3 ) / sumNV;
		double z = ( v3 * (sumSN - p1 * n1 - p2 * n2) + p3 * v1 * n1 + p3 * v2 * n2 ) / sumNV;
		// not going to explain it, this is pure math, it should be correct

		// yay!
		return Vector(x, y, z);
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



	Segment::Segment(Vector p1, Vector p2, int c) : Primitive(c), P1(p1), P2(p2) {}

	Segment::Segment(double x1, double y1, double z1,
			   double x2, double y2, double z2, int c) : Primitive(c)
	{
		P1 = Vector(x1, y1, z1);
		P2 = Vector(x2, y2, z2);
	}

	Primitive::Type Segment::getType() const
	{
		return Primitive::Type::SEGMENT;
	}

	Vector Segment::getCenter() const
	{
		return (P1 + P2) / 2;
	}

	Segment::~Segment() {}



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

	Vector Triangle::getDirection() const
	{
		return (P2 - P1).crossProduct(P2 - P3);
	}

	Triangle::~Triangle() {}



	void Model::add(Primitive* p)
	{
		contents.push_back(p);
	}



	Object::Object() : enabled(true), transforming(false), X(Vector(1, 0, 0)), Y(Vector(0, 1, 0)), Z(Vector(0, 0, 1)), parent(NULL)
	{
		everyObject.push_back(this);
	}

	void Object::addChild(Object* o)
	{
		o->parent = this;
		children.push_back(o);
	}

	void Object::rotateAroundX(double a)
	{
		Y = Y.rotated(X, a);
		Z = Z.rotated(X, a);
	}

	void Object::rotateAroundY(double a)
	{
		X = X.rotated(Y, a);
		Z = Z.rotated(Y, a);
	}

	void Object::rotateAroundZ(double a)
	{
		X = X.rotated(Z, a);
		Y = Y.rotated(Z, a);
	}





	// ==============================================================================
	//
	// 		PUBLICLY ACCESSIBLE METHOD IMPLEMENTATIONS
	//
	// ==============================================================================

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



	bool isInFrontOfCamera(const Vector& v)
	{
		return cameraPlane.distanceFrom(v) > 0;
	}

	bool isOnScreen(const Vector& v)
	{
		return isInFrontOfCamera(v) &&
			   cameraBottomPlane.distanceFrom(v) > 0 &&
			   cameraRightPlane.distanceFrom(v) > 0 &&
			   cameraLeftPlane.distanceFrom(v) > 0 &&
			   cameraTopPlane.distanceFrom(v) > 0;
	}

	bool isOnScreen(const Primitive* p)
	{
		// a Primitive is considered to be on screen if every Vector that it has is in front of the camera and
		// 		at least one Vector is on the screen

		switch(p->getType())
		{
			case SU::Primitive::Type::POINT:
			{
				const Point* pt = static_cast<const Point*>(p);
				return isOnScreen(pt->P1);
			}
			break;

			case SU::Primitive::Type::SEGMENT:
			{
				const Segment* l = static_cast<const Segment*>(p);
				return (isInFrontOfCamera(l->P1) && isInFrontOfCamera(l->P2)) &&
					   (isOnScreen(l->P1) || isOnScreen(l->P2));
			}

			case SU::Primitive::Type::TRIANGLE:
			{
				const Triangle* t = static_cast<const Triangle*>(p);
				return (isInFrontOfCamera(t->P1) && isInFrontOfCamera(t->P2) && isInFrontOfCamera(t->P3)) &&
					   (isOnScreen(t->P1) || isOnScreen(t->P2) || isOnScreen(t->P3));
			}

			default: break;
		}

		return false;
	}

	bool isFacingTheCamera(const Triangle* t)
	{
		return t->getDirection().angleTo(Camera::position - t->getCenter()) < M_PI / 2;
	}

	SDL_Point positionOnScreen(const Vector& v)
	{
		Line lineToVector = Line(Camera::position, v - Camera::position);
		Vector positionOnPlane = cameraPlane.getIntersection(lineToVector);
		Vector fromBottomLeftToPositionOnPlane = positionOnPlane - cameraPlaneBottomLeft;

		double angleToWidth = cameraPlaneWidth.angleTo(fromBottomLeftToPositionOnPlane);
		double angleToHeight = cameraPlaneHeight.angleTo(fromBottomLeftToPositionOnPlane);

		SDL_Point p;

		p.x = cos(angleToWidth) * fromBottomLeftToPositionOnPlane.getLength() / cameraPlaneWidth.getLength() * width;
		p.y = height - (cos(angleToHeight) * fromBottomLeftToPositionOnPlane.getLength() / cameraPlaneHeight.getLength() * height);

		return p;
	}

	void processObject(Object* o)
	{
		if (o->parent == NULL)
		{
			// this is a root Object

			o->resultantPosition = o->position;

			o->resultantX = o->X;
			o->resultantY = o->Y;
			o->resultantZ = o->Z;
		}
		else
		{
			// this is a child object, resultant vectors should be affected by parents transformation

			o->resultantPosition = o->position.transform(o->parent->resultantX, o->parent->resultantY, o->parent->resultantZ, o->parent->resultantPosition);
			o->resultantX = o->X.transform(o->parent->resultantX, o->parent->resultantY, o->parent->resultantZ);
			o->resultantY = o->Y.transform(o->parent->resultantX, o->parent->resultantY, o->parent->resultantZ);
			o->resultantZ = o->Z.transform(o->parent->resultantX, o->parent->resultantY, o->parent->resultantZ);
		}

		if (flags & Flags::DEBUG_TRANSFORMATIONS)
		{
			if (o->transforming)
			{
				primitivesToRender.push_back(new SU::Segment(o->resultantPosition, o->resultantX + o->resultantPosition, SU::mapColor(255, 0, 0)));
				primitivesToRender.push_back(new SU::Segment(o->resultantPosition, o->resultantY + o->resultantPosition, SU::mapColor(0, 255, 0)));
				primitivesToRender.push_back(new SU::Segment(o->resultantPosition, o->resultantZ + o->resultantPosition, SU::mapColor(0, 0, 255)));
			}
			else
			{
				// if the Object isn't transformed, we should draw the normal axes

				primitivesToRender.push_back(new SU::Segment(o->resultantPosition, Vector(1, 0, 0) + o->resultantPosition, SU::mapColor(255, 0, 0)));
				primitivesToRender.push_back(new SU::Segment(o->resultantPosition, Vector(0, 1, 0) + o->resultantPosition, SU::mapColor(0, 255, 0)));
				primitivesToRender.push_back(new SU::Segment(o->resultantPosition, Vector(0, 0, 1) + o->resultantPosition, SU::mapColor(0, 0, 255)));
			}

			primitivesToRender.push_back(new SU::Segment(Vector(0, 0, 0), o->resultantPosition, SU::mapColor(50, 50, 50)));
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
						pt = new SU::Point(static_cast<SU::Point*>(p)->P1.transform(o->resultantX, o->resultantY, o->resultantZ, o->resultantPosition), p->color);
					}
					else
					{
						pt = new SU::Point(static_cast<SU::Point*>(p)->P1 + o->resultantPosition);
					}

					primitivesToRender.push_back(pt);
				}
				break;

				case SU::Primitive::Type::SEGMENT:
				{
					SU::Segment *l;

					if (o->transforming)
					{
						l = new SU::Segment(static_cast<SU::Segment*>(p)->P1.transform(o->resultantX, o->resultantY, o->resultantZ, o->resultantPosition),
											static_cast<SU::Segment*>(p)->P2.transform(o->resultantX, o->resultantY, o->resultantZ, o->resultantPosition),
											p->color);
					}
					else
					{
						l = new SU::Segment(static_cast<SU::Segment*>(p)->P1 + o->resultantPosition,
											static_cast<SU::Segment*>(p)->P2 + o->resultantPosition,
											p->color);
					}

					primitivesToRender.push_back(l);
				}
				break;

				case SU::Primitive::Type::TRIANGLE:
				{
					SU::Triangle *t;

					if (o->transforming)
					{
						t = new SU::Triangle(static_cast<SU::Triangle*>(p)->P1.transform(o->resultantX, o->resultantY, o->resultantZ, o->resultantPosition),
											 static_cast<SU::Triangle*>(p)->P2.transform(o->resultantX, o->resultantY, o->resultantZ, o->resultantPosition),
											 static_cast<SU::Triangle*>(p)->P3.transform(o->resultantX, o->resultantY, o->resultantZ, o->resultantPosition),
											 p->color);
					}
					else
					{
						t = new SU::Triangle(static_cast<SU::Triangle*>(p)->P1 + o->resultantPosition,
											 static_cast<SU::Triangle*>(p)->P2 + o->resultantPosition,
											 static_cast<SU::Triangle*>(p)->P3 + o->resultantPosition,
											 p->color);
					}

					primitivesToRender.push_back(t);
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
		// FIXME: FOV value restriction should be somewhere else?

		if (Camera::FOV < 10)
			Camera::FOV = 10;

		if (Camera::FOV > 170)
			Camera::FOV = 170;


		SDL_FillRect(surface, NULL, bgColor);
		primitivesRendered = 0;


		/*
		 *
		 *	INITIALIZING THE CAMERA PLANES AND VECTORS
		 *
		 */

		 cameraPlane = Plane(Camera::position + Camera::lookDirection.getNormalized() * Camera::viewDistanceMin, Camera::lookDirection);

		 cameraPlaneWidth = Camera::rightDirection.getNormalized() *
		 					(sin(deg2rad(Camera::FOV/2)) / sin(deg2rad(90 - Camera::FOV/2))) * Camera::viewDistanceMin * 2;

		 cameraPlaneHeight = Camera::upDirection.getNormalized() *
		 					 cameraPlaneWidth.getLength() / width * height;

		 cameraPlaneBottomLeft = cameraPlane.position - (cameraPlaneWidth / 2) - (cameraPlaneHeight / 2);


		 cameraBottomPlane = Plane(Camera::position, cameraPlaneBottomLeft + cameraPlaneWidth, cameraPlaneBottomLeft);
		 cameraTopPlane = Plane(Camera::position, cameraPlaneBottomLeft + cameraPlaneHeight, cameraPlaneBottomLeft + cameraPlaneHeight + cameraPlaneWidth);
		 cameraRightPlane = Plane(Camera::position, cameraPlaneBottomLeft + cameraPlaneWidth + cameraPlaneHeight, cameraPlaneBottomLeft + cameraPlaneWidth);
		 cameraLeftPlane = Plane(Camera::position, cameraPlaneBottomLeft, cameraPlaneBottomLeft + cameraPlaneHeight);

		/*
		 *
		 *	COLLECTING WHAT PRIMITIVES TO RENDER
		 *
		 */

		for (Object* o : everyObject)
		{
			if (o->enabled && o->parent == NULL)
			{
				processObject(o);
			}
		}



		/*
		 *
		 *	SORTING THE PRIMITIVES
		 *
		 */

		 if (flags & Flags::DEPTH_SORT)
			 primitivesToRender.sort(sortPrimitivesNearToFar);



		/*
		 *
		 *	RENDERING THE PRIMITIVES
		 *
		 */

		for (Primitive* p : primitivesToRender)
		{
			if (isOnScreen(p))
			{
				primitivesRendered++;

				switch (p->getType())
				{
					case SU::Primitive::Type::POINT:
					{
						Point* pt = static_cast<Point*>(p);
						SDL_Point sp = positionOnScreen(pt->P1);
						point(surface, sp, pt->color);
					}
					break;

					case SU::Primitive::Type::SEGMENT:
					{
						Segment* l = static_cast<Segment*>(p);
						SDL_Point p1 = positionOnScreen(l->P1);
						SDL_Point p2 = positionOnScreen(l->P2);
						line(surface, p1, p2, p->color);
					}
					break;

					case SU::Primitive::Type::TRIANGLE:
					{
						Triangle* t = static_cast<Triangle*>(p);

						if ((flags & Flags::ONLY_FACING_TRIANGLES) ? isFacingTheCamera(t) : true)
						{
							SDL_Point p1 = positionOnScreen(t->P1);
							SDL_Point p2 = positionOnScreen(t->P2);
							SDL_Point p3 = positionOnScreen(t->P3);

							if (flags & Flags::DEBUG_WIREFRAMING)
							{
								line(surface, p1, p2, p->color);
								line(surface, p1, p3, p->color);
								line(surface, p3, p2, p->color);
							}
							else
							{
								tri(surface, p1, p2, p3, p->color);
							}
						}
					}
					break;

					default: break;
				}
			}
		}




		/*
		 *
		 *	FREEING UP MEMORY AFTER RENDER
		 *
		 */

		for(Primitive* p : primitivesToRender)
			delete p;

		primitivesToRender.clear();
	}
}