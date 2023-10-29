// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "GameState.h"
#include "UserSettings.h"
#include "Renderer.h"
#include "Textures.h"
#include "CompatibleSystem.h"
#include "Tga.h"
#include "OSGraphics.h"

// For FPS display
#include "TextWriter.h"
#include <iomanip>

using namespace std;

// only used on here
const static char* TextureResourceNames[_TextureEnumCount] = {

  "title_Logo",
  "InterfaceButtons",
  "title_GameMusicThemes",

  "score_RetrySong",
  "title_ChooseTracks",
  "title_Exit",
  "tracks_BackToTitle",
  "tracks_PlaySong",

  "all_ButtonNotPlayed",
  "all_ButtonPlayedAuto",
  "all_ButtonYouPlay",
  "all_ButtonYouPlaySilent",
  "all_ButtonLearning",
  "all_ButtonLearningSilent",
  "all_ButtonPlayedHidden",

  "title_InputBox",
  "title_OutputBox",
  "title_EmptyBox",
  "title_SongBox",
  "title_DirBox",
  "title_BackBox",
  "title_ButtonPageBack",
  "title_ButtonPageNext",

  "trackbox", // TrackPanel

  "stats_text",

  "play_Status",
  "play_Status2",
  "play_Keys",

  "play_NotesBlackColor",
  "play_NotesBlackShadow",
  "play_NotesWhiteColor",
  "play_NotesWhiteShadow",

  "play_KeyRail",
  "play_KeyShadow",
  "play_KeysBlack"
};

Tga *GameState::GetTexture(Texture tex_name, bool smooth) const {

  if (!m_manager)
    throw GameStateError("Cannot retrieve texture if manager not set!");

  return m_manager->GetTexture(tex_name, smooth);
}

void GameState::ChangeState(GameState *new_state) {

  if (!m_manager)
    throw GameStateError("Cannot change state if manager not set!");

  m_manager->ChangeState(new_state);

  string color = UserSetting::Get(BG_COLOR, "");
  float R,G,B;

  unsigned int hexcolor;   
  std::stringstream ss;
  ss << std::hex << color;
  ss >> hexcolor;

  R = ((hexcolor >> 16) & 0xff)/255.0; 
  G = ((hexcolor >>  8) & 0xff)/255.0; 
  B = ((hexcolor >>  0) & 0xff)/255.0; 

  glClearColor(R, G, B, 1.0);
}

int GameState::GetStateWidth() const {

  if (!m_manager)
    throw GameStateError("Cannot retrieve state width if manager not set!");

  return m_manager->GetStateWidth();
}

int GameState::GetStateHeight() const {

  if (!m_manager)
    throw GameStateError("Cannot retrieve state height if manager not set!");

  return m_manager->GetStateHeight();
}

bool GameState::IsKeyPressed(GameKey key) const {

  if (!m_manager)
    throw GameStateError("Cannot determine key presses if manager not set!");

  return m_manager->IsKeyPressed(key);
}

const MouseInfo &GameState::Mouse() const {

  if (!m_manager)
    throw GameStateError("Cannot determine mouse input if manager not set!");

  return m_manager->Mouse();
}

void GameState::SetManager(GameStateManager *manager) {

  if (m_manager)
    throw GameStateError("State already has a manager!");

  m_manager = manager;
  Init();
}


GameStateManager::~GameStateManager() {

  for (map<Texture, Tga*>::iterator i = m_textures.begin();
       i != m_textures.end(); ++i) {

    if (i->second) Tga::Release(i->second);
    i->second = 0;
  }
}

Tga *GameStateManager::GetTexture(Texture tex_name, bool smooth) const {

  if (!m_textures[tex_name])
    m_textures[tex_name] = Tga::Load(TextureResourceNames[tex_name]);

  m_textures[tex_name]->SetSmooth(smooth);
  return m_textures[tex_name];
}

void GameStateManager::SetStateDimensions(int w, int h) {
  bool dirty = (m_screen_x != w || m_screen_y != h);
  
  m_screen_x = w;
  m_screen_y = h;
  
  if (dirty && m_current_state) {
    m_current_state->Resize();
  }
}

void GameStateManager::KeyPress(GameKey key) {
  m_key_presses |= static_cast<unsigned long>(key);
}

