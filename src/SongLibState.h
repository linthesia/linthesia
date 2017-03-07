// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#ifndef __SONGLIB_STATE_H
#define __SONGLIB_STATE_H

#include "SharedState.h"
#include "GameState.h"
#include "StringTile.h"
#include "CompatibleSystem.h"

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
    int m_page_count;
    int m_current_page;
    int m_tiles_per_page;

    ButtonState m_continue_button;
    ButtonState m_back_button;

    std::vector<StringTile> m_song_tiles;

    SharedState m_state;
};

#endif // __SONGLIB_STATE_H