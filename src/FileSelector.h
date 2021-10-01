// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __FILE_SELECTOR_H
#define __FILE_SELECTOR_H

#include <string>

namespace FileSelector {

  // Returns a filename with no path or .mid/.midi extension
  std::string TrimFilename(const std::string &filename);
};

#endif  // __FILE_SELECTOR_H

