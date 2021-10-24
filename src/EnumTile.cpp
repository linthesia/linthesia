// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "EnumTile.h"
#include "TextWriter.h"
#include "Renderer.h"
#include "Tga.h"

const static int GraphicWidth = 36;
const static int GraphicHeight = 36;

EnumTile::EnumTile(int x, int y, EnumValue &value,
                   std::string tile_title,
                   Tga *button_graphics, Tga *frame_graphics) : m_x(x),
                                                                m_y(y),
                                                                m_value(value),
                                                                m_tile_title(tile_title),
                                                                m_button_graphics(button_graphics),
                                                                m_frame_graphics(frame_graphics)
{

  // Initialize the size and position of each button
  whole_tile = ButtonState(0, 0, EnumTileWidth, EnumTileHeight);
  button_mode_left = ButtonState(6, 38, GraphicWidth, GraphicHeight);
  button_mode_right = ButtonState(428, 38, GraphicWidth, GraphicHeight);
}

void EnumTile::Update(const MouseInfo &translated_mouse)
{

  // Update the mouse state of each button
  whole_tile.Update(translated_mouse);
  button_mode_left.Update(translated_mouse);
  button_mode_right.Update(translated_mouse);

  if (button_mode_left.hit)
  {
    m_value.previous();
  }
  if (button_mode_right.hit)
  {
    m_value.next();
  }
}

int EnumTile::LookupGraphic(TrackTileGraphic graphic, bool button_hovering) const
{

  // There are three sets of graphics
  // set 0: window lit, hovering
  // set 1: window lit, not-hovering
  // set 2: window unlit, (implied not-hovering)
  int graphic_set = 2;
  if (whole_tile.hovering)
    graphic_set--;

  if (button_hovering)
    graphic_set--;

  const int set_offset = GraphicWidth * Graphic_COUNT;
  const int graphic_offset = GraphicWidth * graphic;

  return (set_offset * graphic_set) + graphic_offset;
}

void EnumTile::Draw(Renderer &renderer) const
{
  renderer.SetOffset(m_x, m_y);

  const SDL_Color hover = Renderer::ToColor(0xFF, 0xFF, 0xFF);
  const SDL_Color no_hover = Renderer::ToColor(0xE0, 0xE0, 0xE0);
  renderer.SetColor(whole_tile.hovering ? hover : no_hover);
  renderer.DrawTga(m_frame_graphics, 0, 0);

  // Choose the last (gray) color in the TrackTile bitmap
  int color_offset = GraphicHeight * Track::UserSelectableColorCount;

  renderer.DrawTga(m_button_graphics, BUTTON_RECT(button_mode_left),
                   LookupGraphic(GraphicLeftArrow, button_mode_left.hovering),
                   color_offset);

  renderer.DrawTga(m_button_graphics, BUTTON_RECT(button_mode_right),
                   LookupGraphic(GraphicRightArrow, button_mode_right.hovering),
                   color_offset);

  // Draw mode text
  TextWriter mode(44, 49, renderer, false, 14);
  mode << m_value.AsText();

  // Draw title text
  TextWriter title(17, 15, renderer, false, 22, "FreeSerif.ttf");
  title << Text(m_tile_title, TextAttributes(Renderer::ToColor(0xD6, 0xD6, 0xD6), 
                                             Renderer::ToColor(0x32, 0x32, 0x32)));

  renderer.ResetOffset();
}
