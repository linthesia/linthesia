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
#include <gconfmm.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

#include <iostream>
#include <libgen.h>

#ifndef GRAPHDIR
  #define GRAPHDIR "../graphics"
#endif

using namespace std;

GameStateManager* state_manager;

char *sqlite_db_str;
sqlite3 *db;

const static string application_name = "Linthesia";
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

class DrawingArea : public Gtk::GL::DrawingArea {
public:

  DrawingArea(const Glib::RefPtr<const Gdk::GL::Config>& config) :
    Gtk::GL::DrawingArea(config) {

    set_events(Gdk::POINTER_MOTION_MASK |
               Gdk::BUTTON_PRESS_MASK   |
               Gdk::BUTTON_RELEASE_MASK |
               Gdk::KEY_PRESS_MASK      |
               Gdk::KEY_RELEASE_MASK);

    set_can_focus();

    signal_motion_notify_event().connect(sigc::mem_fun(*this, &DrawingArea::on_motion_notify));
    signal_button_press_event().connect(sigc::mem_fun(*this, &DrawingArea::on_button_press));
    signal_button_release_event().connect(sigc::mem_fun(*this, &DrawingArea::on_button_press));
    signal_key_press_event().connect(sigc::mem_fun(*this, &DrawingArea::on_key_press));
    signal_key_release_event().connect(sigc::mem_fun(*this, &DrawingArea::on_key_release));
  }

  bool GameLoop();

protected:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);

  virtual bool on_motion_notify(GdkEventMotion* event);
  virtual bool on_button_press(GdkEventButton* event);
  virtual bool on_key_press(GdkEventKey* event);
  virtual bool on_key_release(GdkEventKey* event);
};

bool DrawingArea::on_motion_notify(GdkEventMotion* event) {

  state_manager->MouseMove(event->x, event->y);
  return true;
}

bool DrawingArea::on_button_press(GdkEventButton* event) {

  MouseButton b;

  // left and right click allowed
  if (event->button == 1)
    b = MouseLeft;
  else if (event->button == 3)
    b = MouseRight;

  // ignore other buttons
  else
    return false;

  // press or release?
  if (event->type == GDK_BUTTON_PRESS)
    state_manager->MousePress(b);
  else if (event->type == GDK_BUTTON_RELEASE)
    state_manager->MouseRelease(b);
  else
    return false;

  return true;
}

// FIXME: use user settings to do this mapping
int keyToNote(GdkEventKey* event) {
  const unsigned short oct = 4;

  switch(event->keyval) {
  /* no key for C :( */
  case GDK_masculine:  return 12*oct + 1;      /* C# */
  case GDK_Tab:        return 12*oct + 2;      /* D  */
  case GDK_1:          return 12*oct + 3;      /* D# */
  case GDK_q:          return 12*oct + 4;      /* E  */
  case GDK_w:          return 12*oct + 5;      /* F  */
  case GDK_3:          return 12*oct + 6;      /* F# */
  case GDK_e:          return 12*oct + 7;      /* G  */
  case GDK_4:          return 12*oct + 8;      /* G# */
  case GDK_r:          return 12*oct + 9;      /* A  */
  case GDK_5:          return 12*oct + 10;     /* A# */
  case GDK_t:          return 12*oct + 11;     /* B  */

  case GDK_y:          return 12*(oct+1) + 0;  /* C  */
  case GDK_7:          return 12*(oct+1) + 1;  /* C# */
  case GDK_u:          return 12*(oct+1) + 2;  /* D  */
  case GDK_8:          return 12*(oct+1) + 3;  /* D# */
  case GDK_i:          return 12*(oct+1) + 4;  /* E  */
  case GDK_o:          return 12*(oct+1) + 5;  /* F  */
  case GDK_0:          return 12*(oct+1) + 6;  /* F# */
  case GDK_p:          return 12*(oct+1) + 7;  /* G  */
  case GDK_apostrophe: return 12*(oct+1) + 8;  /* G# */
  case GDK_dead_grave: return 12*(oct+1) + 9;  /* A  */
  case GDK_exclamdown: return 12*(oct+1) + 10; /* A# */
  case GDK_plus:       return 12*(oct+1) + 11; /* B  */
  }

  return -1;
}

