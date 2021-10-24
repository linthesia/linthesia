// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __TEXTURES_H
#define __TEXTURES_H

enum Texture {

  // TextureResourceNames should have the same order
  TitleLogo,
  InterfaceButtons,
  GameMusicThemes,

  ButtonRetrySong,
  ButtonChooseTracks,
  ButtonExit,
  ButtonBackToTitle,
  ButtonPlaySong,

  ButtonNotPlayed,
  ButtonPlayedAuto,
  ButtonYouPlay,
  ButtonYouPlaySilent,
  ButtonLearning,
  ButtonLearningSilent,
  ButtonPlayedHidden,
  
  InputBox,
  OutputBox,
  EmptyBox,
  SongBox,
  DirBox,
  ButtonDirBack,
  ButtonPageBack,
  ButtonPageNext,

  TrackPanel,

  StatsText,

  PlayStatus,
  PlayStatus2,
  PlayKeys,

  PlayNotesBlackColor,
  PlayNotesBlackShadow,
  PlayNotesWhiteColor,
  PlayNotesWhiteShadow,

  PlayKeyRail,
  PlayKeyShadow,
  PlayKeysBlack,

  _TextureEnumCount
};

#endif // __TEXTURES_H
