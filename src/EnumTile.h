// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __ENUM_TILE_H
#define __ENUM_TILE_H

#include "GameState.h"
#include "MenuLayout.h"
#include "TrackTile.h"
#include <vector>

#include "libmidi/Midi.h"

const int EnumTileWidth = 510;
const int EnumTileHeight = 80;

enum TrackTileGraphic;

class EnumValue
{
public:
  virtual void next() = 0;
  virtual void previous() = 0;

  virtual std::string AsText() const = 0;
};


class EnumTile 
{
public:

  EnumTile(int x, int y, EnumValue& value, std::string tile_title, Tga* button_graphics, Tga *frame_graphics);

  void Update(const MouseInfo &translated_mouse);
  void Draw(Renderer &renderer) const;

  int GetX() const {
    return m_x;
  }

  int GetY() const {
    return m_y;
  }

  void SetX(const int x) {
    m_x = x;
  }

  void SetY(const int y) {
    m_y = y;
  }



  const ButtonState WholeTile() const {
    return whole_tile;
  }

  const ButtonState ButtonLeft() const {
    return button_mode_left;
  }

  const ButtonState ButtonRight() const {
    return button_mode_right;
  }

private:
  EnumTile operator=(const EnumTile &);

  int m_x;
  int m_y;

  Tga *m_button_graphics;
  Tga *m_frame_graphics;

  EnumValue& m_value;


  ButtonState whole_tile;
  ButtonState button_mode_left;
  ButtonState button_mode_right;

  std::string m_tile_title;

  int LookupGraphic(TrackTileGraphic graphic, bool button_hovering) const;
};

#endif // __ENUM_TILE_H
