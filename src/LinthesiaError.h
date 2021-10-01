// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __LINTHESIA_ERROR_H__
#define __LINTHESIA_ERROR_H__

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

enum LinthesiaErrorCode {

   Error_StringSpecified,
   Error_BadPianoType,
   Error_BadGameState
};


class LinthesiaError : public std::exception {
public:

  // TODO: This would be a sweet place to add stack-trace information...

  LinthesiaError(LinthesiaErrorCode error) :
    m_error(error),
    m_optional_string("") {
  }

  LinthesiaError(const std::string error) :
    m_error(Error_StringSpecified),
    m_optional_string(error) {
  }

  std::string GetErrorDescription() const;

  ~LinthesiaError() throw() { }

  const LinthesiaErrorCode m_error;

private:
   const std::string m_optional_string;
   LinthesiaError operator=(const LinthesiaError&);
};

class LinthesiaSDLError : public LinthesiaError {
public:
  LinthesiaSDLError(const std::string base_error) :
    LinthesiaError(base_error + ": " + std::string(SDL_GetError())) {
  }
};

class LinthesiaSDLImageError : public LinthesiaError {
public:
  LinthesiaSDLImageError(const std::string base_error) :
    LinthesiaError(base_error + ": " + std::string(IMG_GetError())) {
  }
};

class LinthesiaSDLTTFError : public LinthesiaError {
public:
  LinthesiaSDLTTFError(const std::string base_error) :
    LinthesiaError(base_error + ": " + std::string(TTF_GetError())) {
  }
};



#endif // __LINTHESIA_ERROR_H__
