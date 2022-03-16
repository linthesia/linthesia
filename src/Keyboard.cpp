// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "Keyboard.h"
#include "LinthesiaError.h"
#include "StringUtil.h"

#include "Renderer.h"
#include "Textures.h"
#include "Tga.h"

using namespace std;

KeyboardSize::KeyboardSize(std::string key)
{
  value = Size88;
  try
  {
    int val = std::stoi(key);
    switch (val)
    {
      case Size37:
      case Size49:
      case Size61:
      case Size76:
      case Size88:
        value = static_cast<Value>(val);
    }
  } 
  catch (std::invalid_argument& ex)
  {
    
  }
}

std::string KeyboardSize::GetValue() const 
{
  return std::to_string(value);
}

void KeyboardSize::next() 
{
  switch (value)
  {
    case Size37:
      value = Size49;
      break;
    case Size49:
      value = Size61;
      break;
    case Size61:
      value = Size76;
      break;
    case Size76:
    case Size88:
    default:
      value = Size88;
  }
}

void KeyboardSize::previous()
{
  switch (value)
  {
    case Size37:
    case Size49:
      value = Size37;
      break;
    case Size61:
      value = Size49;
      break;
    case Size76:
      value = Size61;
      break;
    case Size88:
      value = Size76;
      break;
    default:
      value = Size88;
      break;
  }
}

std::string KeyboardSize::AsText() const
{
  switch (value)
  {
    case Size37:
      return "37 keys";
    case Size49:
      return "49 keys";
    case Size61:
      return "61 keys";
    case Size76:
      return "76 keys";
    case Size88:
      return "88 keys";
  }
  return "unknown";
}


Keyboard::Keyboard()
{
  *this = Keyboard::GetKeyboardDefaults(KeyboardSize::Size88);
}

Keyboard::Keyboard(KeyboardSize size, 
                   int starting_octave, char starting_note, int white_key_count, 
                   int min_playing_note, int max_playing_note, int keyboard_type_offset):
  m_size(size),
  m_starting_octave(starting_octave),
  m_starting_note(starting_note),
  m_white_key_count(white_key_count),
  m_min_playable_note(min_playing_note),
  m_max_playable_note(max_playing_note),
  m_keyboard_type_offset(keyboard_type_offset)
{
}

int Keyboard::GetStartingOctave() const 
{
  return m_starting_octave;
}

char Keyboard::GetStartingNote() const 
{
  return m_starting_note;
}

int Keyboard::GetWhiteKeyCount() const 
{
  return m_white_key_count;
}

int Keyboard::GetMinPlayableNote() const 
{
  return m_min_playable_note;
}

int Keyboard::GetMaxPlayableNote() const
{
  return m_max_playable_note;
}

int Keyboard::GetKeyboardTypeOffset() const
{
  constexpr unsigned int WhiteNotesPerOctave = 7;
  return m_keyboard_type_offset - WhiteNotesPerOctave;
}

KeyboardSize Keyboard::GetSize() const 
{
  return m_size;
}




Keyboard Keyboard::GetKeyboardDefaults(KeyboardSize size)
{
  // Source: Various "Specification" pages at Yamaha's website
  constexpr int StartingOctaveOn37 = 3;
  constexpr int StartingOctaveOn49 = 2;
  constexpr int StartingOctaveOn61 = 2;
  constexpr int StartingOctaveOn76 = 1;
  constexpr int StartingOctaveOn88 = 0;
  constexpr char StartingKeyOn37 = 'F'; // F3-F6
  constexpr char StartingKeyOn49 = 'C'; // C2-C6
  constexpr char StartingKeyOn61 = 'C'; // C2-C7
  constexpr char StartingKeyOn76 = 'E'; // E1-G7 
  constexpr char StartingKeyOn88 = 'A'; // A0-C8

  // Source: Google Image Search
  constexpr int WhiteKeysOn37 = 22;
  constexpr int WhiteKeysOn49 = 29;
  constexpr int WhiteKeysOn61 = 36;
  constexpr int WhiteKeysOn76 = 45;
  constexpr int WhiteKeysOn88 = 52;


  constexpr int MinKeyOn37 = 65, MaxKeyOn37 = 101;
  constexpr int MinKeyOn49 = 48, MaxKeyOn49 =  96;
  constexpr int MinKeyOn61 = 48, MaxKeyOn61 = 108;
  constexpr int MinKeyOn76 = 40, MaxKeyOn76 = 115;
  constexpr int MinKeyOn88 =  0, MaxKeyOn88 = 120;
  
  // The constants used in the switch below refer to the number
  // of white keys off 'C' that type of piano starts on
  constexpr int KeyboardOffsetOn37 = 4;
  constexpr int KeyboardOffsetOn49 = 7;
  constexpr int KeyboardOffsetOn61 = 7;
  constexpr int KeyboardOffsetOn76 = 5;
  constexpr int KeyboardOffsetOn88 = 2;

  switch (size)
  {
    case KeyboardSize::Size37: return Keyboard(size, StartingOctaveOn37, StartingKeyOn37, WhiteKeysOn37, MinKeyOn37, MaxKeyOn37, KeyboardOffsetOn37);
    case KeyboardSize::Size49: return Keyboard(size, StartingOctaveOn49, StartingKeyOn49, WhiteKeysOn49, MinKeyOn49, MaxKeyOn49, KeyboardOffsetOn49);
    case KeyboardSize::Size61: return Keyboard(size, StartingOctaveOn61, StartingKeyOn61, WhiteKeysOn61, MinKeyOn61, MaxKeyOn61, KeyboardOffsetOn61);
    case KeyboardSize::Size76: return Keyboard(size, StartingOctaveOn76, StartingKeyOn76, WhiteKeysOn76, MinKeyOn76, MaxKeyOn76, KeyboardOffsetOn76);
    case KeyboardSize::Size88: return Keyboard(size, StartingOctaveOn88, StartingKeyOn88, WhiteKeysOn88, MinKeyOn88, MaxKeyOn88, KeyboardOffsetOn88);
    default: throw LinthesiaError(Error_BadPianoType);
  }
}

