// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include <set>

#include "LinthesiaError.h"
#include "FileSelector.h"
#include "UserSettings.h"
#include "StringUtil.h"

using namespace std;

const static char PathDelimiter = '/';

namespace FileSelector {


  string TrimFilename(const string &filename) {
    // lowercase
    string lower = StringLower(filename);

    // remove extension, if known
    set<string> exts;
    exts.insert(".mid");
    exts.insert(".midi");
    for (set<string>::const_iterator i = exts.begin(); i != exts.end(); i++) {
      int len = i->length();
      if (lower.length() > len && lower.substr(lower.length() - len, len) == *i)
        lower = lower.substr(0, lower.length() - len);
    }

    // remove path
    string::size_type i = lower.find_last_of("/");
    if (i != string::npos)
      lower = lower.substr(i+1, lower.length());

    return lower;
  }

}; // End namespace
