// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __USER_SETTINGS_H
#define __USER_SETTINGS_H

#include <string>

#define SCHEMA "com.github.linthesia.linthesia"
#define SCHEMA_PATH "/com/github/linthesia/linthesia/"

#define MIN_KEY_KEY "min-key"
#define MAX_KEY_KEY "max-key"
#define OUTPUT_DEVICE_KEY "last-output-device"
#define INPUT_DEVICE_KEY "last-input-device"
#define KEYBOARD_SIZE_KEY "keyboard-size"
#define SQLITE_DB_KEY "sqlite-db"
#define REFRESH_RATE_KEY "refresh-rate"
#define SONG_LIB_DIR_SETTINGS_KEY "song-lib-last-dir"
#define SONG_LIB_PATH_KEY "song-lib-path"

namespace UserSetting {

   // This must be called exactly once before any of the following will work
   void Initialize();

   std::string Get(const std::string &setting,
		   const std::string &default_value);
  
   void Set(const std::string &setting, 
	    const std::string &value);
};

#endif // __USER_SETTINGS_H
