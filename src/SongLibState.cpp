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

const static string SONG_LIB_DIR_SETTINGS_KEY = "song_lib_last_dir";

void SongLibState::Init() {

    m_back_button = ButtonState(
        Layout::ScreenMarginX,
        Compatible::GetDisplayHeight() - Layout::ScreenMarginY/2 - Layout::ButtonHeight/2,
        Layout::ButtonWidth, Layout::ButtonHeight);

    m_base_path = UserSetting::Get("song_lib_path", MUSICDIR);
    m_curent_path = UserSetting::Get(SONG_LIB_DIR_SETTINGS_KEY, MUSICDIR);
    m_current_page = 0;

    UpdateSongTiles();

    m_next_page_button = ButtonState(
        ContentRight() + ColumnMargin,
        PagesButtonsY,
        Layout::ButtonWidth, Layout::ButtonHeight);

    m_prev_page_button = ButtonState(
        ContentLeft() - ColumnMargin - Layout::ButtonWidth,
        PagesButtonsY,
        Layout::ButtonWidth, Layout::ButtonHeight);

    m_path_up_button = ButtonState(
        Layout::ScreenMarginX,
        (Layout::ScreenMarginY - Layout::ButtonHeight) / 2,
        Layout::ButtonWidth, Layout::ButtonHeight);
}

int SongLibState::ContentLeft() {
    int columns_margins = ColumnMargin * (m_columns - 1);
    int columns_content = SongTileWidth * m_columns;
    
    int content_left_slim  = (GetStateWidth()  - SongTileWidth) / 2;

    int content_left_wide  = (GetStateWidth()  - columns_content - columns_margins) / 2;

    return (m_columns > 1) ? content_left_wide  : content_left_slim;
}

int SongLibState::ContentRight() {
    int columns_margins = ColumnMargin * (m_columns - 1);
    int columns_content = SongTileWidth * m_columns;
    
    int content_left_slim  = (GetStateWidth()  - SongTileWidth) / 2;
    int content_right_slim = content_left_slim + SongTileWidth;

    int content_left_wide  = (GetStateWidth()  - columns_content - columns_margins) / 2;
    int content_right_wide = content_left_wide + columns_content + columns_margins;

    return (m_columns > 1) ? content_right_wide : content_right_slim;
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

string eraseSubstring(string subj, string erase) {
    std::string t = subj;
    std::string s = erase;
    std::string::size_type i = t.find(s);

    if (i != std::string::npos)
        t.erase(i, s.length());
    
    return t;
}

void SongLibState::UpdateSongTiles() {
    
    m_song_tiles.clear();

    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir (m_curent_path.c_str())) != NULL) {

        Tga* song_tile_graphics = GetTexture(SongBox);
        Tga* dir_tile_graphics = GetTexture(DirBox);

        while ((ent = readdir (dir)) != NULL) {
            string f_name = string(ent->d_name);
            if (f_name.compare(".") != 0 && f_name.compare("..") != 0) {

                if (ent->d_type == DT_DIR || isMidiFile(string(ent->d_name))) {
                    Tga * graphics = ent->d_type == DT_DIR ? dir_tile_graphics : song_tile_graphics;
                    string path = m_curent_path + "/" + ent->d_name;
                    string title = string(FileSelector::TrimFilename(ent->d_name));
                    SongTile song_tile = SongTile(0, 0, path, title,
                                        ent->d_type == DT_DIR,
                                        graphics);
                    m_song_tiles.push_back(song_tile);
                }

            }
        }
        closedir (dir);

        //Assign coordinates for the first time
        Resize();
    }
    else {
        throw LinthesiaError("Can't open dir");
    }

}

void SongLibState::Resize() {

    UpdateSongTilesPage();

    m_back_button.SetX(Layout::ScreenMarginX);
    m_back_button.SetY(GetStateHeight() - Layout::ScreenMarginY/2 - Layout::ButtonHeight/2);

    m_prev_page_button.SetX(ContentLeft() - ColumnMargin - Layout::ButtonWidth);
    m_prev_page_button.SetY(PagesButtonsY);

    m_next_page_button.SetX(ContentRight() + ColumnMargin);
    m_next_page_button.SetY(PagesButtonsY);
}

