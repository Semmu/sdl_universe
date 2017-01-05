#ifndef SU_eth_h
#define SU_eth_h

#include <cmath>

#if USING_SDL1
struct SDL_Point
{
    int x, y;
};
#endif

double randDouble(double max = 1.0);

double deg2rad(double d);
double rad2deg(double r);

#endif