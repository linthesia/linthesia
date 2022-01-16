// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include <string>

#include "OSGraphics.h"
#include "StringUtil.h"
#include "FileSelector.h"
#include "UserSettings.h"
#include "Version.h"
#include "CompatibleSystem.h"
#include "LinthesiaError.h"
#include "Tga.h"
#include "Renderer.h"
#include "SharedState.h"
#include "GameState.h"
#include "TitleState.h"
#include "DpmsThread.h"
#include "SongLibState.h"

#include "libmidi/Midi.h"
#include "libmidi/MidiUtil.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

#include <iostream>
#include <libgen.h>

#include <SDL_image.h>

using namespace std;

GameStateManager* state_manager;
SDL_Window* sdl_window;
bool main_loop_running = true;

char *sqlite_db_str;
sqlite3 *db;

const static string friendly_app_name = STRING("Linthesia " <<
                                               LinthesiaVersionString);

const static string error_header1 = "Linthesia detected a";
const static string error_header2 = " problem and must close:\n\n";
const static string error_footer = "\n\nIf you don't think this should have "
  "happened, please fill a bug report on : \nhttps://github.com/linthesia/linthesia\n\nThank you.";

const static int vsync_interval = 1;

class EdgeTracker  {
public:

  EdgeTracker() :
    active(true),
    just_active(true) {
  }

  void Activate() {
    just_active = true;
    active = true;
  }

  void Deactivate() {
    just_active = false;
    active = false;
  }

  bool IsActive() {
    return active;
  }

  bool JustActivated() {
    bool was_active = just_active;
    just_active = false;
    return was_active;
  }

private:
  bool active;
  bool just_active;
};

static EdgeTracker window_state;

class DrawingArea {
public:

  DrawingArea(SDL_Window* sdl_window) :
    m_sdl_window(sdl_window)
  {
  }

  bool GameLoop();

  void PollEvent(SDL_Event& event);

  virtual void on_configure_event();
protected:
  virtual void on_expose_event(SDL_WindowEvent& event);
  virtual void on_hide_event(SDL_WindowEvent& event);

  virtual bool on_motion_notify(SDL_MouseMotionEvent& event);
  virtual bool on_button_press(SDL_MouseButtonEvent& event);
  virtual bool on_key_press(SDL_KeyboardEvent& event);
  virtual bool on_key_release(SDL_KeyboardEvent& event);

  virtual void on_window_event(SDL_WindowEvent& event);

  int get_width()  const
  {
    int w;
    SDL_GetWindowSize(m_sdl_window, &w, nullptr);
    return w;
  }

  int get_height()  const
  {
    int h;
    SDL_GetWindowSize(m_sdl_window, nullptr, &h);
    return h;
  }


  SDL_Window* m_sdl_window;

};

void DrawingArea::PollEvent(SDL_Event& event)
{
  switch (event.type)
  {
    case SDL_MOUSEMOTION:
      on_motion_notify(event.motion);
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      on_button_press(event.button);
      break;
    case SDL_KEYDOWN:
      on_key_press(event.key);
      break;
    case SDL_KEYUP:
      on_key_release(event.key);
      break;
    case SDL_WINDOWEVENT:
      on_window_event(event.window);
      break;

    default:
      break;
  }
}

void DrawingArea::on_window_event(SDL_WindowEvent& event)
{
  switch (event.event)
  {
    case SDL_WINDOWEVENT_EXPOSED:
      on_expose_event(event);
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      on_hide_event(event);
      break;
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      on_configure_event();
      break;
  }
}

bool DrawingArea::on_motion_notify(SDL_MouseMotionEvent& event) {

  state_manager->MouseMove(event.x, event.y);
  return true;
}

bool DrawingArea::on_button_press(SDL_MouseButtonEvent& event) {

  MouseButton b;

  // left and right click allowed
  if (event.button == SDL_BUTTON_LEFT)
    b = MouseLeft;
  else if (event.button == SDL_BUTTON_RIGHT)
    b = MouseRight;

  // ignore other buttons
  else
    return false;

  // press or release?
  if (event.state == SDL_PRESSED)
    state_manager->MousePress(b);
  else if (event.state == SDL_RELEASED)
    state_manager->MouseRelease(b);
  else
    return false;

  return true;
}

