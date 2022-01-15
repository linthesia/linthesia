// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "UserSettings.h"
#include "StringUtil.h"
#include <giomm/settings.h>

using namespace std;

namespace UserSetting {

  static bool g_initialized(false);
  Glib::RefPtr<Gio::Settings> settings;

  void Initialize() {
    if (g_initialized) 
      return;

    settings = Gio::Settings::create(SCHEMA, SCHEMA_PATH);
    g_initialized = true;
  }

  string Get(const string &setting, const string &default_value) {
    if (!g_initialized) 
      return default_value;

    const char* value = settings->get_string(Glib::ustring(setting)).data();

    if (strcmp(value, "") == 0)
      return default_value;
    
    return value;
  }
    
  void Set(const string &setting, const string &value) {
    if (!g_initialized) 
      return;

    settings->set_string(setting.c_str(), value.c_str());
  }

}; // End namespace
