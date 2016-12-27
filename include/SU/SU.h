#ifndef SU_h
#define SU_h

#include <SDL/SDL.h>
#include <string>
#include <list>
#include "SU/SU_math.h"
#include "SU/SU_etc.h"


#define WHITE 0xffffffff


namespace SU
{
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

        static bool sort(const Primitive* p1, const Primitive* p2);
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
        bool lighted;

        Triangle(Vector p1, Vector p2, Vector p3, int c = WHITE, bool l = false);
        Triangle(double x1, double y1, double z1,
                 double x2, double y2, double z2,
                 double x3, double y3, double z3, int c = WHITE, bool l = false);

        void flip();

        Type getType() const;
        Vector getCenter() const;

        Vector getDirection() const;

        ~Triangle();
    };

    // convenience only feature
    class Quad
    {
    public:
        Vector P1, P2, P3, P4;
        int color;
        bool lighted;

        Quad(Vector, Vector, Vector, Vector, int c = WHITE, bool l = false);
        Quad(double x1, double y1, double z1,
             double x2, double y2, double z2,
             double x3, double y3, double z3,
             double x4, double y4, double z4, int c = WHITE, bool l = false);
    };

    class Mesh
    {
    public:
        std::list<Primitive*> primitives;

        void add(Primitive* p);
        void add(Quad* q);

        // inmediate OpenGL-like methods, TODO
        void begin();
        void add(Vector v);
        void add(double, double, double);
        void end();
    };

    class Object
    {
    public:
        bool enabled;

        bool flipTriangles;
        Mesh* mesh;
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

        void scale(double r);

        Uint8 getLevel();
    };


    namespace Shaders
    {
        class Shader
        {
        public:
            virtual void shadePoint(Point* p);
            virtual void shadeSegment(Segment* p);
            virtual void shadeTriangle(Triangle* p);
        };

        class BumpMapShader : public Shader
        {
        public:
            virtual void shadePoint(Point* p);
            virtual void shadeSegment(Segment* p);
            virtual void shadeTriangle(Triangle* p);
        };

        extern std::list<Shader*> shaders;

        void shadePoint(Point* p);
        void shadeSegment(Segment* s);
        void shadeTriangle(Triangle* t);
    }




    // ==============================================================================
    //
    //      PUBLICLY ACCESSIBLE VARIABLE DECLARATIONS
    //
    // ==============================================================================

    enum Flags
    {
        DEBUG_WIREFRAMING =         0b1,
        DEBUG_TRANSFORMATIONS =     0b10,
        DEBUG_TRANSLATIONS =        0b100,
        ONLY_FACING_TRIANGLES =     0b1000,
        DEPTH_SORT =                0b10000,
        LIGHTING =                  0b100000,
        DEBUG_BUMPMAP =             0b1000000,
//      GLOBAL_LIGHTS,
//      LOCAL_LIGHTS,
//      BIG_CHUNK_SPLITTING,
//      COLLISION_SPLITTING,
//      Z_BUFFER_SORT
        DEFAULT =                   ONLY_FACING_TRIANGLES | DEPTH_SORT
    };

    extern int bgColor;
    extern int flags;


    // FIXME: this is temporary
    // TODO: maybe create a Statistics sub-namespace for the like
    extern int primitivesRendered;

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

    namespace GlobalLight
    {
        extern Vector direction;
        extern int color;

        namespace Ambient
        {
            extern int color;
        }
    }

    // ==============================================================================
    //
    //      PUBLICLY ACCESSIBLE METHOD DECLARATIONS
    //
    // ==============================================================================

    bool init(SDL_Surface* surface, int f = Flags::DEFAULT);

    void setFlag(int f);
    void unsetFlag(int f);
    void toggleFlag(int f);
    bool hasFlag(int f);

    int mapColor(int r, int g, int b);

    Uint32 colorFromVectorDirection(const Vector& v);

    bool isOnScreen(const Vector& v);

    bool isOnScreen(const Primitive* p);

    SDL_Point positionOnScreen(const Vector& v);

    void render();
}

#endif