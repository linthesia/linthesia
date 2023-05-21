// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "TextWriter.h"
#include "Renderer.h"
#include "LinthesiaError.h"
#include "OSGraphics.h"
#include "UserSettings.h"

#include <map>
#include <iostream>

#include <SDL_ttf.h>

using namespace std;


// TODO: This should be deleted at shutdown
static map<std::pair<int, string>, TTF_Font*> font_map;

static TTF_Font* get_font(int size, string fontname) {
  auto key = make_pair(size, fontname);
  TTF_Font* font = font_map[key];

  if (!font) {
    std::string font_path = GRAPHDIR + std::string("/") + fontname;
    font = TTF_OpenFont(font_path.c_str(), size);
    if (!font)
      throw LinthesiaSDLTTFError("unable to load font " + fontname + " from " + font_path);
    
    font_map[key] = font;
  }
  return font;
}

TextWriter::TextWriter(int in_x, int in_y, Renderer &in_renderer,
                       bool in_centered, int in_size, string fontname) :
  x(in_x),
  y(in_y),
  size(in_size),
  original_x(0),
  last_line_height(0),
  centered(in_centered),
  renderer(in_renderer) {

  x += renderer.m_xoffset;
  original_x = x;

  y += renderer.m_yoffset;
  point_size = size;

  font = get_font(in_size, fontname);
}


int TextWriter::get_point_size() {
  return point_size;
}

TextWriter& TextWriter::next_line() {
  y += max(last_line_height, get_point_size());
  x = original_x;

  last_line_height = 0;
  return *this;
}

void Text::DrawText(TextWriter& tw, SDL_Color color, int draw_x, int draw_y) const
{
  SDL_Surface* sFont = TTF_RenderText_Blended(tw.font, m_text.c_str(), color);
  if (sFont == nullptr)
    throw LinthesiaSDLTTFError("error rendering text");

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glPixelStorei(GL_UNPACK_ROW_LENGTH, sFont->pitch / sFont->format->BytesPerPixel);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sFont->w, sFont->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sFont->pixels);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  glPushMatrix();

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0,0); glVertex2f(draw_x, draw_y);
    glTexCoord2f(1,0); glVertex2f(draw_x + sFont->w, draw_y);
    glTexCoord2f(1,1); glVertex2f(draw_x + sFont->w, draw_y + sFont->h);
    glTexCoord2f(0,1); glVertex2f(draw_x, draw_y + sFont->h);
  }
  glEnd();

  glPopMatrix();
  glDeleteTextures(1, &texture);

  SDL_FreeSurface(sFont);
}

TextWriter& Text::operator<<(TextWriter& tw) const {
  int draw_x = 0;
  int draw_y = 0;

  if (m_text.size() == 0)
    return tw;

  calculate_position_and_advance_cursor(tw, &draw_x, &draw_y);

  if (m_attrs.has_shadow)
    DrawText(tw, m_attrs.shadow, draw_x + 2, draw_y + 2);

  DrawText(tw, m_attrs.color, draw_x, draw_y);


  return tw;
}

void Text::calculate_position_and_advance_cursor(TextWriter &tw, int *out_x, int *out_y) const {
  int w, h;
  if(TTF_SizeUTF8(tw.font, m_text.c_str(), &w, &h)) {
    throw LinthesiaSDLTTFError("TTF_SizeUTF8 returned an error");
  }
  if (tw.centered) {
    *out_x = tw.x - w / 2;
  }
  else {
    *out_x = tw.x;
    tw.x += w;
  }
  *out_y = tw.y;
}

TextWriter& operator<<(TextWriter& tw, const Text& t) {
  return t.operator <<(tw);
}

TextWriter& newline(TextWriter& tw) {
  return tw.next_line();
}

TextWriter& operator<<(TextWriter& tw, const string& s)        { return tw << Text(s, White); }
TextWriter& operator<<(TextWriter& tw, const int& i)           { return tw << Text(i, White); }
TextWriter& operator<<(TextWriter& tw, const unsigned int& i)  { return tw << Text(i, White); }
TextWriter& operator<<(TextWriter& tw, const long& l)          { return tw << Text(l, White); }
TextWriter& operator<<(TextWriter& tw, const unsigned long& l) { return tw << Text(l, White); }
