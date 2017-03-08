// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#ifndef __SONGLIB_STATE_H
#define __SONGLIB_STATE_H

#include "SharedState.h"
#include "GameState.h"
#include "SongTile.h"
#include "CompatibleSystem.h"
#include "FileSelector.h"

#include <vector>
#include <string>

#ifndef SONGLIBDIR
#define SONGLIBDIR "../music"
#endif

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

    int m_page_count;
    int m_current_page;
    int m_tiles_per_page;

    ButtonState m_continue_button;
    ButtonState m_back_button;

    string m_curent_path;

    std::vector<SongTile> m_song_tiles;

    SharedState m_state;
    bool m_skip_next_mouse_up;
};

#endif // __SONGLIB_STATE_H