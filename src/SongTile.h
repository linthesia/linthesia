// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#ifndef __SONG_TILE_H
#define __SONG_TILE_H

#include "GameState.h"
#include "MenuLayout.h"
#include <string>

const int SongTileWidth = 510;
const int SongTileHeight = 80;

class SongTile {
public:

  SongTile(int x, int y, string path, string title, bool dir, Tga *frame_graphics);

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

  bool IsDir() const {
      return m_dir;
  }

  string GetPath() const {
      return m_path;
  }

  const ButtonState WholeTile() const {
    return whole_tile;
  }

private:

  int m_x;
  int m_y;

  bool m_dir;

  string m_path;
  string m_name;
  string m_curent_dir;

  Tga *m_frame_graphics;

  ButtonState whole_tile;

};

#endif // __SONG_TILE_H
