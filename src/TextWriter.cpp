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
#include <fontconfig/fontconfig.h>

using namespace std;

// TODO: This should be deleted at shutdown
static map<int, int> font_size_lookup;

// TODO: This should be deleted at shutdown
static map<int, Pango::FontDescription*> font_lookup;

// Returns the most suitable font available on the platform
// or an empty string if no font is available;
static const std::string get_default_font();

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

  if (font_size_lookup[size] == 0) {
    GLuint list_start = glGenLists(128);
    Pango::FontDescription *font_desc = NULL;
    Glib::RefPtr<Pango::Font> ret;

    // Try to get the requested name first
    font_desc = new Pango::FontDescription(STRING(fontname << " " << in_size));
    ret = Gdk::GL::Font::use_pango_font(*font_desc, 0, 128, list_start);

    if (!ret) {
      delete font_desc;
      font_desc = NULL;
      // Get font from user settings
      const std::string userfontname = UserSetting::Get("font_desc", "");
      if (!userfontname.empty()) {
        font_desc = new Pango::FontDescription(STRING(userfontname << " " << in_size));
        ret = Gdk::GL::Font::use_pango_font(*font_desc, 0, 128, list_start);
      }
    }

    if (!ret) {
      delete font_desc;
      font_desc = NULL;
      // Get font from system settings
      const std::string sysfontname = get_default_font();
      if (!sysfontname.empty()) {
        font_desc = new Pango::FontDescription(STRING(sysfontname << " " << in_size));
        ret = Gdk::GL::Font::use_pango_font(*font_desc, 0, 128, list_start);
      } 
    }

    if (!ret) {
  	FcConfig *config = FcInitLoadConfigAndFonts();
	FcPattern* pat = FcPatternCreate();
	FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
	FcFontSet* fs = FcFontList(config, pat, os);

	printf("Warning : cannot load fonts so far : trying EVERY font you have: %d\n", fs->nfont);
	for (int i=0; fs && i < fs->nfont; ++i) {
	   FcPattern* font = fs->fonts[i];
	   FcChar8 *file, *style, *family;
	   if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
	       FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
	       FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch) {
		      //printf("Filename: %s (family %s, style %s)\n", file, family, style);
		      font_desc = new Pango::FontDescription(STRING((char *)family << " " << style << " 14" ));
		      ret = Gdk::GL::Font::use_pango_font(*font_desc, 0, 128, list_start);
		      //printf ("DEBUG : family = %d    -    ret=%d \n\n", family, ret);
		      if (ret) {
			  printf ("DEBUG : FOUND !!!!\n");
                          font_size_lookup[size] = list_start;
                          font_lookup[size] = font_desc;
			  break;
                      }

            }
      }
    }
    if (!ret) {
       fprintf(stderr, "FATAL WARNING: An error ocurred while trying to use (any) pango font. \n"); // FIXME ?
       // Trying to go without a working pango font.... 
	    font_size_lookup[size] = list_start;
	    font_lookup[size] = font_desc;
      //     delete font_desc;
      //     glDeleteLists(list_start, 128);
      //     throw LinthesiaError("An error ocurred while trying to use pango font");
     } else {
	    font_size_lookup[size] = list_start;
	    font_lookup[size] = font_desc;
    }
  }
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

TextWriter& Text::operator<<(TextWriter& tw) const {
  int draw_x = 0;
  int draw_y = 0;
  calculate_position_and_advance_cursor(tw, &draw_x, &draw_y);

  string narrow(m_text.begin(), m_text.end());

  glBindTexture(GL_TEXTURE_2D, 0);

  glPushMatrix();
  tw.renderer.SetColor(m_color);
  glListBase(font_size_lookup[tw.size]);
  glRasterPos2i(draw_x, draw_y + tw.size);
  glCallLists(static_cast<int>(narrow.length()), GL_UNSIGNED_BYTE, narrow.c_str());
  glPopMatrix();

  // TODO: Should probably delete these on shutdown.
  //glDeleteLists(1000, 128);

  return tw;
}

void Text::calculate_position_and_advance_cursor(TextWriter &tw, int *out_x, int *out_y) const  {
  Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(tw.renderer.m_pangocontext);
  layout->set_text(m_text);
  layout->set_font_description(*(font_lookup[tw.size]));

  Pango::Rectangle drawing_rect = layout->get_pixel_logical_extents();
  tw.last_line_height = drawing_rect.get_height();

  if (tw.centered)
    *out_x = tw.x - drawing_rect.get_width() / 2;

  else {
    *out_x = tw.x;
    tw.x += drawing_rect.get_width();
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

static
const std::string get_default_font()
{
  std::string returnedFont;

  FcResult fcres;
  FcConfig *config = FcInitLoadConfigAndFonts();

  FcPattern *pattern = FcPatternCreate();
  //FcPattern *pattern = FcNameParse((const FcChar8*)"serif");
  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);
  FcPattern *match = FcFontMatch(config, pattern, &fcres);

  FcChar8 *family = NULL;
  if (fcres == FcResultMatch) {
    fcres = FcPatternGetString(match, FC_FAMILY, 0, &family);
    returnedFont = (char *)family;
  }
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);

  return returnedFont;
}
