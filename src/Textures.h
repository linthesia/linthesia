// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __TEXTURES_H
#define __TEXTURES_H

enum Texture {

  // TextureResourceNames should have the same order
  TitleLogo, //0
  InterfaceButtons, //1
  GameMusicThemes, //2

  ButtonRetrySong, //3
  ButtonChooseTracks, //4
  ButtonExit, //5
  ButtonBackToTitle, //6
  ButtonPlaySong, //7

  ButtonNotPlayed, //8
  ButtonPlayedAuto, //9
  ButtonYouPlay, //10
  ButtonYouPlaySilent, //11
  ButtonLearning, //12
  ButtonLearningSilent, //13
  ButtonPlayedHidden, //14
  
  InputBox, //15
  OutputBox, //16
  EmptyBox, //17
  SongBox, //18
  DirBox, //19
  ButtonDirBack, //20
  ButtonPageBack, //21
  ButtonPageNext, //22

  TrackPanel, //23

  StatsText, //24

  PlayStatus, //25
  PlayStatus2, //26
  PlayKeys, //27

  PlayNotesBlackColor, //28
  PlayNotesBlackShadow, //29
  PlayNotesWhiteColor, //30
  PlayNotesWhiteShadow, //31

  PlayKeyRail, //32
  PlayKeyShadow, //33
  PlayKeysBlack, //34

  _TextureEnumCount //35
};

#endif // __TEXTURES_H
