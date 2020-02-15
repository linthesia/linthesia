// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#ifndef __SONGLIB_STATE_H
#define __SONGLIB_STATE_H

#include "SharedState.h"
#include "GameState.h"
#include "SongTile.h"
#include "CompatibleSystem.h"
#include "FileSelector.h"
#include "MenuLayout.h"
#include <sqlite3.h>

#include <vector>
#include <string>

#ifndef MUSICDIR
#define MUSICDIR "../music/"
#endif

const int ColumnMargin = Layout::ScreenMarginX * 2;
const int PagesButtonsY = 
    Layout::ScreenMarginY + Layout::ScreenMarginY / 2 + 
    SongTileHeight / 2 - Layout::ButtonHeight/2;

class SongLibState : public GameState {

public:
    SongLibState(const SharedState &state): 
    m_state(state){
    }

protected:
    virtual void Init();
    virtual void Update();
    virtual void Resize();
    virtual void Draw(Renderer &renderer) const;

private:

    void UpdateSongTiles();
    void OpenTitleState(string path);
    void UpdateSongTilesPage();

    void GoUpDirectory();

    int ContentLeft();
    int ContentRight();

    int m_page_count;
    int m_current_page;
    int m_columns;
    int m_tiles_per_page;

    ButtonState m_back_button;
    ButtonState m_path_up_button;
    ButtonState m_next_page_button;
    ButtonState m_prev_page_button;

    string m_current_path;
    string m_base_path;

    std::vector<SongTile> m_song_tiles;

    SharedState m_state;
    bool m_skip_next_mouse_up;
};

#endif // __SONGLIB_STATE_H
