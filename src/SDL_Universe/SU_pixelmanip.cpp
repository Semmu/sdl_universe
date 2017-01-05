#include "SDL_Universe/SU_pixelmanip.h"
#include <utility>

void SDL_Surface_putpixel_nolock(SDL_Surface* s, int x, int y, int color)
{
    if (x < 0 || x >= s->w || y < 0 || y >= s->h)
        return;

    Uint32 *pixels = (Uint32 *)s->pixels;
    pixels[ ( y * s->w ) + x ] = color;
}

void SDL_Surface_putpixel_nolock(SDL_Surface* s, SDL_Point p, int c)
{
    SDL_Surface_putpixel_nolock(s, p.x, p.y, c);
}

void SDL_Surface_putpixel(SDL_Surface* s, int x, int y, int color)
{
    if (SDL_MUSTLOCK(s))
        SDL_LockSurface(s);

    SDL_Surface_putpixel_nolock(s, x, y, color);

    if (SDL_MUSTLOCK(s))
        SDL_UnlockSurface(s);
}



void SDL_Surface_draw_point(SDL_Surface* dst, int x1, int y1, int color)
{
    SDL_Surface_putpixel(dst, x1, y1, color);
}

void SDL_Surface_draw_point(SDL_Surface* dst, SDL_Point p, int c)
{
    SDL_Surface_draw_point(dst, p.x, p.y, c);
}



void SDL_Surface_draw_line(SDL_Surface* s, int x0, int y0, int x1, int y1, int color)
{
    // Bresenham's line algorithm
    //      http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    int dx = abs(x1-x0);
    int dy = abs(y1-y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    bool loop = true;


    if (SDL_MUSTLOCK(s))
        SDL_LockSurface(s);

    while(loop)
    {
        SDL_Surface_putpixel_nolock(s, x0, y0, color);
        if (x0 == x1 && y0 == y1)
            loop = false;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err = err - dy;
            x0 = x0 + sx;
        }

        if (e2 < dx)
        {
            err = err + dx;
            y0 = y0 + sy;
        }
    }

    if (SDL_MUSTLOCK(s))
        SDL_UnlockSurface(s);
}

void SDL_Surface_draw_line(SDL_Surface* dst, SDL_Point p1, SDL_Point p2, int c)
{
    SDL_Surface_draw_line(dst, p1.x, p1.y, p2.x, p2.y, c);
}



void SDL_Surface_draw_triangle(SDL_Surface* s, int x0, int y0, int x1, int y1, int x2, int y2, int c)
{
    // snippet from:
    //       http://www.codeproject.com/Tips/86354/draw-triangle-algorithm-D

    int width = s->w;
    int height = s->h;

    if (y1 > y2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    if (y0 > y1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    if (y1 > y2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    double dx_far = double(x2 - x0) / (y2 - y0 + 1);
    double dx_upper = double(x1 - x0) / (y1 - y0 + 1);
    double dx_low = double(x2 - x1) / (y2 - y1 + 1);
    double xf = x0;
    double xt = x0 + dx_upper;
    for (int y = y0; y <= (y2 > height-1 ? height-1 : y2); y++)
    {
        if (y >= 0)
        {
            for (int x = (xf > 0 ? int(xf) : 0); x <= (xt < width ? xt : width-1) ; x++)
                SDL_Surface_putpixel_nolock(s, x, y, c);
            for (int x = (xf < width ? int(xf) : width-1); x >= (xt > 0 ? xt : 0); x--)
                SDL_Surface_putpixel_nolock(s, x, y, c);
        }
        xf += dx_far;
        if (y < y1)
            xt += dx_upper;
        else
            xt += dx_low;
    }
}

void SDL_Surface_draw_triangle(SDL_Surface* s, SDL_Point p1, SDL_Point p2, SDL_Point p3, int c)
{
    SDL_Surface_draw_triangle(s, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c);
}