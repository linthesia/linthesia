// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include <sys/time.h>
#include <gtkmm.h>

#include "MidiComm.h"
#include "CompatibleSystem.h"
#include "StringUtil.h"
#include "Version.h"

using namespace std;

namespace Compatible {

  unsigned long GetMilliseconds() {

    timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  }


  void ShowError(const string &err) {

    const static string friendly_app_name =
      STRING("Linthesia " << LinthesiaVersionString);
    const static string message_box_title =
      STRING(friendly_app_name << " Error");

    Gtk::MessageDialog dialog(err, false, Gtk::MESSAGE_ERROR);
    dialog.run();
  }

  void HideMouseCursor() {
    // TODO
  }

  void ShowMouseCursor() {
    // TODO
  }

  int GetDisplayWidth() {
    return Gdk::Screen::get_default()->get_width();
  }

  int GetDisplayHeight() {
    return Gdk::Screen::get_default()->get_height();
  }

  void GracefulShutdown() {
    midiStop();
    Gtk::Main::instance()->quit();
  }

}; // End namespace