// FIXME: use user settings to do this mapping
int keyToNote(SDL_KeyboardEvent& event) {
  const unsigned short oct = 4;

  switch(event.keysym.scancode) {
  /* no key for C :( */
  case SDL_SCANCODE_GRAVE:        return 12*oct + 1;      /* C# */
  case SDL_SCANCODE_TAB:          return 12*oct + 2;      /* D  */
  case SDL_SCANCODE_1:            return 12*oct + 3;      /* D# */
  case SDL_SCANCODE_Q:            return 12*oct + 4;      /* E  */
  case SDL_SCANCODE_W:            return 12*oct + 5;      /* F  */
  case SDL_SCANCODE_3:            return 12*oct + 6;      /* F# */
  case SDL_SCANCODE_E:            return 12*oct + 7;      /* G  */
  case SDL_SCANCODE_4:            return 12*oct + 8;      /* G# */
  case SDL_SCANCODE_R:            return 12*oct + 9;      /* A  */
  case SDL_SCANCODE_5:            return 12*oct + 10;     /* A# */
  case SDL_SCANCODE_T:            return 12*oct + 11;     /* B  */

  case SDL_SCANCODE_Y:            return 12*(oct+1) + 0;  /* C  */
  case SDL_SCANCODE_7:            return 12*(oct+1) + 1;  /* C# */
  case SDL_SCANCODE_U:            return 12*(oct+1) + 2;  /* D  */
  case SDL_SCANCODE_8:            return 12*(oct+1) + 3;  /* D# */
  case SDL_SCANCODE_I:            return 12*(oct+1) + 4;  /* E  */
  case SDL_SCANCODE_O:            return 12*(oct+1) + 5;  /* F  */
  case SDL_SCANCODE_0:            return 12*(oct+1) + 6;  /* F# */
  case SDL_SCANCODE_P:            return 12*(oct+1) + 7;  /* G  */
  case SDL_SCANCODE_MINUS:        return 12*(oct+1) + 8;  /* G# */
  case SDL_SCANCODE_LEFTBRACKET:  return 12*(oct+1) + 9;  /* A  */
  case SDL_SCANCODE_EQUALS:       return 12*(oct+1) + 10; /* A# */
  case SDL_SCANCODE_RIGHTBRACKET: return 12*(oct+1) + 11; /* B  */
  }

  return -1;
}

typedef set<int> ConnectMap;
ConnectMap pressed;

bool __sendNoteOff(int note)
{

  ConnectMap::iterator it = pressed.find(note);
  if (it == pressed.end())
    return false;

  sendNote(note, false);
  pressed.erase(it);

  return true;
}

bool DrawingArea::on_key_press(SDL_KeyboardEvent& event) {

  // if is a note...
  int note = keyToNote(event);
  if (note >= 0) {

    // if first press, send Note-On
    ConnectMap::iterator it = pressed.find(note);
    if (it == pressed.end())
    {
      sendNote(note, true);
      pressed.insert(note);
    }
    // otherwise, cancel emission of Note-off

    return true;
  }

  switch (event.keysym.sym)
  {
  case SDLK_UP:       state_manager->KeyPress(KeyUp);      break;
  case SDLK_DOWN:     state_manager->KeyPress(KeyDown);    break;
  case SDLK_LEFT:     state_manager->KeyPress(KeyLeft);    break;
  case SDLK_RIGHT:    state_manager->KeyPress(KeyRight);   break;
  case SDLK_SPACE:    state_manager->KeyPress(KeySpace);   break;
  case SDLK_RETURN:   state_manager->KeyPress(KeyEnter);   break;
  case SDLK_ESCAPE:   state_manager->KeyPress(KeyEscape);  break;

  // show FPS
  case SDLK_F6:       state_manager->KeyPress(KeyF6);      break;

  // increase/decrease octave
  case SDLK_PERIOD:    state_manager->KeyPress(KeyGreater); break;
  case SDLK_COMMA:     state_manager->KeyPress(KeyLess);    break;

  // +/- 5 seconds
  case SDLK_PAGEDOWN: state_manager->KeyPress(KeyForward);  break;
  case SDLK_PAGEUP:   state_manager->KeyPress(KeyBackward); break;

  case SDLK_KP_PLUS:  state_manager->KeyPress(KeyVolumeDown); break; // [
  case SDLK_KP_MINUS: state_manager->KeyPress(KeyVolumeUp);   break; // ]

  default:
    return false;
  }

  return true;
}

bool DrawingArea::on_key_release(SDL_KeyboardEvent& event) {

  // if is a note...
  int note = keyToNote(event);
  if (note >= 0)
  {
    ConnectMap::iterator it = pressed.find(note);
    if (it != pressed.end())
    {
      sendNote(note, false);
      pressed.erase(it);
    }
    return true;
  }

  return false;
}

