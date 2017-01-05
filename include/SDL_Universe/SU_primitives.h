#ifndef SU_primitives_h
#define SU_primitives_h

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
}

#endif