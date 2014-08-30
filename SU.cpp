#include <iostream>
#include <sstream>
#include <cmath>
#include "SU.h"

// ==================================================================================
//
// 		SMALL HELPER FUNCTION IMPLEMENTATIONS
//
// ==================================================================================

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

		Plane cameraPlane;
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



	bool isOnScreen(const Vector& v)
	{
		return cameraPlane.distanceFrom(v) > 0;
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

			case SU::Primitive::Type::SEGMENT:
			{
				const Segment* l = static_cast<const Segment*>(p);
				return (isOnScreen(l->P1) && isOnScreen(l->P2));
			}

			default: break;
		}

		return false;
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

					case SU::Primitive::Type::SEGMENT:
					{
						Segment* l = static_cast<Segment*>(p);
						SDL_Point p1 = positionOnScreen(l->P1);
						SDL_Point p2 = positionOnScreen(l->P2);
						line(surface, p1, p2, p->color);
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