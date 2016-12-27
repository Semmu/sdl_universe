#include "SU/SU_math.h"
#include "SU/SU_primitives.h"
#include "SU/SU.h"

namespace SU
{
    Primitive::Primitive(int c) : color(c) {}
    Primitive::~Primitive() {}

    bool Primitive::sort(const Primitive* p1, const Primitive* p2)
    {
        return (p1->getCenter() - SU::Camera::position).getLength() > (p2->getCenter() - SU::Camera::position).getLength();
    }


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



    Triangle::Triangle(Vector p1, Vector p2, Vector p3, int c, bool l) : Primitive(c), P1(p1), P2(p2), P3(p3), lighted(l) {}

    Triangle::Triangle(double x1, double y1, double z1,
                       double x2, double y2, double z2,
                       double x3, double y3, double z3, int c, bool l) : Primitive(c), lighted(l)
    {
        P1 = Vector(x1, y1, z1);
        P2 = Vector(x2, y2, z2);
        P3 = Vector(x3, y3, z3);
    }

    void Triangle::flip()
    {
        Vector t = P1;
        P1 = P2;
        P2 = t;
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



    Quad::Quad(Vector p1, Vector p2, Vector p3, Vector p4, int c, bool l) : P1(p1), P2(p2), P3(p3), P4(p4), color(c), lighted(l) {}

    Quad::Quad(double x1, double y1, double z1,
               double x2, double y2, double z2,
               double x3, double y3, double z3,
               double x4, double y4, double z4, int c, bool l) : color(c), lighted(l)
    {
        P1 = Vector(x1, y1, z1);
        P2 = Vector(x2, y2, z2);
        P3 = Vector(x3, y3, z3);
        P4 = Vector(x4, y4, z4);
    }
}