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

  bool IsVisible() const {
     return m_visible;
  }

  void SetX(const int x) {
    m_x = x;
  }

  void SetY(const int y) {
    m_y = y;
  }

  void SetVisible(const bool visible) {
    m_visible = visible;
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

  string GetSortPath() const {
    // to sort strings, we need to have string to sort
    // directories "first", right ?
    if (m_dir)
      return ("first" + m_path); 
    else
      return ("second" + m_path);
  }

private:

  int m_x;
  int m_y;

  bool m_dir;
  bool m_visible;

  string m_path;
  string m_name;
  string m_current_path;

  Tga *m_frame_graphics;

  ButtonState whole_tile;

};

#endif // __SONG_TILE_H
