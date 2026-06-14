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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Handle a SDL_Renderer.
 *****************************************************************************/

#ifndef RENDERER_H
#define RENDERER_H

#include "color.h"
#include "surface.h"

struct SDL_Renderer;

class Renderer
{
private:
  SDL_Renderer* renderer;
  
public:

  explicit Renderer() : renderer(NULL) { };
  explicit Renderer(SDL_Renderer* sdl_renderer) : renderer(sdl_renderer) { };
  ~Renderer() { Free(); };
  Renderer(const Renderer &src);
  Renderer &operator=(const Renderer &src);
  
  void Free();
  
  bool IsNull() const { return renderer == NULL; };
  Point2i GetSize() const;

  inline int GetWidth() const { return GetSize().x; };
  inline int GetHeight() const { return GetSize().y; };

  /**
   * Return the pointer of the SDL_Renderer.
   *
   * Should be used carefully.
   */
  SDL_Renderer *GetRenderer() { return renderer; };
  const SDL_Renderer *GetRenderer() const { return renderer; };
  
  int BoxColor(const Rectanglei &rect, const Color &color);
  int RectangleColor(const Rectanglei &rect, const Color &color, const uint &border_size = 1);
  int VlineColor(const uint &x, const uint &y1, const uint &y2, const Color &color);
  int HlineColor(const uint &x1, const uint &x2, const uint &y, const Color &color);
  int LineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color);
  int AALineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color);
  int AAFadingLineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color1, const Color &color2);
  int CircleColor(const uint &x, const uint &y, const uint &rad, const Color &color);
  int FilledCircleColor(const uint &x, const uint &y, const uint &rad, const Color &color);
  int PieColor(const uint &x, const uint &y, const uint &rad, const int &start, const int &end, const Color &color);
  int FilledPieColor(const uint &x, const uint &y, const uint &rad, const int &start, const int &end, const Color &color);
  int AAPolygonColor(const Sint16 * vx, const Sint16 * vy, const int n, const Color & color);
  int AAPolygonColor(std::list<Point2i> polygon, const Color & color);
  int FilledPolygon(const Sint16 * vx, const Sint16 * vy, const int n, const Color & color);
  int FilledPolygon(std::list<Point2i> polygon, const Color & color);
  int TexturedPolygon(const Sint16 * vx, const Sint16 * vy, const int n, const Surface *texture, const int texture_dx, const int texture_dy);
  int TexturedPolygon(std::list<Point2i> polygon, const Surface *texture);
};

#endif
