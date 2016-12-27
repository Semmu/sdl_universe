#include "SU/SU.h"
#include "SU/SU_math.h"
#include "SU/SU_pixelmanip.h"
#include <iostream>
#include <algorithm>

namespace SU
{
    // ==============================================================================
    //
    //      PUBLICLY ACCESSIBLE VARIABLE DEFINITIONS
    //
    // ==============================================================================


    int bgColor = 0;
    int flags = 0;

    // FIXME: this is temporary
    int primitivesRendered = 0;


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

    namespace GlobalLight
    {
        Vector direction = Vector(-1, -1.3, -1.2);
        int color = 0x00ff00ff;

        namespace Ambient
        {
            int color = 0xffffffff;
        }
    }




    // ==============================================================================
    //
    //      PRIVATE VARIABLE DECLARATIONS AND DEFINITIONS
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

        Shaders::BumpMapShader* bumpMapShader = new Shaders::BumpMapShader();
    }





    // ==============================================================================
    //
    //      CUSTOM TYPE AND CLASS IMPLEMENTATIONS
    //
    // ==============================================================================

    void Mesh::add(Primitive* p)
    {
        primitives.push_back(p);
    }

    void Mesh::add(Quad* q)
    {
        add(new Triangle(q->P1, q->P2, q->P3, q->color, q->lighted));
        add(new Triangle(q->P1, q->P3, q->P4, q->color, q->lighted));

        delete q;
    }



    Object::Object() : enabled(true), flipTriangles(false), mesh(NULL), position(Vector(0, 0, 0)), transforming(false), X(Vector(1, 0, 0)), Y(Vector(0, 1, 0)), Z(Vector(0, 0, 1)), parent(NULL)
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

    void Object::scale(double r)
    {
        X *= r;
        Y *= r;
        Z *= r;
    }

    Uint8 Object::getLevel()
    {
        if (parent == NULL)
            return 1;
        else
            return 1 + parent->getLevel();
    }

    namespace Shaders
    {
        void shadePoint(Point* p)
        {
            for (auto shader : shaders)
                shader->shadePoint(p);
        }

        void shadeSegment(Segment* s)
        {
            for (auto shader : shaders)
                shader->shadeSegment(s);
        }

        void shadeTriangle(Triangle* t)
        {
            for (auto shader : shaders)
                shader->shadeTriangle(t);
        }

        std::list<Shader*> shaders;

        void Shader::shadePoint(Point* p) {}
        void Shader::shadeSegment(Segment* s) {}
        void Shader::shadeTriangle(Triangle* t) {}

        void BumpMapShader::shadePoint(Point* p)
        {
            p->color = WHITE;
        }

        void BumpMapShader::shadeSegment(Segment* s)
        {
            s->color = WHITE;
        }

        void BumpMapShader::shadeTriangle(Triangle* t)
        {
            t->lighted = false;
            t->color = colorFromVectorDirection(t->getDirection());
        }
    }





    // ==============================================================================
    //
    //      PUBLICLY ACCESSIBLE METHOD IMPLEMENTATIONS
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

    bool hasFlag(int f)
    {
        return (flags & f);
    }

    int mapColor(int r, int g, int b)
    {
        return SDL_MapRGB(surface->format, r, g, b);
    }

    Uint32 computeColor(Uint32 c, const Vector& dir)
    {
        Uint8 r, g, b;
        SDL_GetRGB(c, surface->format, &r, &g, &b);
        double primitiveR = r / 256.0,
               primitiveG = g / 256.0,
               primitiveB = b / 256.0;

        SDL_GetRGB(GlobalLight::color, surface->format, &r, &g, &b);
        double globalR = r / 256.0,
               globalG = g / 256.0,
               globalB = b / 256.0;

        SDL_GetRGB(GlobalLight::Ambient::color, surface->format, &r, &g, &b);
        double ambientR = r / 256.0,
               ambientG = g / 256.0,
               ambientB = b / 256.0;

        double facingAmount = dir.angleTo(GlobalLight::direction) / M_PI;

        #define FORMULA(prim, amb, glob) (prim * glob * facingAmount * 256 + prim * amb)

        r = FORMULA(primitiveR, globalR, ambientR);
        g = FORMULA(primitiveG, globalG, ambientG);
        b = FORMULA(primitiveB, globalB, ambientB);

        return mapColor(r, g, b);
    }

    Uint32 colorFromVectorDirection(const Vector& v)
    {
        Vector n = v.getNormalized();

        double r = n.angleTo(Vector(1, 0, 0));
        double g = n.angleTo(Vector(0, 1, 0));
        double b = n.angleTo(Vector(0, 0, 1));

        return SU::mapColor(r / M_PI * 255, g / M_PI * 255, b / M_PI * 255);
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
        //      at least one Vector is on the screen

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
        if (!o->enabled)
            return;

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

        if (hasFlag(Flags::DEBUG_TRANSFORMATIONS))
        {
            // we will draw the transformation axes

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

        // also the translation (a.k.a. position) segment
        if (hasFlag(Flags::DEBUG_TRANSLATIONS))
            primitivesToRender.push_back(new SU::Segment((o->parent == NULL ? Vector(0, 0, 0) : o->parent->resultantPosition), o->resultantPosition, SU::mapColor(o->getLevel() & 0b00000100 ? 255 : 0, o->getLevel() & 0b00000010 ? 255 : 0, o->getLevel() & 0b00000001 ? 255 : 0)));

        if (o->mesh != NULL)
        {
            for (Primitive* p : o->mesh->primitives)
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
                                                 p->color, static_cast<SU::Triangle*>(p)->lighted);
                        }
                        else
                        {
                            t = new SU::Triangle(static_cast<SU::Triangle*>(p)->P1 + o->resultantPosition,
                                                 static_cast<SU::Triangle*>(p)->P2 + o->resultantPosition,
                                                 static_cast<SU::Triangle*>(p)->P3 + o->resultantPosition,
                                                 p->color, static_cast<SU::Triangle*>(p)->lighted);
                        }

                        if (o->flipTriangles)
                            t->flip();

                        if (hasFlag(Flags::DEBUG_WIREFRAMING) ? true : (hasFlag(Flags::ONLY_FACING_TRIANGLES) ? isFacingTheCamera(t) : true))
                            primitivesToRender.push_back(t);
                        else
                            delete t;
                    }
                    break;

                    default: break;
                }
            }
        }

        if (o->children.size() > 0)
            for(Object *ch : o->children)
                processObject(ch);
    }

    void render()
    {
        if (SU::hasFlag(SU::Flags::DEBUG_BUMPMAP))
            SU::Shaders::shaders.push_back(SU::bumpMapShader);

        // FIXME: FOV value restriction should be somewhere else?

        if (Camera::FOV < 10)
            Camera::FOV = 10;

        if (Camera::FOV > 170)
            Camera::FOV = 170;


        SDL_FillRect(surface, NULL, bgColor);
        primitivesRendered = 0;


        /*
         *
         *  INITIALIZING THE CAMERA PLANES AND VECTORS
         *
         */

         cameraPlane = Plane(Camera::position + Camera::lookDirection.getNormalized() * Camera::viewDistanceMin, Camera::lookDirection);

         Camera::rightDirection = Camera::upDirection.crossProduct(Camera::lookDirection);

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
         *  COLLECTING WHAT PRIMITIVES TO RENDER
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
         *  SORTING THE PRIMITIVES
         *
         */

         if (hasFlag(Flags::DEPTH_SORT))
             primitivesToRender.sort(Primitive::sort);



        /*
         *
         *  RENDERING THE PRIMITIVES
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

                        Shaders::shadePoint(pt);

                        SDL_Point sp = positionOnScreen(pt->P1);
                        SDL_Surface_draw_point(surface, sp, pt->color);

                        primitivesRendered++;
                    }
                    break;

                    case SU::Primitive::Type::SEGMENT:
                    {
                        Segment* l = static_cast<Segment*>(p);

                        Shaders::shadeSegment(l);

                        SDL_Point p1 = positionOnScreen(l->P1);
                        SDL_Point p2 = positionOnScreen(l->P2);
                        SDL_Surface_draw_line(surface, p1, p2, p->color);

                        primitivesRendered++;
                    }
                    break;

                    case SU::Primitive::Type::TRIANGLE:
                    {
                        Triangle* t = static_cast<Triangle*>(p);

                        Shaders::shadeTriangle(t);

                        SDL_Point p1 = positionOnScreen(t->P1);
                        SDL_Point p2 = positionOnScreen(t->P2);
                        SDL_Point p3 = positionOnScreen(t->P3);

                        if (hasFlag(Flags::DEBUG_WIREFRAMING))
                        {
                            SDL_Surface_draw_line(surface, p1, p2, p->color);
                            SDL_Surface_draw_line(surface, p1, p3, p->color);
                            SDL_Surface_draw_line(surface, p3, p2, p->color);

                            primitivesRendered += 3;
                        }
                        else
                        {
                            int color;

                            if (hasFlag(Flags::LIGHTING) && t->lighted)
                                color = computeColor(t->color, t->getDirection());
                            else
                                color = t->color;

                            SDL_Surface_draw_triangle(surface, p1, p2, p3, color);
                            primitivesRendered++;
                        }
                    }
                    break;

                    default: break;
                }
            }
        }


        SU::Shaders::shaders.erase(std::remove(SU::Shaders::shaders.begin(), SU::Shaders::shaders.end(), SU::bumpMapShader), SU::Shaders::shaders.end());


        /*
         *
         *  FREEING UP MEMORY AFTER RENDER
         *
         */

        for(Primitive* p : primitivesToRender)
            delete p;

        primitivesToRender.clear();
    }
}