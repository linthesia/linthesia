// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#include "SongTile.h"
#include "TextWriter.h"
#include "Renderer.h"
#include "Tga.h"

SongTile::SongTile(int x, int y, string path, string title, bool dir, Tga *frame_graphics) :
  m_x(x),
  m_y(y),
  m_path(path),
  m_name(title),
  m_dir(dir),
  m_frame_graphics(frame_graphics) {

  // Initialize the size and position of each button
  whole_tile = ButtonState(0, 0, SongTileWidth, SongTileHeight);
}

void SongTile::Update(const MouseInfo &translated_mouse) {
  whole_tile.Update(translated_mouse);
}

void SongTile::Draw(Renderer &renderer) const {

  renderer.SetOffset(m_x, m_y);

  const Color hover = Renderer::ToColor(0xFF,0xFF,0xFF);
  const Color no_hover = Renderer::ToColor(0xE0,0xE0,0xE0);
  renderer.SetColor(whole_tile.hovering ? hover : no_hover);
  renderer.DrawTga(m_frame_graphics, 0, 0);

  // Draw mode text
  TextWriter title(44, 49, renderer, false, 14);
  title << m_name.c_str();

  renderer.ResetOffset();
}
