#ifndef SU_pixelmanip_h
#define SU_pixelmanip_h

#include <SDL/SDL.h>
#include "SU/SU_etc.h"

void SDL_Surface_putpixel_nolock(SDL_Surface* s, int x, int y, int color);
void SDL_Surface_putpixel_nolock(SDL_Surface* s, SDL_Point p, int c);
void SDL_Surface_putpixel(SDL_Surface* s, int x, int y, int color);

void SDL_Surface_draw_point(SDL_Surface* dst, int x1, int y1, int color);
void SDL_Surface_draw_point(SDL_Surface* dst, SDL_Point p, int c);

void SDL_Surface_draw_line(SDL_Surface* s, int x0, int y0, int x1, int y1, int color);
void SDL_Surface_draw_line(SDL_Surface* dst, SDL_Point p1, SDL_Point p2, int c);

void SDL_Surface_draw_triangle(SDL_Surface* s, int x0, int y0, int x1, int y1, int x2, int y2, int c);
void SDL_Surface_draw_triangle(SDL_Surface* s, SDL_Point p1, SDL_Point p2, SDL_Point p3, int c);

#endif