// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __MENU_LAYOUT_H
#define __MENU_LAYOUT_H

#include "GameState.h"

#include <iostream>
using namespace std;

struct ButtonState {

  ButtonState() :
    hovering(false),
    depressed(false),
    x(0), y(0), w(0), h(0) {
  }

  ButtonState(int x, int y, int w, int h) :
    hovering(false),
    depressed(false),
    x(x), y(y), w(w), h(h) {
  }

  void Update(const MouseInfo &mouse) {
    hovering = mouse.x > x && mouse.x < x+w && mouse.y > y && mouse.y < y+h;
    depressed = hovering && mouse.held.left;
    hit = hovering && mouse.released.left;
  }

  void SetX(int _x) {
    x = _x;
  }

  void SetY(int _y) {
    y = _y;
  }

  // Simple mouse over
  bool hovering;

  // Mouse over while (left) button is held down
  bool depressed;

  // Mouse over just as the (left) button is released
  bool hit;

  int x, y;
  int w, h;
};

// Macro to turn replace Renderer::DrawTga()'s 4 parameters with one
#define BUTTON_RECT(button) ((button).x), ((button).y), ((button).w), ((button).h)

namespace Layout {

  void DrawTitle(Renderer &renderer, const std::string &title);
  void DrawSubTitle(Renderer &renderer, const std::string &title);

  void DrawHorizontalRule(Renderer &renderer, int state_width, int y);
  void DrawButton(Renderer &renderer,
                  const ButtonState &button,
                  const Tga *tga);

  // Pixel margin forced at edges of screen
  const static int ScreenMarginX = 16;
  const static int ScreenMarginY = 86;

  const static int TitleFontSize = 20;
  const static int ScoreFontSize = 30;
  const static int ButtonFontSize = 16;
  const static int SmallFontSize = 12;
  const static int TimeFontSize = 13;

  const static int ButtonWidth = 176;
  const static int ButtonHeight = 46;

  const static int NoteNameSize = 12;
  const static int BarFontSize = 12;

};

#endif // __MENU_LAYOUT_H