typedef map<int,sigc::connection> ConnectMap;
ConnectMap pressed;

bool __sendNoteOff(int note) {

  ConnectMap::iterator it = pressed.find(note);
  if (it == pressed.end())
    return false;

  sendNote(note, false);
  pressed.erase(it);

  return true;
}

bool DrawingArea::on_key_press(GdkEventKey* event) {

  // if is a note...
  int note = keyToNote(event);
  if (note >= 0) {

    // if first press, send Note-On
    ConnectMap::iterator it = pressed.find(note);
    if (it == pressed.end())
      sendNote(note, true);

    // otherwise, cancel emission of Note-off
    else
      it->second.disconnect();

    return true;
  }

  switch (event->keyval) {
  case GDK_Up:       state_manager->KeyPress(KeyUp);      break;
  case GDK_Down:     state_manager->KeyPress(KeyDown);    break;
  case GDK_Left:     state_manager->KeyPress(KeyLeft);    break;
  case GDK_Right:    state_manager->KeyPress(KeyRight);   break;
  case GDK_space:    state_manager->KeyPress(KeySpace);   break;
  case GDK_Return:   state_manager->KeyPress(KeyEnter);   break;
  case GDK_Escape:   state_manager->KeyPress(KeyEscape);  break;

  // show FPS
  case GDK_F6:       state_manager->KeyPress(KeyF6);      break;

  // increase/decrease octave
  case GDK_greater:  state_manager->KeyPress(KeyGreater); break;
  case GDK_less:     state_manager->KeyPress(KeyLess);    break;

  // +/- 5 seconds
  case GDK_Page_Down:state_manager->KeyPress(KeyForward);  break;
  case GDK_Page_Up:  state_manager->KeyPress(KeyBackward); break;

  case GDK_bracketleft:  state_manager->KeyPress(KeyVolumeDown); break; // [
  case GDK_bracketright: state_manager->KeyPress(KeyVolumeUp);   break; // ]

  default:
    return false;
  }

  return true;
}

bool DrawingArea::on_key_release(GdkEventKey* event) {

  // if is a note...
  int note = keyToNote(event);
  if (note >= 0) {

    // setup a timeout with Note-Off
    pressed[note] = Glib::signal_timeout().connect(
        sigc::bind<int>(sigc::ptr_fun(&__sendNoteOff), note), 20);
    return true;
  }

  return false;
}

void DrawingArea::on_realize() {
  // we need to call the base on_realize()
  Gtk::GL::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return;

  glwindow->gl_end();
}

bool DrawingArea::on_configure_event(GdkEventConfigure* event) {

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

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
  gluOrtho2D(0, get_width(), 0, get_height());

  state_manager->SetStateDimensions(get_width(), get_height());
  state_manager->Update(window_state.JustActivated());

  glwindow->gl_end();
  return true;
}

bool DrawingArea::on_expose_event(GdkEventExpose* event) {

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCallList(1);

  Renderer rend(get_gl_context(), get_pango_context());
  rend.SetVSyncInterval(vsync_interval);
  state_manager->Draw(rend);

  // swap buffers.
  if (glwindow->is_double_buffered())
     glwindow->swap_buffers();
  else
     glFlush();

  glwindow->gl_end();
  return true;
}

