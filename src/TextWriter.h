// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __TEXTWRITER_H
#define __TEXTWRITER_H

#ifndef __cdecl
#define __cdecl
#endif

#include <vector>
#include <string>
#include <iomanip>

#include "StringUtil.h"
#include "TrackProperties.h"

#include <SDL2/SDL_ttf.h>

// A nice ostream-like class for drawing OS-specific (or OpenGL) text to the
// screen in varying colors, fonts, and sizes.
class TextWriter {
public:

  // Centering only works for single-write lines... in other words, centered
  // lines can only be 1 color.
  TextWriter(int in_x, int in_y,
	     Renderer &in_renderer, bool in_centered = false,
	     int in_size = 12, std::string fontname = "FreeSans.ttf");

  // Skips at least 1 line, or the height of the last write... whichever
  // is greater (so that you can skip down past a multiline write)
  TextWriter& next_line();

  // Allow manipulators
  TextWriter& operator<<(TextWriter& (__cdecl *_Pfn)(TextWriter&)) {
    (*_Pfn)(*(TextWriter *)this);
    return (*this);
  }

private:
  TextWriter operator=(const TextWriter&);
  TextWriter(const TextWriter&);

  int get_point_size();

  int point_size;
  int x, y, size, original_x;
  int last_line_height;
  bool centered;
  Renderer renderer;
  TTF_Font* font;

  friend class Text;
};

// Some colors to choose from, for convenience
const static SDL_Color Black       = { 0x00,0x00,0x00, 0xFF };
const static SDL_Color Dk_Blue     = { 0xC4,0x00,0x00, 0xFF };
const static SDL_Color Dk_Green    = { 0x00,0xC4,0x00, 0xFF };
const static SDL_Color Dk_Cyan     = { 0xFF,0x80,0x00, 0xFF };
const static SDL_Color Dk_Red      = { 0x00,0x00,0xC4, 0xFF };
const static SDL_Color Dk_Purple   = { 0x80,0x00,0x80, 0xFF };
const static SDL_Color Brown       = { 0x00,0x40,0x80, 0xFF };
const static SDL_Color Gray        = { 0xBB,0xBB,0xBB, 0xFF };
const static SDL_Color Dk_Gray     = { 0x55,0x55,0x55, 0xFF };
const static SDL_Color Blue        = { 0xFF,0x00,0x00, 0xFF };
const static SDL_Color Green       = { 0x00,0xFF,0x00, 0xFF };
const static SDL_Color Cyan        = { 0xFF,0xFF,0x00, 0xFF };
const static SDL_Color Red         = { 0x00,0x00,0xFF, 0xFF };
const static SDL_Color Magenta     = { 0xFF,0x00,0xFF, 0xFF };
const static SDL_Color Yellow      = { 0x00,0xFF,0xFF, 0xFF };
const static SDL_Color White       = { 0xFF,0xFF,0xFF, 0xFF };
const static SDL_Color Orange      = { 0x20,0x80,0xFF, 0xFF };
const static SDL_Color Pink        = { 0xA0,0x80,0xFF, 0xFF };
const static SDL_Color CheatYellow = { 0x00,0xCC,0xFF, 0xFF };


struct TextAttributes
{
  TextAttributes(SDL_Color color): color(color){};
  TextAttributes(SDL_Color color, SDL_Color shadow): 
    color(color), 
    shadow(shadow),
    has_shadow(true)
  {};

  SDL_Color color;
  SDL_Color shadow;
  bool has_shadow = false;
};

// A class to use TextWriter, and write to the screen
class Text {
public:

  Text(std::string t, TextAttributes attrs) :
    m_attrs(attrs),
    m_text(t)  {
  }

  Text(int i, TextAttributes attrs) :
    m_attrs(attrs),
    m_text(STRING(i)) {
  }

  Text(double d, int prec, TextAttributes attrs) :
    m_attrs(attrs),
    m_text(STRING(std::setprecision(prec) << d)) {
  }

  TextWriter& operator<<(TextWriter& tw) const;

private:

  // This will return where the text should be drawn on
  // the screen (determined in an OS dependent way) and
  // advance the TextWriter's position by the width and/or
  // height of the text.
  void calculate_position_and_advance_cursor(TextWriter &tw, int *out_x,
                                             int *out_y) const;

  void DrawText(TextWriter& tw, SDL_Color color, int draw_x, int draw_y) const;

  TextAttributes m_attrs;
  std::string m_text;
};

// newline manipulator
TextWriter& newline(TextWriter& tw);

TextWriter& operator<<(TextWriter& tw, const Text& t);
TextWriter& operator<<(TextWriter& tw, const std::string& s);
TextWriter& operator<<(TextWriter& tw, const int& i);
TextWriter& operator<<(TextWriter& tw, const unsigned int& i);
TextWriter& operator<<(TextWriter& tw, const long& l);
TextWriter& operator<<(TextWriter& tw, const unsigned long& l);

#endif // __TEXTWRITER_H
