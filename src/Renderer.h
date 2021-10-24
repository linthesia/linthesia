// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __RENDERER_H
#define __RENDERER_H

#include "OSGraphics.h"
#include "Tga.h"
#include "SDL2/SDL.h"

static bool m_vsync_initialized;

class Renderer {
public:

  Renderer(SDL_Window* sdl_window);

  static SDL_Color ToColor(int r, int g, int b, int a = 0xFF);

  void SwapBuffers();

  // 0 will disable vsync, 1 will enable.
  void SetVSyncInterval(int interval = 1);

  void SetOffset(int x, int y) {
    m_xoffset = x; m_yoffset = y;
  }

  void ResetOffset() {
    SetOffset(0,0);
  }

  void ForceTexture(unsigned int texture_id);

  void SetColor(SDL_Color c);
  void SetColor(int r, int g, int b, int a = 0xFF);
  void DrawQuad(int x, int y, int w, int h);

  void DrawTga(const Tga *tga, int x, int y) const;
  void DrawTga(const Tga *tga, int x, int y, int width, int height,
               int src_x, int src_y) const;

  void DrawStretchedTga(const Tga *tga, int x, int y, int w, int h) const;
  void DrawStretchedTga(const Tga *tga, int x, int y, int w, int h,
                        int src_x, int src_y, int src_w, int src_h) const;

private:

  // NOTE: These are used externally by the friend
  // class TextWriter (along with the context)
  int m_xoffset;
  int m_yoffset;

  SDL_Window* m_sdl_window;

  friend class Text;
  friend class TextWriter;
};

#endif // __RENDERER_H