void DrawingArea::on_configure_event() {
  glClearColor(.25, .25, .25, 1.0);
  glClearDepth(1.0);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glShadeModel(GL_SMOOTH);

  glViewport(0, 0, get_width(), get_height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, get_width(), 0, get_height(), -1, 1);

  state_manager->SetStateDimensions(get_width(), get_height());
  state_manager->Update(window_state.JustActivated());

  glEnd();
}

void DrawingArea::on_expose_event(SDL_WindowEvent& event) {
  if (!window_state.IsActive())
    window_state.Activate();
}

void DrawingArea::on_hide_event(SDL_WindowEvent& event) {
  if (window_state.IsActive())
    window_state.Deactivate();
}

bool DrawingArea::GameLoop() {

  if (window_state.IsActive()) {

    state_manager->Update(window_state.JustActivated());

    Renderer rend(m_sdl_window);
    rend.SetVSyncInterval(vsync_interval);

    state_manager->Draw(rend);
  }

  return true;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string & option)
{
    return std::find(begin, end, option) != end;
}

std::string getExePath()
// https://stackoverflow.com/questions/23943239/how-to-get-path-to-current-exe-file-on-linux
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( dirname(result), (count > 0) ? count : 0 );
}

int main(int argc, char *argv[]) {


  try {

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
      throw LinthesiaSDLError("error initializing SDL");

    string file_opt("");

    UserSetting::Initialize();

    if (cmdOptionExists(argv, argv+argc, "-f"))
      file_opt = string(getCmdOption(argv, argv + argc, "-f"));

    // TODO: parse from command line args
    bool windowed = cmdOptionExists(argv, argv+argc, "-w");
    bool fullscreen = cmdOptionExists(argv, argv+argc, "-W");

    // strip any leading or trailing quotes from the filename
    // argument (to match the format returned by the open-file
    // dialog later).
    if (file_opt.length() > 0 &&
        file_opt[0] == '\"')
      file_opt = file_opt.substr(1, file_opt.length() - 1);

    if (file_opt.length() > 0 &&
        file_opt[file_opt.length()-1] == '\"')
      file_opt = file_opt.substr(0, file_opt.length() - 1);

    Midi *midi = 0;

    // attempt to open the midi file given on the command line first
    if (file_opt != "") {
      try {
        midi = new Midi(Midi::ReadFromFile(file_opt));
      }

      catch (const MidiError &e) {
        string wrapped_description = STRING("Problem while loading file: " <<
                                            file_opt <<
                                            "\n") + e.GetErrorDescription();
        Compatible::ShowError(wrapped_description);

        file_opt = "";
        midi = 0;
      }
    }

    /* Loading the Sqlite Library
    */
    string tmp_user_db_str = UserSetting::Get(SQLITE_DB_KEY, "");

    if (tmp_user_db_str.empty() ) {
      struct stat st;

      // no user pref : let's create one !
      struct passwd *pw = getpwuid(getuid());
      sqlite_db_str = strcat(pw->pw_dir, "/.local/linthesia");
      if ( stat(sqlite_db_str, &st) == -1) {
        const int dir_err = mkdir(sqlite_db_str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_err)
        {
          fprintf(stderr, "Error creating directory : %s\n", sqlite_db_str);
          exit(1);
        }
      }
      sqlite_db_str = strcat(sqlite_db_str, "/music.sqlite");
      UserSetting::Set(SQLITE_DB_KEY, sqlite_db_str);
    } else {
        // user pref exist : let's use it !
        sqlite_db_str = (char*) tmp_user_db_str.c_str();
    }

    if (sqlite3_open(sqlite_db_str, &db)) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
    } else {
      // fprintf(stderr, "Opened database successfully\n");
      sqlite3_close(db);
    }

    const int default_sw = 1280;
    const int default_sh = 720;
    Uint32 flags = SDL_WINDOW_OPENGL;

    // Lauch fullscreen if asked for it OR if neither fullllscreen and windowed is asked AND we are not in jail.
    bool injail = true; // FIXME : how to detect we are injail without doing something nasty ?
                        // Jail is launched by AppImage, within :
                        //   firejail --quiet --noprofile --net=none --appimage ./"$FILENAME" &

    if (fullscreen || ( (!windowed && !fullscreen) && (!injail ) ) ) {
      flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else
    {
      flags |= SDL_WINDOW_RESIZABLE;
    }

    sdl_window = SDL_CreateWindow(
        friendly_app_name.c_str(),         // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        default_sw,                        // width, in pixels
        default_sh,                        // height, in pixels
        flags                              // flags - see below
    );

    // Check that the window was successfully created
    if (sdl_window == NULL)
      throw LinthesiaSDLError("Could not create window");


    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);
    if (gl_context == nullptr)
      throw LinthesiaSDLError("Could not create GL Context");

    {
      int w,h;
      SDL_GetWindowSize(sdl_window, &w, &h);
      state_manager = new GameStateManager(w, h);
    }
    struct stat st;
    chdir (getExePath().c_str());

    if ( !stat((GRAPHDIR +  std::string("/linthesia.png")).c_str(),&st) == 0) {
      throw LinthesiaError("FATAL : File not found : make install not done ?" +
                            std::string(GRAPHDIR) +  std::string("/linthesia.png"));
    }

    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
      throw LinthesiaSDLImageError("SDL_image could not initialize! SDL_image Error");

    if (TTF_Init() == -1)
      throw LinthesiaSDLTTFError("error in TTF_Init");

    std::string path = GRAPHDIR + std::string("/linthesia.png");
    SDL_Surface* image = IMG_Load(path.c_str());
    if (image == nullptr)
      throw LinthesiaSDLImageError( "Unable to load image " + path + "! SDL_image Error");

    SDL_SetWindowIcon(sdl_window, image);

    // Init DHMS thread once for the whole program
    DpmsThread* dpms_thread = new DpmsThread();

    // do this after gl context is created (ie. after da realized)
    SharedState state;
    state.dpms_thread = dpms_thread;
    if (midi) {
      state.song_title = FileSelector::TrimFilename(file_opt);
      state.midi = midi;
      state_manager->SetInitialState(new TitleState(state));
    }
    else {
      // if midi couldn't be opened from command line filename or there
      // simply was no command line filename, use a song-lib.
      state_manager->SetInitialState(new SongLibState(state));
    }

    // get refresh rate from user settings
    int default_rate = 300;

    string user_rate = UserSetting::Get(REFRESH_RATE_KEY, "");

    if (user_rate.empty()) {
      user_rate = STRING(default_rate);
      UserSetting::Set(REFRESH_RATE_KEY, user_rate);
    }
    else {
      istringstream iss(user_rate);
      if (not (iss >> default_rate)) {
        Compatible::ShowError("Invalid setting for 'refresh_rate' key.\n\nReset to default value when reload.");
        UserSetting::Set(REFRESH_RATE_KEY, "");
      }
    }

    UserSetting::Set(MIN_KEY_KEY, "");
    UserSetting::Set(MAX_KEY_KEY, "");

    if (cmdOptionExists(argv, argv+argc, "--min-key")) {
      string min_key = STRING(getCmdOption(argv, argv + argc, "--min-key"));
      UserSetting::Set(MIN_KEY_KEY, min_key);
    }

    if (cmdOptionExists(argv, argv+argc, "--max-key")) {
      string max_key = STRING(getCmdOption(argv, argv + argc, "--max-key"));
      UserSetting::Set(MAX_KEY_KEY, max_key);
    }

    DrawingArea da(sdl_window);
    da.on_configure_event();
    while (main_loop_running)
    {
      SDL_Event Event;
      while (SDL_PollEvent(&Event))
      {
        if (Event.type == SDL_QUIT)
        {
          main_loop_running = false;
        } else
        {
          da.PollEvent(Event);
        }
      }
      da.GameLoop();
    }
    midiStop();
    window_state.Deactivate();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);
    delete dpms_thread;
    SDL_Quit();

    return 0;
  }
  catch (const LinthesiaError &e) {
    string wrapped_description = STRING(error_header1 <<
                                        error_header2 <<
                                        e.GetErrorDescription() <<
                                        error_footer);
    Compatible::ShowError(wrapped_description);
  }

  catch (const MidiError &e) {
    string wrapped_description = STRING(error_header1 <<
                                        " MIDI" <<
                                        error_header2 <<
                                        e.GetErrorDescription() <<
                                        error_footer);
    Compatible::ShowError(wrapped_description);
  }
  catch (const exception &e) {
    string wrapped_description = STRING("Linthesia detected an unknown "
                                        "problem and must close!  '" <<
                                        e.what() << "'" << error_footer);
    Compatible::ShowError(wrapped_description);
  }

  catch (...) {
    string wrapped_description = STRING("Linthesia detected an unknown "
                                        "problem and must close!" <<
                                        error_footer);
    Compatible::ShowError(wrapped_description);
  }

  return 1;
}
