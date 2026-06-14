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

#include "graphic/fading_effect.h"

int clipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
  int code = 0;

  if (x < left) {
    code |= CLIP_LEFT_EDGE;
  } else if (x > right) {
    code |= CLIP_RIGHT_EDGE;
  }
  if (y < top) {
    code |= CLIP_TOP_EDGE;
  } else if (y > bottom) {
    code |= CLIP_BOTTOM_EDGE;
  }
  return code;
}

int clipLine(SDL_Rect &rect, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2)
{
  Sint16 left, right, top, bottom;
  int code1, code2;
  int draw = 0;
  Sint16 swaptmp;
  float m;

  /*
   * Get clipping boundary
   */
  left = rect.x;
  right = rect.x + rect.w - 1;
  top = rect.y;
  bottom = rect.y + rect.h - 1;

  while (1) {
    code1 = clipEncode(*x1, *y1, left, top, right, bottom);
    code2 = clipEncode(*x2, *y2, left, top, right, bottom);
    if (CLIP_ACCEPT(code1, code2)) {
      draw = 1;
      break;
    } else if (CLIP_REJECT(code1, code2))
      break;
    else {
      if (CLIP_INSIDE(code1)) {
        swaptmp = *x2;
        *x2 = *x1;
        *x1 = swaptmp;
        swaptmp = *y2;
        *y2 = *y1;
        *y1 = swaptmp;
        swaptmp = code2;
        code2 = code1;
        code1 = swaptmp;
      }
      if (*x2 != *x1) {
        m = (*y2 - *y1) / (float) (*x2 - *x1);
      } else {
        m = 1.0f;
      }
      if (code1 & CLIP_LEFT_EDGE) {
        *y1 += (Sint16) ((int)((left - *x1) * m));
        *x1 = left;
      } else if (code1 & CLIP_RIGHT_EDGE) {
        *y1 += (Sint16) ((int)((right - *x1) * m));
        *x1 = right;
      } else if (code1 & CLIP_BOTTOM_EDGE) {
        if (*x2 != *x1) {
          *x1 += (Sint16) ((int)((bottom - *y1) / m));
        }
        *y1 = bottom;
      } else if (code1 & CLIP_TOP_EDGE) {
        if (*x2 != *x1) {
          *x1 += (Sint16) ((int)((top - *y1) / m));
        }
        *y1 = top;
      }
    }
  }

  return draw;
}

int clipLine(SDL_Surface * dst, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2)
{
  return clipLine(dst->clip_rect, x1, y1, x2, y2);
}

int clipLine(SDL_Renderer * renderer, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2)
{
  SDL_Rect rect;
  SDL_RenderGetClipRect(renderer, &rect);
  return clipLine(rect, x1, y1, x2, y2);
}

