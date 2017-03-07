// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#include "SongLibState.h"
#include "Textures.h"
#include "UserSettings.h"
#include "MenuLayout.h"

#include <dirent.h>

using namespace std;

void SongLibState::Init() {

    m_back_button = ButtonState(
        Layout::ScreenMarginX,
        Compatible::GetDisplayHeight() - Layout::ScreenMarginY/2 - Layout::ButtonHeight/2,
        Layout::ButtonWidth, Layout::ButtonHeight);

    string default_dir = UserSetting::Get("default_music_directory", SONGLIBDIR);
    //default_dir += string("/Learning");

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (default_dir.c_str())) != NULL) {

        int initial_y = Layout::ScreenMarginY;
        int each_y = StringTileHeight + 80;
        int tile_index = 0;
        
        int width = GetStateWidth() || Compatible::GetDisplayWidth();

        while ((ent = readdir (dir)) != NULL) {
            string f_name = string(ent->d_name);
            if (f_name.compare(".") != 0 && f_name.compare("..") != 0) {
                
                StringTile file_tile = StringTile(
                                    (width - StringTileWidth) / 2,
                                    initial_y + each_y * tile_index++,
                                    GetTexture(SongBox));

                file_tile.SetString(ent->d_name);
                m_song_tiles.push_back(file_tile);
            }
        }

        closedir (dir);
    }
}

void SongLibState::Resize() {

    int initial_y = Layout::ScreenMarginY;
    int each_y = StringTileHeight + 80;

    for(std::vector<StringTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        m_song_tiles[i].SetX((GetStateWidth() - StringTileWidth) / 2);
        m_song_tiles[i].SetY(initial_y + each_y * i);
    }

    m_back_button.SetX(Layout::ScreenMarginX);
    m_back_button.SetY(GetStateHeight() - Layout::ScreenMarginY/2 - Layout::ButtonHeight/2);
}

void SongLibState::Update() {
    MouseInfo mouse = Mouse();
    m_back_button.Update(mouse);

    for(std::vector<StringTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        MouseInfo tile_mouse(mouse);
        tile_mouse.x -= m_song_tiles[i].GetX();
        tile_mouse.y -= m_song_tiles[i].GetY();
        m_song_tiles[i].Update(tile_mouse);
    }

    if (IsKeyPressed(KeyEscape) || m_back_button.hit) {
        delete m_state.midi_out;
        m_state.midi_out = 0;

        delete m_state.midi_in;
        m_state.midi_in = 0;

        delete m_state.midi;
        m_state.midi = 0;

        Compatible::GracefulShutdown();
        return;
   }
}

void SongLibState::Draw(Renderer &renderer) const {

    Layout::DrawButton(renderer, m_back_button, GetTexture(ButtonExit));

    for(std::vector<StringTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        m_song_tiles[i].Draw(renderer);
    }
}