bool GameStateManager::IsKeyPressed(GameKey key) const {

   return ( (m_key_presses & static_cast<unsigned long>(key)) != 0);
}

bool GameStateManager::IsKeyReleased(GameKey key) const {
  return (!IsKeyPressed(key) &&
          ((m_last_key_presses & static_cast<unsigned long>(key)) != 0));
}

void GameStateManager::MousePress(MouseButton button) {

  switch (button) {
  case MouseLeft:
    m_mouse.held.left = true;
    m_mouse.released.left = false;
    m_mouse.newPress.left = true;
    break;

  case MouseRight:
    m_mouse.held.right = true;
    m_mouse.released.right = false;
    m_mouse.newPress.right = true;
    break;
  }
}

void GameStateManager::MouseRelease(MouseButton button) {

  switch (button) {
  case MouseLeft:
    m_mouse.held.left = false;
    m_mouse.released.left = true;
    m_mouse.newPress.left = false;
    break;

  case MouseRight:
    m_mouse.held.right = false;
    m_mouse.released.right = true;
    m_mouse.newPress.right = false;
    break;
  }
}

void GameStateManager::MouseMove(int x, int y) {

  m_mouse.x = x;
  m_mouse.y = y;
}

void GameStateManager::SetInitialState(GameState *first_state) {

  if (m_current_state)
    throw GameStateError("Cannot set an initial state because GameStateManager"
                         " already has a state!");

  first_state->SetManager(this);
  m_current_state = first_state;
}

void GameStateManager::ChangeState(GameState *new_state) {

  if (!m_current_state)
    throw GameStateError("Cannot change state without a state!  "
                         "Use SetInitialState()!");

  if (!new_state)
    throw GameStateError("Cannot change to a null state!");

  if (!m_inside_update)
    throw GameStateError("ChangeState must be called from inside another "
                         "state's Update() function!  (This is so we can "
                         "guarantee the ordering of the draw/update calls.)");

  m_next_state = new_state;
}

void GameStateManager::Update(bool skip_this_update) {

  // Manager's timer grows constantly
  const unsigned long now = Compatible::GetMicroseconds();
  const unsigned long delta = now - m_last_microseconds;
  m_last_microseconds = now;
  // Now that we've updated the time, we can return if
  // we've been told to skip this one.
  if (skip_this_update)
    return;

  m_fps.Frame(delta);
  if (IsKeyReleased(KeyF6))
    m_show_fps = !m_show_fps;

  if (m_next_state && m_current_state) {

    m_current_state->Finish();
    delete m_current_state;
    m_current_state = 0;

    // We return here to insert a blank frame (that may or may
    // not last a long time) while the next state's Init()
    // and first Update() are being called.
    return;
  }

  if (m_next_state) {

    m_current_state = m_next_state;
    m_next_state = 0;

    m_current_state->SetManager(this);
  }

  if (!m_current_state)
    return;

  m_inside_update = true;

  m_fps.Frame(delta);
  m_frameavg.Frame(delta);
  unsigned long game_delta = m_frameavg.GetAverage();

  m_current_state->m_last_delta_microseconds = game_delta;
  m_current_state->m_state_microseconds += game_delta;
  m_current_state->Update();

  m_inside_update = false;

  // Reset our keypresses for the next frame
  m_last_key_presses = m_key_presses;
  m_key_presses = 0;

  // Reset our mouse clicks for the next frame
  m_mouse.newPress = MouseButtons();
  m_mouse.released = MouseButtons();
}

void GameStateManager::Draw(Renderer &renderer) {

  if (!m_current_state)
    return;

  // NOTE: Sweet transition effects are *very* possible here... rendering
  // the previous state *and* the current state during some transition
  // would be really easy.

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0., static_cast<GLfloat>(GetStateHeight()), 0.);
  glScalef (1., -1., 1.);
  glTranslatef(0.375, 0.375, 0.);

  m_current_state->Draw(renderer);

  if (m_show_fps) {
    renderer.SetColor(White);
    TextWriter fps_writer(0, 0, renderer);
    fps_writer << "FPS: " << STRING(setprecision(2) << fixed << m_fps.GetFramesPerSecond());
  }

  glFlush ();
  renderer.SwapBuffers();
}

