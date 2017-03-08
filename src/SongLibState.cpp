// -*- mode: c++; coding: utf-8 -*-

// Linthesia

#include "SongLibState.h"
#include "Textures.h"
#include "UserSettings.h"
#include "MenuLayout.h"
#include "TitleState.h"
#include "LinthesiaError.h"

#include "libmidi/Midi.h"

#include <dirent.h>

using namespace std;

void SongLibState::Init() {

    m_back_button = ButtonState(
        Layout::ScreenMarginX,
        Compatible::GetDisplayHeight() - Layout::ScreenMarginY/2 - Layout::ButtonHeight/2,
        Layout::ButtonWidth, Layout::ButtonHeight);

    m_curent_path = UserSetting::Get("default_music_directory", SONGLIBDIR);

    UpdateSongTiles();
}

bool hasEnding (string fullString, string ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool isMidiFile(string f) {
    return hasEnding(f, string(".midi")) || hasEnding(f, string(".mid"));
}

void SongLibState::UpdateSongTiles() {
    
    m_song_tiles.clear();

    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir (m_curent_path.c_str())) != NULL) {

        int initial_y = Layout::ScreenMarginY;
        int each_y = SongTileHeight + SongTileHeight / 5;
        int tile_index = 0;
        
        Tga* song_tile_graphics = GetTexture(SongBox);

        while ((ent = readdir (dir)) != NULL) {
            string f_name = string(ent->d_name);
            if (f_name.compare(".") != 0 && f_name.compare("..") != 0) {

                if (ent->d_type == DT_DIR || isMidiFile(string(ent->d_name))) {
                    SongTile song_tile = SongTile(
                                        (GetStateWidth() - SongTileWidth) / 2,
                                        initial_y + each_y * tile_index++,
                                        m_curent_path + "/" + ent->d_name,
                                        string(ent->d_name),
                                        ent->d_type == DT_DIR,
                                        song_tile_graphics);
                    m_song_tiles.push_back(song_tile);
                }

            }
        }

        closedir (dir);
    }
    else {
        throw LinthesiaError("Can't open dir");
    }

}

void SongLibState::Resize() {

    int initial_y = Layout::ScreenMarginY;
    int each_y = SongTileHeight + 80;

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        m_song_tiles[i].SetX((GetStateWidth() - SongTileWidth) / 2);
        m_song_tiles[i].SetY(initial_y + each_y * i);
    }

    m_back_button.SetX(Layout::ScreenMarginX);
    m_back_button.SetY(GetStateHeight() - Layout::ScreenMarginY/2 - Layout::ButtonHeight/2);
}

void SongLibState::Update() {
    MouseInfo mouse = Mouse();

    if (m_skip_next_mouse_up) {
        mouse.released.left = false;
        m_skip_next_mouse_up = false;
    }
    
    m_back_button.Update(mouse);

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

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        MouseInfo tile_mouse(mouse);
        tile_mouse.x -= m_song_tiles[i].GetX();
        tile_mouse.y -= m_song_tiles[i].GetY();
        m_song_tiles[i].Update(tile_mouse);
    }

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        if(m_song_tiles[i].WholeTile().hit) {

            if (m_song_tiles[i].IsDir()) {
                m_skip_next_mouse_up = true;
                m_curent_path = m_song_tiles[i].GetPath();
                UpdateSongTiles();
            }
            else {
                OpenTitleState(m_song_tiles[i].GetPath());
            }

            return;
        }
    }
}

void SongLibState::OpenTitleState(string path) {
    Midi *midi = 0;

    try {
        midi = new Midi(Midi::ReadFromFile(path.c_str()));
    }

    catch (const MidiError &e) {
        string wrapped_description = STRING("Problem while loading file: " <<
                                            path.c_str() <<
                                            "\n") + e.GetErrorDescription();

        Compatible::ShowError(wrapped_description);

        midi = 0;
    }

    if (midi) {
        m_state.midi = midi;
        m_state.song_title = FileSelector::TrimFilename(path.c_str());
        ChangeState(new TitleState(m_state));
    }
}

void SongLibState::Draw(Renderer &renderer) const {

    Layout::DrawButton(renderer, m_back_button, GetTexture(ButtonExit));

    Layout::DrawHorizontalRule(renderer,
                             GetStateWidth(),
                             GetStateHeight() - Layout::ScreenMarginY);

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        renderer.ForceTexture(0);
        m_song_tiles[i].Draw(renderer);
    }
}