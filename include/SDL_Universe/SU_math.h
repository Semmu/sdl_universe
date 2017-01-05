#ifndef SU_math_h
#define SU_math_h

#include <string>

namespace SU
{
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

        Vector operator-() const;


        Vector operator*(const double d) const;
        friend Vector operator*(const double d, const Vector& v);
        void operator*=(const double d);

        Vector operator/(const double d) const;
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
}

#endif