bool DrawingArea::GameLoop() {

  if (window_state.IsActive()) {

    state_manager->Update(window_state.JustActivated());

    Renderer rend(get_gl_context(), get_pango_context());
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
  Gtk::Main main_loop(argc, argv);
  Gtk::GL::init(argc, argv);

  try {
    string file_opt("");

    UserSetting::Initialize(application_name);

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

    Glib::RefPtr<Gdk::GL::Config> glconfig;

    // try double-buffered visual
    glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
                                       Gdk::GL::MODE_DEPTH  |
                                       Gdk::GL::MODE_DOUBLE);
    if (!glconfig) {
      cerr << "*** Cannot find the double-buffered visual.\n"
           << "*** Trying single-buffered visual.\n";

      // try single-buffered visual
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                         Gdk::GL::MODE_DEPTH);
      if (!glconfig) {
        string description = STRING(error_header1 <<
                                    " OpenGL" <<
                                    error_header2 <<
                                    "Cannot find any OpenGL-capable visual." <<
                                    error_footer);
        Compatible::ShowError(description);
        return 1;
      }
    }
    
    /* Loading the Sqlite Library
    */
    string tmp_user_db_str = UserSetting::Get("sqlite_db", "");

    if (tmp_user_db_str. empty() ) {
        // no user pref : let's create one !
        struct passwd *pw = getpwuid(getuid());
        sqlite_db_str = strcat(pw->pw_dir, "/.local/linthesia");
        const int dir_err = mkdir(sqlite_db_str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_err)
        {
          fprintf(stderr, "Error creating directory : %s\n", sqlite_db_str);
          exit(1);
        }
        sqlite_db_str = strcat(sqlite_db_str, "/music.sqlite");
        UserSetting::Set("sqlite_db", sqlite_db_str);
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

    const int default_sw = 1024;
    const int default_sh = 768;
    int sh = Compatible::GetDisplayHeight();
    int sw = Compatible::GetDisplayWidth();
    state_manager = new GameStateManager(sw, sh);

    Gtk::Window window;
    window.set_default_size(default_sw, default_sh);
    DrawingArea da(glconfig);
    window.add(da);
    window.show_all();

    window.set_title(friendly_app_name);

    struct stat st;
    chdir (getExePath().c_str());  
    
    if ( !stat((GRAPHDIR +  std::string("/linthesia.png")).c_str(),&st) == 0) {
       fprintf(stderr, "FATAL : File not found : make install not done ?\n");
       cout << (GRAPHDIR +  std::string("/linthesia.png")) << "\n";
    //   exit(0);
    }

    window.set_icon_from_file(GRAPHDIR + std::string("/linthesia.png"));

    // Lauch fullscreen if asked for it OR if neither fullllscreen and windowed is asked AND we are not in jail.
    bool injail = true; // FIXME : how to detect we are injail without doing something nasty ?
                        // Jail is launched by AppImage, within :
                        //   firejail --quiet --noprofile --net=none --appimage ./"$FILENAME" &

    if (fullscreen || ( (!windowed && !fullscreen) && (!injail ) ) ) {
        window.fullscreen();
    }
    else {
        if (sw < default_sw) {
          fprintf (stderr, "Your display width is smaller than window size : %d < %d\n", sw, default_sw);
        }

        if (sh < default_sh) {
          fprintf (stderr, "Your display height is smaller than window size : %d < %d\n", sh, default_sh);
        }

        window.maximize();
    }

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
    int default_rate = 30;

    string user_rate = UserSetting::Get("refresh_rate", "");

    if (! user_rate.empty() && std::stoi(user_rate) > default_rate) {
      fprintf (stdout, "WARNING :: Your refresh_rate is set to %d. I recommand using %d.\n", std::stoi(user_rate), default_rate);
      fprintf (stdout, "           You may update it using gconf-2.\n");
    }

    if (user_rate.empty()) {
      user_rate = STRING(default_rate);
      UserSetting::Set("refresh_rate", user_rate);
    }
    else {
      istringstream iss(user_rate);
      if (not (iss >> default_rate)) {
        Compatible::ShowError("Invalid setting for 'refresh_rate' key.\n\nReset to default value when reload.");
        UserSetting::Set("refresh_rate", "");
      }
    }

    Glib::signal_timeout().connect(sigc::mem_fun(da, &DrawingArea::GameLoop), 1000/std::stoi(user_rate));

    UserSetting::Set("min_key", "");
    UserSetting::Set("max_key", "");

    if (cmdOptionExists(argv, argv+argc, "--min-key")) {
      string min_key = STRING(getCmdOption(argv, argv + argc, "--min-key"));
      UserSetting::Set("min_key", min_key);
    }

    if (cmdOptionExists(argv, argv+argc, "--max-key")) {
      string max_key = STRING(getCmdOption(argv, argv + argc, "--max-key"));
      UserSetting::Set("max_key", max_key);
    }


    main_loop.run(window);
    window_state.Deactivate();

    delete dpms_thread;

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

  catch (const Gnome::Conf::Error& e) {
    string wrapped_description = STRING(error_header1 <<
                                        " Gnome::Conf::Error" <<
                                        error_header2 <<
                                        e.what() <<
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

