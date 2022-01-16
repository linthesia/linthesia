// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include <sys/time.h>

#include "MidiComm.h"
#include "CompatibleSystem.h"
#include "StringUtil.h"
#include "Version.h"

#include <SDL.h>

using namespace std;

extern SDL_Window* sdl_window;
extern bool main_loop_running;

namespace Compatible {

  unsigned long GetMilliseconds() {

    timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  }

  unsigned long GetMicroseconds() {

    timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
  }


  void ShowError(const string &err) {

    const static string friendly_app_name =
      STRING("Linthesia " << LinthesiaVersionString);
    const static string message_box_title =
      STRING(friendly_app_name << " Error");

    printf("%s\n", err.c_str());
    if (sdl_window)
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                               message_box_title.c_str(),
                               err.c_str(),
                               sdl_window);
    }
  }

  void HideMouseCursor() {
    SDL_ShowCursor(SDL_DISABLE);
  }

  void ShowMouseCursor() {
    SDL_ShowCursor(SDL_ENABLE);
  }

  int GetWindowHeight()
  {
    int h;
    SDL_GetWindowSize(sdl_window, nullptr, &h);
    return h;
  }


  void GracefulShutdown() {
    main_loop_running = false;
  }

}; // End namespace