int aafadingLineColorInt(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                         Uint32 color1, Uint32 color2, int draw_endpoint)
{
  Sint32 xx0, yy0, xx1, yy1;
  int result;
  int dx, dy, tmp, xdir;
  Uint32 color = color1;
  float step;

  if (y1 == y2) {
    /* Horizontal line */
    dx = x2-x1;
    xdir = 1;
    if (dx == 0) {
      return 0;
    }
    if (dx < 0) {
      dx = -dx;
      xdir = -1;
    }
    step = dx;
    while (--dx) {
      color = interpolateColor(color2, color1, dx / step);
      result |= pixelColorNolock(dst, x1, y1, color);
      x1 += xdir;
    }
  }
  if (y1 > y2) {
    color = color1;
    color1 = color2;
    color2 = color;
  }

  /*
   * Clip line and test if we have to draw
   */
  if (!(clipLine(dst, &x1, &y1, &x2, &y2))) {
    return (0);
  }

  /*
   * Keep on working with 32bit numbers
   */
  xx0 = x1;
  yy0 = y1;
  xx1 = x2;
  yy1 = y2;

  /*
   * Reorder points if required
   */
  if (yy0 > yy1) {
    tmp = yy0;
    yy0 = yy1;
    yy1 = tmp;
    tmp = xx0;
    xx0 = xx1;
    xx1 = tmp;
  }

  /*
   * Calculate distance
   */
  dx = xx1 - xx0;
  dy = yy1 - yy0;

  /*
   * Adjust for negative dx and set xdir
   */
  if (dx >= 0) {
    xdir = 1;
  } else {
    xdir = -1;
    dx = (-dx);
  }

  /* Lock surface */
  if (SDL_MUSTLOCK(dst)) {
    if (SDL_LockSurface(dst) < 0) {
      return (-1);
    }
  }

  /*
   * Draw the initial pixel in the foreground color
   */
  result = pixelColorNolock(dst, x1, y1, color1);

  /*
   * Check for special cases
   */
  if (dx == 0 && dy == 0) {
    /* nothing to do */
  } else if (dx == 0) {
    /* Vertical line */
    step = dy;
    while (--dy) {
      color = interpolateColor(color2, color1, dy / step);
      result |= pixelColorNolock(dst, x1, ++yy0, color);
    }
  } else if (dy == 0) {
  } else if (dx == dy) {
    /* Diagonal line */
    step = dx;
    while (--dx) {
      color = interpolateColor(color2, color1, dx / step);
      result |= pixelColorNolock(dst, xx0, ++yy0, color);
      xx0 += xdir;
    }
  } else {
    Uint32 erracc = 0; /* Zero accumulator */
    Uint32 intshift = 32 - AAbits; /* # of bits by which to shift erracc to get intensity level */
    Uint32 erradj;
    Uint32 erracctmp, wgt;

    /*
     * x-major or y-major?
     */
    if (dy > dx) {

      /*
       * y-major.  Calculate 16-bit fixed point fractional part of a
       * pixel that X advances every time Y advances 1 pixel, truncating
       * the result so that we won't overrun the endpoint along the
       * X axis
       */
      /*
       * Not-so-portable version:
       * erradj = ((Uint64)dx << 32) / (Uint64)dy;
       */
      erradj = ((dx << 16) / dy) << 16;

      /*
       * draw all pixels other than the first and last
       */
      int x0pxdir = xx0 + xdir;
      step = dy;
      while (--dy) {
        color = interpolateColor(color2, color1, dy / step);
        erracctmp = erracc;
        erracc += erradj;
        if (erracc <= erracctmp) {
          /*
           * rollover in error accumulator, x coord advances
           */
          xx0 = x0pxdir;
          x0pxdir += xdir;
        }
        yy0++;              /* y-major so always advance Y */

        /*
         * the AAbits most significant bits of erracc give us the
         * intensity weighting for this pixel, and the complement of
         * the weighting for the paired pixel.
         */
        wgt = (erracc >> intshift) & 255;
        result |= pixelColorWeightNolock (dst, xx0, yy0, color, 255 - wgt);
        result |= pixelColorWeightNolock (dst, x0pxdir, yy0, color, wgt);
      }

    } else {

      /*
       * x-major line.  Calculate 16-bit fixed-point fractional part of
       * a pixel that Y advances each time X advances 1 pixel truncating
       * the result so that we won't overrun the endpoint along the
       * X axis.
       */
      /*
       * Not-so-portable version:
       * erradj = ((Uint64)dy << 32) / (Uint64)dx;
       */
      erradj = ((dy << 16) / dx) << 16;

      /*
       * draw all pixels other than the first and last
       */
      int y0p1 = yy0 + 1;
      step = dx;
      while (--dx) {
        color = interpolateColor(color2, color1, dx / step);
        erracctmp = erracc;
        erracc += erradj;
        if (erracc <= erracctmp) {
          /*
           * Accumulator turned over, advance y
           */
          yy0 = y0p1;
          y0p1++;
        }
        xx0 += xdir;        /* x-major so always advance X */
        /*
         * the AAbits most significant bits of erracc give us the
         * intensity weighting for this pixel, and the complement of
         * the weighting for the paired pixel.
         */
        wgt = (erracc >> intshift) & 255;
        result |= pixelColorWeightNolock (dst, xx0, yy0, color, 255 - wgt);
        result |= pixelColorWeightNolock (dst, xx0, y0p1, color, wgt);
      }
    }
  }
  /*
   * Do we have to draw the endpoint
   */
  if (draw_endpoint) {
    /*
     * Draw final pixel, always exactly intersected by the line and doesn't
     * need to be weighted.
     */
    result |= pixelColorNolock (dst, x2, y2, color2);
  }

  /* Unlock surface */
  if (SDL_MUSTLOCK(dst)) {
    SDL_UnlockSurface(dst);
  }

  return (result);
}

