#include "SDL_Universe/SU_etc.h"
#include <cmath>
#include <iostream>

double randDouble(double max)
{
    return double(rand()) / RAND_MAX * max;
}

double deg2rad(double d)
{
    return d / 180 * M_PI;
}

double rad2deg(double r)
{
    return r / M_PI * 180;
}
