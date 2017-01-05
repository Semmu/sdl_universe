#ifndef SU_h
#define SU_h

#if USING_SDL1
    #include <SDL/SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

#include <string>
#include <list>
#include "SDL_Universe/SU_math.h"
#include "SDL_Universe/SU_etc.h"
#include "SDL_Universe/SU_primitives.h"




namespace SU
{
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