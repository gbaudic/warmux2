/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * fading effect largely copy/paste from SDL_Gfx
 *****************************************************************************/

#ifndef FADING_EFFECT_H
#define FADING_EFFECT_H

#include <SDL2_gfxPrimitives.h>
#include <SDL.h>

/* ----- Defines for pixel clipping tests */

#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1

/* AA Line */

#define AAlevels 256
#define AAbits 8

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

int clipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom);

int clipLine(SDL_Rect &rect, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2);

int clipLine(SDL_Surface * dst, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2);

int clipLine(SDL_Renderer * renderer, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2);

inline int _putPixelAlpha(SDL_Surface * surface, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha)
{
  SDL_PixelFormat *fmt = surface->format;
  Uint32 Rmask = fmt->Rmask, Gmask = fmt->Gmask, Bmask = fmt->Bmask, Amask = fmt->Amask;
  Uint32 R, G, B, A = 0;

  if (x >= clip_xmin(surface) && x <= clip_xmax(surface)
    && y >= clip_ymin(surface) && y <= clip_ymax(surface)) {

    switch (fmt->BytesPerPixel) {
    case 2:        /* Probably 15-bpp or 16-bpp */
      if (alpha == 255) {
        *((Uint16 *) surface->pixels + y * surface->pitch / 2 + x) = color;
      } else {
        Uint16 *pixel = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;
        Uint32 dc = *pixel;

        R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
        G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
        B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
        if (Amask)
          A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

        *pixel = R | G | B | A;
      }
      break;

    case 3:        /* Slow 24-bpp mode, usually not used */
      {
        Uint8 *pix = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;
        Uint8 rshift8 = fmt->Rshift / 8;
        Uint8 gshift8 = fmt->Gshift / 8;
        Uint8 bshift8 = fmt->Bshift / 8;
        Uint8 ashift8 = fmt->Ashift / 8;


        if (alpha == 255) {
          *(pix + rshift8) = color >> fmt->Rshift;
          *(pix + gshift8) = color >> fmt->Gshift;
          *(pix + bshift8) = color >> fmt->Bshift;
          *(pix + ashift8) = color >> fmt->Ashift;
        } else {
          Uint8 dR, dG, dB, dA = 0;
          Uint8 sR, sG, sB, sA = 0;

          pix = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;

          dR = *((pix) + rshift8);
          dG = *((pix) + gshift8);
          dB = *((pix) + bshift8);
          dA = *((pix) + ashift8);

          sR = (color >> fmt->Rshift) & 0xff;
          sG = (color >> fmt->Gshift) & 0xff;
          sB = (color >> fmt->Bshift) & 0xff;
          sA = (color >> fmt->Ashift) & 0xff;

          dR = dR + ((sR - dR) * alpha >> 8);
          dG = dG + ((sG - dG) * alpha >> 8);
          dB = dB + ((sB - dB) * alpha >> 8);
          dA = dA + ((sA - dA) * alpha >> 8);

          *((pix) + rshift8) = dR;
          *((pix) + gshift8) = dG;
          *((pix) + bshift8) = dB;
          *((pix) + ashift8) = dA;
        }
      }
      break;

    case 4:        /* Probably 32-bpp */
      if (alpha == 255) {
        *((Uint32 *) surface->pixels + y * surface->pitch / 4 + x) = color;
      } else {
        Uint32 *pixel = (Uint32 *) surface->pixels + y * surface->pitch / 4 + x;
        Uint32 dc = *pixel;

        R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
        G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
        B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
        if (Amask)
          A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

        *pixel = R | G | B | A;
      }
      break;
    }
  }

  return (0);
}

inline int pixelColorNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color)
{
  Uint8 alpha;
  Uint32 mcolor;
  int result = 0;

  /*
   * Setup color
   */
  alpha = color & 0x000000ff;
  mcolor =
    SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
                (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

  /*
   * Draw
   */
  result = _putPixelAlpha(dst, x, y, mcolor, alpha);

  return result;
}

inline int pixelColorNolock(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint32 color)
{
  int result = 0;

  /*
   * Setup color
   */
  result = SDL_SetRenderDrawColor(renderer, (color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, color & 0x000000ff);

  if (result == 0) {
    /*
     * Draw
     */
    result = SDL_RenderDrawPoint(renderer, x, y);
  }

  return result;
}

inline int pixelColorWeightNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight)
{
  /* Get alpha */
  Uint32 a = color & 0x000000ffU;

  /*
   * Modify Alpha by weight
   */
  a = (a * weight) >> 8;

  return pixelColorNolock(dst, x, y, (color & 0xffffff00U) | a);
}

inline int pixelColorWeightNolock(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint32 color, Uint32 weight)
{
  /* Get alpha */
  Uint32 a = color & 0x000000ffU;

  /*
   * Modify Alpha by weight
   */
  a = (a * weight) >> 8;

  return pixelColorNolock(renderer, x, y, (color & 0xffffff00U) | a);
}

inline int interpolateInt(int start, int stop, float step)
{
  float diff = stop - start;
  return start + (int)(diff * step);
}

inline Uint32 interpolateColor(Uint32 color1, Uint32 color2, float step)
{
  int c1 = interpolateInt(color1 & 0xFF000000, color2 & 0xFF000000, step) & 0xFF000000;
  int c2 = interpolateInt(color1 & 0x00FF0000, color2 & 0x00FF0000, step) & 0x00FF0000;
  int c3 = interpolateInt(color1 & 0x0000FF00, color2 & 0x0000FF00, step) & 0x0000FF00;
  int c4 = interpolateInt(color1 & 0x000000FF, color2 & 0x000000FF, step) & 0x000000FF;
  return (Uint32)(c1 | c2 | c3 | c4);
}

int aafadingLineColorInt(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                         Uint32 color1, Uint32 color2, int draw_endpoint);

int aafadingLineColorInt(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                         Uint32 color1, Uint32 color2, int draw_endpoint);

int aafadingLineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1,
                      Sint16 x2, Sint16 y2, Uint32 color1, Uint32 color2);


int aafadingLineColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1,
                      Sint16 x2, Sint16 y2, Uint32 color1, Uint32 color2);


#endif /* FADING_EFFECT_H */
