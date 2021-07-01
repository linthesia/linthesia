// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include <gconf/gconf-client.h>

#include "StringUtil.h"

using namespace std;

namespace UserSetting {

  static bool g_initialized(false);
  static string g_app_name("");
  static GConfClient* gconf;

  void Initialize(const string &app_name) {
    if (g_initialized) 
      return;

    gconf = gconf_client_get_default();
    g_app_name = "/apps/" + app_name;
    g_initialized = true;
  }

  string Get(const string &setting, const string &default_value) {
    if (!g_initialized) 
      return default_value;

    const char* value = gconf_client_get_string(gconf, (g_app_name + "/" + setting).c_str(), NULL);
    if (value == nullptr)
      return default_value;
    
    return value;
  }
    
  void Set(const string &setting, const string &value) {
    if (!g_initialized) 
      return;

    gconf_client_set_string(gconf,  (g_app_name + "/" + setting).c_str(), value.c_str(), NULL);
  }

}; // End namespace
