#include <string>
#include <list>

#if USING_SDL1
	#include <SDL/SDL.h>
#else
	#include <SDL2/SDL.h>
#endif

#define WHITE 0xffffffff

#if USING_SDL1
struct SDL_Point
{
	int x, y;
};
#endif

// ==================================================================================
//
// 		SMALL HELPER FUNCTION DECLARATIONS
//
// ==================================================================================

double randDouble(double max = 1.0);

double deg2rad(double d);
double rad2deg(double r);

namespace SU
{
	// ==============================================================================
	//
	// 		CUSTOM TYPE AND CLASS DECLARATIONS
	//
	// ==============================================================================

	class Vector
	{
	public:
		double x, y, z;
		Vector(double xx = 0, double yy = 0, double zz = 0);

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

		Vector transform(const Vector& x, const Vector& y, const Vector& z, const Vector& p = Vector(0, 0, 0)) const;

		std::string toString() const;
		friend std::ostream& operator<<(std::ostream& os, const Vector& v);
	};

	class Line
	{
	public:
		Vector position, direction;

		Line(Vector pos = Vector(0, 0, 0), Vector dir = Vector(1, 1, 1));

		double distanceFrom(const Vector& v) const;
		double distanceFrom(const Line& l) const;
	};

	class Plane
	{
	public:
		Vector position, direction;

		Plane(Vector pos = Vector(0, 0, 0), Vector dir = Vector(0, 0, 1));
		Plane(Vector a, Vector b, Vector c);

		double distanceFrom(const Vector& v) const;

		bool doesIntersect(const Line& l) const;
		Vector getIntersection(const Line& l) const;
	};

	class Primitive
	{
	public:
		enum Type
		{
			NONE, POINT, SEGMENT, TRIANGLE
		};
		int color;

		Primitive(int c = WHITE);

		virtual Type getType() const = 0;
		virtual Vector getCenter() const = 0;

		virtual ~Primitive();
	};

	class Point : public Primitive
	{
	public:
		Vector P1;

		Point(Vector p1, int c = WHITE);
		Point(double x, double y, double z, int c = WHITE);

		Type getType() const;
		Vector getCenter() const;

		~Point();
	};

	class Segment : public Primitive
	{
	public:
		Vector P1, P2;

		Segment(Vector p1, Vector p2, int c = WHITE);
		Segment(double x1, double y1, double z1, double x2, double y2, double z2, int c = WHITE);

		Type getType() const;
		Vector getCenter() const;

		~Segment();
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

		~Triangle();
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
		bool enabled;

		Model* model;
		Vector position;

		bool transforming;
		Vector X, Y, Z;

		Vector resultantPosition;
		Vector resultantX, resultantY, resultantZ;

		std::list<Object*> children;
		Object* parent;

		Object();
		void addChild(Object* o);

		void rotateAroundX(double angle);
		void rotateAroundY(double angle);
		void rotateAroundZ(double angle);
	};


	namespace Camera
	{
		extern int FOV;

		extern double viewDistanceMin,
					  viewDistanceMax;

		extern Vector position,
					  lookDirection,
					  upDirection,
					  rightDirection;

		void roll(double a);
		void pitch(double a);
		void yaw(double a);
	}





	// ==============================================================================
	//
	// 		PUBLICLY ACCESSIBLE VARIABLE DECLARATIONS
	//
	// ==============================================================================

	enum Flags
	{
		DEBUG_WIREFRAMING,
		DEBUG_TRANSFORMATIONS
//		GLOBAL_LIGHT,
//		LOCAL_LIGHTS,
//		BIG_CHUNK_SPLITTING,
//		COLLISION_SPLITTING,
//		FAST_DEPTH_SORT,
//		Z_BUFFER_SORT
	};

	extern int bgColor;
	extern int flags;

	// FIXME: this is temporary
	extern int primitivesRendered;

	// ==============================================================================
	//
	// 		PUBLICLY ACCESSIBLE METHOD DECLARATIONS
	//
	// ==============================================================================

	bool init(SDL_Surface* surface, int f = 0);

	void setFlag(int f);
	void unsetFlag(int f);
	void toggleFlag(int f);

	int mapColor(int r, int g, int b);

	bool isOnScreen(const Vector& v);

	bool isOnScreen(const Primitive* p);

	SDL_Point positionOnScreen(const Vector& v);

	void render();
}