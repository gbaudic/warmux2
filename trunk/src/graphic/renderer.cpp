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

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "graphic/renderer.h"


int Renderer::BoxColor(const Rectanglei &rect, const Color &color)
{
  if (rect.IsSizeZero())
    return 0;

  Point2i ptBR = rect.GetBottomRightPoint();

  return boxRGBA(renderer, rect.GetPositionX(), rect.GetPositionY(), ptBR.GetX(), ptBR.GetY(), color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::RectangleColor(const Rectanglei &rect, const Color &color,
                            const uint &border_size)
{
  if (rect.IsSizeZero())
    return 0;

  Point2i ptBR = rect.GetBottomRightPoint();

  if (border_size == 1)
    return rectangleRGBA(renderer, rect.GetPositionX(), rect.GetPositionY(), ptBR.GetX(), ptBR.GetY(), color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

  // top border
  boxRGBA (renderer,
           rect.GetPositionX(), rect.GetPositionY(), ptBR.GetX(), rect.GetPositionY()+border_size,
           color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

  // bottom border
  boxRGBA (renderer,
           rect.GetPositionX(), ptBR.GetY() - border_size, ptBR.GetX(), ptBR.GetY(),
           color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

  // left border
  boxRGBA (renderer,
           rect.GetPositionX(), rect.GetPositionY() + border_size, rect.GetPositionX()+border_size, ptBR.GetY()-border_size,
           color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

  // right border
  boxRGBA (renderer,
           ptBR.GetX() - border_size, rect.GetPositionY() + border_size, ptBR.GetX(), ptBR.GetY()-border_size,
           color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

  return 1;
}

int Renderer::VlineColor(const uint &x, const uint &y1, const uint &y2, const Color &color)
{
  return vlineRGBA(renderer, x, y1, y2, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::HlineColor(const uint &x1, const uint &x2, const uint &y, const Color &color)
{
  return hlineRGBA(renderer, x1, x2, y, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::LineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color)
{
  return lineRGBA(renderer, x1, y1, x2, y2, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::AALineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color)
{
  return aalineRGBA(renderer, x1, y1, x2, y2, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::AAFadingLineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color1, const Color &color2)
{
  return aafadingLineColor(renderer, x1, y1, x2, y2,color1.GetColor(), color2.GetColor());
}

int Renderer::CircleColor(const uint &x, const uint &y, const uint &rad, const Color &color)
{
  return circleRGBA(renderer, x, y, rad, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::FilledCircleColor(const uint &x, const uint &y, const uint &rad, const Color &color)
{
  return filledCircleRGBA(renderer, x, y, rad, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::PieColor(const uint &x, const uint &y, const uint &rad, const int &start, const int &end, const Color &color)
{
  return pieRGBA(renderer, x, y, rad, start, end, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::FilledPieColor(const uint &x, const uint &y, const uint &rad, const int &start, const int &end, const Color &color)
{
  return filledPieRGBA(renderer, x, y, rad, start, end, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::AAPolygonColor(const Sint16 * vx, const Sint16 * vy, const int n, const Color & color) {
  return aapolygonRGBA(renderer, vx, vy, n, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::AAPolygonColor(std::list<Point2i> polygon, const Color & color)
{
  Sint16 * vx, * vy;
  vx = new Sint16[polygon.size()];
  vy = new Sint16[polygon.size()];
  int i = 0;
  for (std::list<Point2i>::iterator point = polygon.begin(); point != polygon.end(); point++, i++) {
    vx[i] = point->x;
    vy[i] = point->y;
  }
  int result = aapolygonRGBA(renderer, vx, vy, polygon.size(), color.GetRed(),
                             color.GetGreen(), color.GetBlue(), color.GetAlpha());
  delete[] vx;
  delete[] vy;
  return result;
}

int Renderer::FilledPolygon(const Sint16 * vx, const Sint16 * vy, const int n, const Color & color)
{
  // Internal static leak in sdl_gfx
  return filledPolygonRGBA(renderer, vx, vy, n, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

int Renderer::FilledPolygon(std::list<Point2i> polygon, const Color & color)
{
  Sint16 * vx, * vy;
  vx = new Sint16[polygon.size()];
  vy = new Sint16[polygon.size()];
  int i = 0;
  for (std::list<Point2i>::iterator point = polygon.begin(); point != polygon.end(); point++, i++) {
    vx[i] = point->x;
    vy[i] = point->y;
  }
  int result = filledPolygonRGBA(renderer, vx, vy, polygon.size(), color.GetRed(),
                                 color.GetGreen(), color.GetBlue(), color.GetAlpha());
  delete[] vx;
  delete[] vy;
  return result;
}

int Renderer::TexturedPolygon(const Sint16 * vx, const Sint16 * vy, const int n, const Surface *texture, const int texture_dx, const int texture_dy)
{
  return texturedPolygon(renderer, vx, vy, n, texture->surface, texture_dx, texture_dy);
}

int Renderer::TexturedPolygon(std::list<Point2i> polygon, const Surface * texture)
{
  Sint16 * vx, * vy;
  vx = new Sint16[polygon.size()];
  vy = new Sint16[polygon.size()];
  int i = 0;
  for (std::list<Point2i>::iterator point = polygon.begin(); point != polygon.end(); point++, i++) {
    vx[i] = point->x;
    vy[i] = point->y;
  }
  int result = texturedPolygon(renderer, vx, vy, polygon.size(), texture->surface, 0, 0);
  delete[] vx;
  delete[] vy;
  return result;
}