int aafadingLineColorInt(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                         Uint32 color1, Uint32 color2, int draw_endpoint)
{
  Sint32 xx0, yy0, xx1, yy1;
  int result;
  int dx, dy, tmp, xdir;
  Uint32 color = color1;
  float step;

  if (y1 == y2) {
    /* Horizontal line */
    dx = x2-x1;
    xdir = 1;
    if (dx == 0) {
      return 0;
    }
    if (dx < 0) {
      dx = -dx;
      xdir = -1;
    }
    step = dx;
    while (--dx) {
      color = interpolateColor(color2, color1, dx / step);
      result |= pixelColorNolock(renderer, x1, y1, color);
      x1 += xdir;
    }
  }
  if (y1 > y2) {
    color = color1;
    color1 = color2;
    color2 = color;
  }

  /*
   * Clip line and test if we have to draw
   */
  if (!(clipLine(renderer, &x1, &y1, &x2, &y2))) {
    return (0);
  }

  /*
   * Keep on working with 32bit numbers
   */
  xx0 = x1;
  yy0 = y1;
  xx1 = x2;
  yy1 = y2;

  /*
   * Reorder points if required
   */
  if (yy0 > yy1) {
    tmp = yy0;
    yy0 = yy1;
    yy1 = tmp;
    tmp = xx0;
    xx0 = xx1;
    xx1 = tmp;
  }

  /*
   * Calculate distance
   */
  dx = xx1 - xx0;
  dy = yy1 - yy0;

  /*
   * Adjust for negative dx and set xdir
   */
  if (dx >= 0) {
    xdir = 1;
  } else {
    xdir = -1;
    dx = (-dx);
  }

  /*
   * Draw the initial pixel in the foreground color
   */
  result = pixelColorNolock(renderer, x1, y1, color1);

  /*
   * Check for special cases
   */
  if (dx == 0 && dy == 0) {
    /* nothing to do */
  } else if (dx == 0) {
    /* Vertical line */
    step = dy;
    while (--dy) {
      color = interpolateColor(color2, color1, dy / step);
      result |= pixelColorNolock(renderer, x1, ++yy0, color);
    }
  } else if (dy == 0) {
  } else if (dx == dy) {
    /* Diagonal line */
    step = dx;
    while (--dx) {
      color = interpolateColor(color2, color1, dx / step);
      result |= pixelColorNolock(renderer, xx0, ++yy0, color);
      xx0 += xdir;
    }
  } else {
    Uint32 erracc = 0; /* Zero accumulator */
    Uint32 intshift = 32 - AAbits; /* # of bits by which to shift erracc to get intensity level */
    Uint32 erradj;
    Uint32 erracctmp, wgt;

    /*
     * x-major or y-major?
     */
    if (dy > dx) {

      /*
       * y-major.  Calculate 16-bit fixed point fractional part of a
       * pixel that X advances every time Y advances 1 pixel, truncating
       * the result so that we won't overrun the endpoint along the
       * X axis
       */
      /*
       * Not-so-portable version:
       * erradj = ((Uint64)dx << 32) / (Uint64)dy;
       */
      erradj = ((dx << 16) / dy) << 16;

      /*
       * draw all pixels other than the first and last
       */
      int x0pxdir = xx0 + xdir;
      step = dy;
      while (--dy) {
        color = interpolateColor(color2, color1, dy / step);
        erracctmp = erracc;
        erracc += erradj;
        if (erracc <= erracctmp) {
          /*
           * rollover in error accumulator, x coord advances
           */
          xx0 = x0pxdir;
          x0pxdir += xdir;
        }
        yy0++;              /* y-major so always advance Y */

        /*
         * the AAbits most significant bits of erracc give us the
         * intensity weighting for this pixel, and the complement of
         * the weighting for the paired pixel.
         */
        wgt = (erracc >> intshift) & 255;
        result |= pixelColorWeightNolock (renderer, xx0, yy0, color, 255 - wgt);
        result |= pixelColorWeightNolock (renderer, x0pxdir, yy0, color, wgt);
      }

    } else {

      /*
       * x-major line.  Calculate 16-bit fixed-point fractional part of
       * a pixel that Y advances each time X advances 1 pixel truncating
       * the result so that we won't overrun the endpoint along the
       * X axis.
       */
      /*
       * Not-so-portable version:
       * erradj = ((Uint64)dy << 32) / (Uint64)dx;
       */
      erradj = ((dy << 16) / dx) << 16;

      /*
       * draw all pixels other than the first and last
       */
      int y0p1 = yy0 + 1;
      step = dx;
      while (--dx) {
        color = interpolateColor(color2, color1, dx / step);
        erracctmp = erracc;
        erracc += erradj;
        if (erracc <= erracctmp) {
          /*
           * Accumulator turned over, advance y
           */
          yy0 = y0p1;
          y0p1++;
        }
        xx0 += xdir;        /* x-major so always advance X */
        /*
         * the AAbits most significant bits of erracc give us the
         * intensity weighting for this pixel, and the complement of
         * the weighting for the paired pixel.
         */
        wgt = (erracc >> intshift) & 255;
        result |= pixelColorWeightNolock (renderer, xx0, yy0, color, 255 - wgt);
        result |= pixelColorWeightNolock (renderer, xx0, y0p1, color, wgt);
      }
    }
  }
  /*
   * Do we have to draw the endpoint
   */
  if (draw_endpoint) {
    /*
     * Draw final pixel, always exactly intersected by the line and doesn't
     * need to be weighted.
     */
    result |= pixelColorNolock (renderer, x2, y2, color2);
  }

  return (result);
}

int aafadingLineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1,
                      Sint16 x2, Sint16 y2, Uint32 color1, Uint32 color2)
{
  return aafadingLineColorInt(dst, x1, y1, x2, y2, color1, color2, 1);
}

int aafadingLineColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1,
                      Sint16 x2, Sint16 y2, Uint32 color1, Uint32 color2)
{
  return aafadingLineColorInt(renderer, x1, y1, x2, y2, color1, color2, 1);
}
