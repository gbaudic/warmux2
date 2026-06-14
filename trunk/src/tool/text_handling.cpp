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
 * Text typing, deleting, editing, ...
 *****************************************************************************/

#include "tool/text_handling.h"
#include <SDL.h>
#include "tool/copynpaste.h"

static bool MoveCursorLeft(const std::string& text, std::string::size_type& pos)
{
  if (pos != 0)
    {
      while ((text[--pos] & 0xc0) == 0x80) {} ;

      return true;
    }
  return false;
}

static bool MoveCursorRight(const std::string& text, std::string::size_type& pos)
{
  if (pos < text.size())
    {
      while ((text[++pos] & 0xc0) == 0x80) {} ;

      return true;
    }
  return false;
}

static bool RemoveUTF8CharBefore(std::string& text, std::string::size_type& pos)
{
  if (pos != 0) {
    while ((text[--pos] & 0xc0) == 0x80)
      {
        text.erase(pos, 1);
      }
    text.erase(pos, 1);
    return true;
  }
  return false;
}

static bool RemoveUTF8CharAfter(std::string& text, std::string::size_type& pos)
{
  if (pos < text.size()) {
    MoveCursorRight(text, pos);
    return RemoveUTF8CharBefore(text, pos);
  }

  return false;
}


static bool processModifier(std::string& text, std::string::size_type& pos, const SDL_Keysym& key)
{
  switch (key.sym)
    {
    case SDLK_v:
    case SDLK_y:
      // copy/paste
      return RetrieveBuffer(text, pos);
    default:
      return false;
    }
}

bool TextHandle(std::string& text, std::string::size_type& pos, const char* keys)
{
  // check cursor position
  if (pos > text.size()) {
    pos = text.size();
  }

  int i = 0;
  while (i < SDL_TEXTINPUTEVENT_TEXT_SIZE && keys[i] != '\0')
  {
    text.insert(pos++, 1, keys[i]);
    i++;
  }

  return i > 0;
}

bool TextHandle(std::string& text, std::string::size_type& pos, const SDL_Keysym& key)
{
  bool r = true;

  switch (key.sym) {

  case SDLK_RETURN:
  case SDLK_KP_ENTER:
  case SDLK_TAB:
  case SDLK_CLEAR:
  case SDLK_ESCAPE:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_INSERT:
  case SDLK_PAGEUP:
  case SDLK_PAGEDOWN:
    // all those keys are forbidden!
    r = false;
    break;

    // we return true for all the following cases because even if action has failed,
    // the action has not been handled by something else
  case SDLK_HOME:
    pos = 0;
    break;
  case SDLK_END:
    pos = text.size();
    break;
  case SDLK_LEFT:
    MoveCursorLeft(text, pos);
    break;
  case SDLK_RIGHT:
    MoveCursorRight(text, pos);
    break;
  case SDLK_BACKSPACE:
    RemoveUTF8CharBefore(text, pos);
    break;
  case SDLK_DELETE:
    RemoveUTF8CharAfter(text, pos);
    break;

  default:
    if (SDL_GetModState()&(KMOD_CTRL|KMOD_GUI))
      r = processModifier(text, pos, key);
    // Actual text input is no longer handled here
    break;
  }

  return r;
}
