// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <map>
#include <vector>
#include <string>

#include "TrackTile.h"
#include "TrackProperties.h"

#include "libmidi/Note.h"
#include "libmidi/MidiTypes.h"
#include "EnumTile.h"

class KeyboardSize: public EnumValue
{
public:
  enum Value
  {
    Size37 = 37,
    Size49 = 49,
    Size61 = 61,
    Size76 = 76,
    Size88 = 88
  };

  constexpr KeyboardSize(): value(Size88) {};
  KeyboardSize(std::string key);
  constexpr KeyboardSize(Value value) : value(value) { }

  operator Value() const { return value; }
  explicit operator bool() = delete;

  std::string GetValue() const;

  void next() override;
  void previous() override;

  std::string AsText() const override;

private:
  Value value;
};



/** class that represents keyboard information
 */
class Keyboard 
{
public:

  Keyboard();
  Keyboard(KeyboardSize size,
           int starting_octave, char starting_note, int white_key_count, 
           int min_playing_note, int max_playing_note, int keyboard_type_offset);

  // Retrieves which white-key a piano with the given key count
  // will start with on the far left side
  char GetStartingNote() const;

  // Retrieves which octave a piano with the given key count
  // will start with on the far left side
  int GetStartingOctave() const;

  // Retrieves the number of white keys a piano with the given
  // key count will contain
  int GetWhiteKeyCount() const;

  // retrieves the minimum playing node
  int GetMinPlayableNote() const;

  // retrieves the maximum playing node
  int GetMaxPlayableNote() const;

  int GetKeyboardTypeOffset() const;

  static Keyboard GetKeyboardDefaults(KeyboardSize size);

  KeyboardSize GetSize() const;

private:
  KeyboardSize m_size;
  int m_starting_octave;
  char m_starting_note;
  int m_white_key_count;
  int m_min_playable_note;
  int m_max_playable_note;
  int m_keyboard_type_offset;
};

#endif // __KEYBOARD_H
