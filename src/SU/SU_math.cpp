#include "SU/SU_math.h"
#include "SU/SU_etc.h"
#include <iostream>
#include <sstream>

namespace SU
{
    Vector::Vector(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}

    double Vector::getLength() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    void Vector::setLength(const double l)
    {
        double ratio = l / getLength();

        x *= ratio;     y *= ratio;     z *= ratio;
    }



    Vector Vector::operator+(const Vector& v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    void Vector::operator+=(const Vector& v)
    {
        x += v.x;       y += v.y;       z += v.z;
    }

    Vector Vector::operator-(const Vector& v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    void Vector::operator-=(const Vector& v)
    {
        x -= v.x;       y -= v.y;       z -= v.z;
    }

    Vector Vector::operator-() const
    {
        return Vector(-x, -y, -z);
    }



    Vector Vector::operator*(const double d) const
    {
        return Vector(x * d, y * d, z * d);
    }
    Vector operator*(const double d, const Vector& v)
    {
        return Vector(v.x * d, v.y * d, v.z * d);
    }
    void Vector::operator*=(const double d)
    {
        x *= d;     y *= d;     z *= d;
    }

    Vector Vector::operator/(const double d) const
    {
        return Vector(x / d, y / d, z / d);
    }
    void Vector::operator/=(const double d)
    {
        x /= d;     y /= d;     z /= d;
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
        double s1 = position.x;         // "s" is a point of the plane
        double s2 = position.y;
        double s3 = position.z;

        double n1 = direction.x;        // "n" is the directional vector (diagonal) of the plane
        double n2 = direction.y;
        double n3 = direction.z;

        double p1 = l.position.x;       // "p" is a point of the line
        double p2 = l.position.y;
        double p3 = l.position.z;

        double v1 = l.direction.x;      // "v" is the direction of the line
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
}