void SongLibState::UpdateSongTilesPage() {
    int initial_y = Layout::ScreenMarginY + SongTileHeight / 2;
    int each_y = SongTileHeight + SongTileHeight / 4;

    // Can't we place songs in two columns?
    int max_columns = 2;
    bool slim = GetStateWidth() < ColumnMargin + SongTileWidth * max_columns;
    m_columns = slim ? 1 : max_columns;

    int rows = (GetStateHeight() - initial_y - Layout::ScreenMarginY) / each_y;
    int tiles_per_page = rows * m_columns;
    
    std::vector<SongTile>::size_type tiles_total = m_song_tiles.size();
    m_page_count = (tiles_per_page == 0 ? 0 : (tiles_total / tiles_per_page)) + 1;

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        m_song_tiles[i].SetVisible(false);
    }

    for(std::vector<SongTile>::size_type i = m_current_page * tiles_per_page; i < m_song_tiles.size() && i < tiles_per_page * (m_current_page + 1) ; i++) {

        int tx = 0;
        int ty = 0;
        if (slim) {
            tx = (GetStateWidth() - SongTileWidth) / 2;
            ty = initial_y + each_y * (i - m_current_page * tiles_per_page);
        }
        else {
            int column = i % m_columns;
            int columns_base_offset = (GetStateWidth() - SongTileWidth * m_columns - ColumnMargin * (m_columns - 1)) / 2;
            tx = columns_base_offset + column * (ColumnMargin + SongTileWidth);
            ty = initial_y + each_y * ((i - m_current_page * tiles_per_page) / m_columns);
        }

        m_song_tiles[i].SetX(tx);
        m_song_tiles[i].SetY(ty);

        m_song_tiles[i].SetVisible(true);
    }
}

void SongLibState::Update() {
    MouseInfo mouse = Mouse();

    if (m_skip_next_mouse_up) {
        mouse.released.left = false;
        m_skip_next_mouse_up = false;
    }

    m_back_button.Update(mouse);

    if (IsKeyPressed(KeyEscape) || m_back_button.hit) {
        if (m_state.midi) {
            ChangeState(new TitleState(m_state));
        }
        else {
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

    if (m_current_page + 1 < m_page_count) {
        m_next_page_button.Update(mouse);

        if (IsKeyPressed(KeyRight) || m_next_page_button.hit) {
            m_current_page++;
            UpdateSongTilesPage();
        }
    }

    if (m_current_page > 0) {
        m_prev_page_button.Update(mouse);

        if (IsKeyPressed(KeyLeft) || m_prev_page_button.hit) {
            m_current_page--;
            UpdateSongTilesPage();
        }
    }

    string path_title = eraseSubstring(m_curent_path, m_base_path);
    if (path_title.length() > 0) {
        m_path_up_button.Update(mouse);

        if (IsKeyPressed(KeyBackward) || m_path_up_button.hit) {
            m_skip_next_mouse_up = true;
            m_current_page = 0;
            GoUpDirectory();
        }
    }

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        MouseInfo tile_mouse(mouse);
        tile_mouse.x -= m_song_tiles[i].GetX();
        tile_mouse.y -= m_song_tiles[i].GetY();
        m_song_tiles[i].Update(tile_mouse);
    }

    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        if(m_song_tiles[i].IsVisible() && m_song_tiles[i].WholeTile().hit) {

            if (m_song_tiles[i].IsDir()) {
                m_skip_next_mouse_up = true;
                m_curent_path = m_song_tiles[i].GetPath();
                UserSetting::Set(SONG_LIB_DIR_SETTINGS_KEY, m_curent_path);
                UpdateSongTiles();
            }
            else {
                OpenTitleState(m_song_tiles[i].GetPath());
            }

            return;
        }
    }
}

void SongLibState::GoUpDirectory() {
    m_curent_path = m_curent_path.substr(0, m_curent_path.find_last_of("\\/"));
    UserSetting::Set(SONG_LIB_DIR_SETTINGS_KEY, m_curent_path);
    UpdateSongTiles();
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

    Layout::DrawButton(renderer, m_back_button, 
        m_state.midi ? GetTexture(ButtonBackToTitle) : GetTexture(ButtonExit));
    
    if(m_current_page > 0) {
        Layout::DrawButton(renderer, m_prev_page_button, GetTexture(ButtonPageBack));
    }
    if(m_current_page + 1 < m_page_count) {
        Layout::DrawButton(renderer, m_next_page_button, GetTexture(ButtonPageNext));
    }

    Layout::DrawHorizontalRule(renderer,
                             GetStateWidth(),
                             GetStateHeight() - Layout::ScreenMarginY);

    Layout::DrawHorizontalRule(renderer,
                             GetStateWidth(),
                             Layout::ScreenMarginY);
    
    for(std::vector<SongTile>::size_type i = 0; i < m_song_tiles.size(); i++) {
        if(m_song_tiles[i].IsVisible()) {
            renderer.ForceTexture(0);
            m_song_tiles[i].Draw(renderer);
        }
    }

    string path_title = eraseSubstring(m_curent_path, m_base_path);
    if (path_title.length() > 0) {
        // Draw mode text
        TextWriter title(Layout::ScreenMarginX + Layout::ButtonWidth + ColumnMargin, Layout::ScreenMarginY / 2 - 6, renderer, false, 14);
        title << path_title.c_str();

        Layout::DrawButton(renderer, m_path_up_button, GetTexture(ButtonDirBack));
    }

    //TextWriter dbg(Layout::ScreenMarginX, GetStateHeight() - Layout::ScreenMarginY * 2, renderer, false, 14);
    //dbg << m_current_page;
}