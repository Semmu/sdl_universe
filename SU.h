#include <string>
#include <list>
#include <SDL2/SDL.h>

#define WHITE 0xffffffff


double deg2rad(double d);
double rad2deg(double r);

bool point(SDL_Surface* dst, int x1, int y1, int color);
bool point(SDL_Surface* dst, SDL_Point p, int c);

bool line(SDL_Surface* dst, int x1, int y1, int x2, int y2, int color);
bool line(SDL_Surface* dst, SDL_Point p1, SDL_Point p2, int c);



namespace SU
{
	// PUBLICLY ACCESSIBLE VARIABLES BELONG HERE

	extern int bgColor;
	extern double clipNear, clipFar;
	extern int FOV;




	class Vector
	{
	public:
		double x, y, z;
		Vector(double xx = 0, double yy = 0, double zz = 0) : x(xx), y(yy), z(zz) {}

		double getLength() const;
		void setLength(const double l);


		Vector operator+(const Vector& v) const;
		void operator+=(const Vector& v);

		Vector operator-(const Vector& v) const;
		void operator-=(const Vector& v);

		Vector operator-() const;		// negate


		Vector operator*(const double d) const;
		friend Vector operator*(const double d, const Vector& v);		// double * Vector
		void operator*=(const double d);

		Vector operator/(const double d) const;				// this is Vector / double operation, double / Vector would be meaningless, it's not implemented
		void operator/=(const double d);


		Vector getNormalized() const;
		void normalize();


		double dotProduct(const Vector& v) const;
		double operator*(const Vector& v) const;

		Vector crossProduct(const Vector& v) const;

		Vector projectionOn(const Vector& v) const;


		double angleTo(const Vector& v) const;
		double operator^(const Vector& v) const;


		Vector rotated(const Vector& axis, const double angle) const;

		std::string toString() const;
		friend std::ostream& operator<<(std::ostream& os, const Vector& v);
	};

	class Primitive
	{
	public:
		enum Type
		{
			NONE, POINT, LINE, TRIANGLE
		};
		int color;

		Primitive(int c = WHITE);

		virtual Type getType() const = 0;
		virtual Vector getCenter() const = 0;
	};

	class Point : public Primitive
	{
	public:
		Vector P1;

		Point(Vector p1, int c = WHITE);
		Point(double x, double y, double z, int c = WHITE);

		Type getType() const;
		Vector getCenter() const;
	};

	class Line : public Primitive
	{
	public:
		Vector P1, P2;

		Line(Vector p1, Vector p2, int c = WHITE);
		Line(double x1, double y1, double z1, double x2, double y2, double z2, int c = WHITE);

		Type getType() const;
		Vector getCenter() const;
	};

	class Triangle : public Primitive
	{
	public:
		Vector P1, P2, P3;

		Triangle(Vector p1, Vector p2, Vector p3, int c = WHITE);
		Triangle(double x1, double y1, double z1,
				 double x2, double y2, double z2,
				 double x3, double y3, double z3, int c = WHITE);

		Type getType() const;
		Vector getCenter() const;
	};

	class Model
	{
	public:
		std::list<Primitive*> contents;

		void add(Primitive* p);
	};

	class Object
	{
	public:
		static std::list<Object*> objects;

		Model* model;
		Vector position;

		Object();
	};





	bool init(SDL_Surface* surface);

	int mapColor(int r, int g, int b);

	bool isOnScreen(const Vector& v);

	bool isOnScreen(const Primitive* p);

	SDL_Point positionOnScreen(const Vector& v);

	void render();
}