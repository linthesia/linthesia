// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "PlayingState.h"
#include "TrackSelectionState.h"
#include "StatsState.h"
#include "Renderer.h"
#include "Textures.h"
#include "CompatibleSystem.h"

#include <string>
#include <iomanip>

#include "StringUtil.h"
#include "MenuLayout.h"
#include "TextWriter.h"

#include "libmidi/Midi.h"
#include "libmidi/MidiTrack.h"
#include "libmidi/MidiEvent.h"
#include "libmidi/MidiUtil.h"

#include "MidiComm.h"

using namespace std;


void PlayingState::SetupNoteState() {

  TranslatedNoteSet old = m_notes;
  m_notes.clear();

  for (TranslatedNoteSet::const_iterator i = old.begin(); i != old.end(); ++i) {
    TranslatedNote n = *i;

    n.state = AutoPlayed;
    if (m_state.track_properties[n.track_id].mode == Track::ModeYouPlay)
      n.state = UserPlayable;

    m_notes.insert(n);
  }
}

void PlayingState::ResetSong() {

  if (m_state.midi_out)
    m_state.midi_out->Reset();

  if (m_state.midi_in)
    m_state.midi_in->Reset();

  // TODO: These should be moved to a configuration file
  // along with ALL other "const static something" variables.
  const static microseconds_t LeadIn = 5500000;
  const static microseconds_t LeadOut = 1000000;

  if (!m_state.midi)
    return;

  m_state.midi->Reset(LeadIn, LeadOut);

  m_notes = m_state.midi->Notes();
  SetupNoteState();

  m_state.stats = SongStatistics();
  m_state.stats.total_note_count = static_cast<int>(m_notes.size());

  m_current_combo = 0;

  m_note_offset = 0;
  m_max_allowed_title_alpha = 1.0;
}

PlayingState::PlayingState(const SharedState &state) :
  m_paused(false),
  m_keyboard(0),
  m_any_you_play_tracks(false),
  m_first_update(true),
  m_state(state) {
}

void PlayingState::Init() {

  if (!m_state.midi)
    throw GameStateError("PlayingState: Init was passed a null MIDI!");

  m_look_ahead_you_play_note_count = 0;
  for (size_t i = 0; i < m_state.track_properties.size(); ++i) {

    if (m_state.track_properties[i].mode == Track::ModeYouPlay) {
      m_look_ahead_you_play_note_count += m_state.midi->Tracks()[i].Notes().size();
      m_any_you_play_tracks = true;
    }
  }

  // This many microseconds of the song will
  // be shown on the screen at once
  const static microseconds_t DefaultShowDurationMicroseconds = 3250000;
  m_show_duration = DefaultShowDurationMicroseconds;

  m_keyboard = new KeyboardDisplay(KeyboardSize88, GetStateWidth() - Layout::ScreenMarginX*2, CalcKeyboardHeight());

  // Hide the mouse cursor while we're playing
  Compatible::HideMouseCursor();

  ResetSong();
}

PlayingState::~PlayingState() {
  Compatible::ShowMouseCursor();
}

int PlayingState::CalcKeyboardHeight() const {
  // Start with the size of the screen
  int height = GetStateHeight();

  // Allow a couple lines of text below the keys
  height -= Layout::ButtonFontSize * 8;

  return height;
}

void PlayingState::Play(microseconds_t delta_microseconds) {

  MidiEventListWithTrackId evs = m_state.midi->Update(delta_microseconds);

  const size_t length = evs.size();
  for(size_t i = 0; i < length; ++i) {

    const size_t &track_id = evs[i].first;
    const MidiEvent &ev = evs[i].second;

    // Draw refers to the keys lighting up (automatically) -- not necessarily
    // the falling notes.  The KeyboardDisplay object contains its own logic
    // to decide how to draw the falling notes
    bool draw = false;
    bool play = false;
    switch (m_state.track_properties[track_id].mode) {
    case Track::ModeNotPlayed:           draw = false;  play = false;  break;
    case Track::ModePlayedButHidden:     draw = false;  play = true;   break;
    case Track::ModeYouPlay:             draw = false;  play = false;  break;
    case Track::ModePlayedAutomatically: draw = true;   play = true;   break;
    case Track::ModeCount: break;
    }

    // Even in "You Play" tracks, we have to play the non-note
    // events as per usual.
    if (m_state.track_properties[track_id].mode
        && ev.Type() != MidiEventType_NoteOn
        && ev.Type() != MidiEventType_NoteOff)
      play = true;


    if (draw && (ev.Type() == MidiEventType_NoteOn || ev.Type() == MidiEventType_NoteOff)) {
      int vel = ev.NoteVelocity();
      const string name = MidiEvent::NoteName(ev.NoteNumber());

      m_keyboard->SetKeyActive(name, (vel > 0), m_state.track_properties[track_id].color);
    }

    if (play && m_state.midi_out)
      m_state.midi_out->Write(ev);
  }
}

double PlayingState::CalculateScoreMultiplier() const {
  const static double MaxMultiplier = 5.0;
  double multiplier = 1.0;

  const double combo_addition = m_current_combo / 10.0;
  multiplier += combo_addition;

  return min(MaxMultiplier, multiplier);
}

void PlayingState::Listen() {
  if (!m_state.midi_in)
    return;

  while (m_state.midi_in->KeepReading()) {

    microseconds_t cur_time = m_state.midi->GetSongPositionInMicroseconds();
    MidiEvent ev = m_state.midi_in->Read();

    // Just eat input if we're paused
    if (m_paused)
      continue;

    // We're only interested in NoteOn and NoteOff
    if (ev.Type() != MidiEventType_NoteOn && ev.Type() != MidiEventType_NoteOff)
      continue;

    // Octave Sliding
    ev.ShiftNote(m_note_offset);

    string note_name = MidiEvent::NoteName(ev.NoteNumber());

    // On key release we have to look for existing "active" notes and turn them off.
    if (ev.Type() == MidiEventType_NoteOff || ev.NoteVelocity() == 0) {

      // NOTE: This assumes mono-channel input.  If they're piping an entire MIDI file
      //       (or even the *same* MIDI file) through another source, we could get the
      //       same NoteId on different channels -- and this code would start behaving
      //       incorrectly.
      for (ActiveNoteSet::iterator i = m_active_notes.begin(); i != m_active_notes.end(); ++i) {
        if (ev.NoteNumber() != i->note_id)
          continue;

        // Play it on the correct channel to turn the note we started
        // previously, off.
        ev.SetChannel(i->channel);
        if (m_state.midi_out)
          m_state.midi_out->Write(ev);

        m_active_notes.erase(i);
        break;
      }

      m_keyboard->SetKeyActive(note_name, false, Track::FlatGray);
      continue;
    }

    bool any_found = false;

    TranslatedNoteSet::iterator closest_match = m_notes.end();
    for (TranslatedNoteSet::iterator i = m_notes.begin(); i != m_notes.end(); ++i) {

      const microseconds_t window_start = i->start - (KeyboardDisplay::NoteWindowLength / 2);
      const microseconds_t window_end = i->start + (KeyboardDisplay::NoteWindowLength / 2);

      // As soon as we start processing notes that couldn't possibly
      // have been played yet, we're done.
      if (window_start > cur_time)
        break;

      if (i->state != UserPlayable)
        continue;

      if (window_end > cur_time && i->note_id == ev.NoteNumber()) {

        if (closest_match == m_notes.end()) {
          closest_match = i;
          continue;
        }

        microseconds_t this_distance = cur_time - i->start;
        if (i->start > cur_time)
          this_distance = i->start - cur_time;

        microseconds_t known_best = cur_time - closest_match->start;
        if (closest_match->start > cur_time)
          known_best = closest_match->start - cur_time;

        if (this_distance < known_best)
          closest_match = i;
      }
    }

    Track::TrackColor note_color = Track::FlatGray;

    if (closest_match != m_notes.end()) {
      any_found = true;
      note_color = m_state.track_properties[closest_match->track_id].color;

      // "Open" this note so we can catch the close later and turn off
      // the note.
      ActiveNote n;
      n.channel = closest_match->channel;
      n.note_id = closest_match->note_id;
      n.velocity = closest_match->velocity;
      m_active_notes.insert(n);

      // Play it
      ev.SetChannel(n.channel);
      ev.SetVelocity(n.velocity);

      if (m_state.midi_out)
        m_state.midi_out->Write(ev);

      // Adjust our statistics
      const static double NoteValue = 100.0;
      m_state.stats.score += NoteValue * CalculateScoreMultiplier() * (m_state.song_speed / 100.0);

      m_state.stats.notes_user_could_have_played++;
      m_state.stats.speed_integral += m_state.song_speed;

      m_state.stats.notes_user_actually_played++;
      m_current_combo++;
      m_state.stats.longest_combo = max(m_current_combo, m_state.stats.longest_combo);

      TranslatedNote replacement = *closest_match;
      replacement.state = UserHit;

      m_notes.erase(closest_match);
      m_notes.insert(replacement);
    }

    else
      m_state.stats.stray_notes++;

    m_state.stats.total_notes_user_pressed++;
    m_keyboard->SetKeyActive(note_name, true, note_color);
  }
}

void PlayingState::Update() {

  // Calculate how visible the title bar should be
  const static double fade_in_ms = 350.0;
  const static double stay_ms = 2500.0;
  const static double fade_ms = 500.0;

  m_title_alpha = 0.0;
  unsigned long ms = GetStateMilliseconds() * max(m_state.song_speed, 50) / 100;

  if (double(ms) <= stay_ms)
    m_title_alpha = min(1.0, ms / fade_in_ms);

  if (double(ms) >= stay_ms)
    m_title_alpha = min(max((fade_ms - (ms - stay_ms)) / fade_ms, 0.0), 1.0);

  // Lock down the alpha so that if you are slowing the song down as it
  // fades out, it doesn't cut back into a much higher alpha value
  m_title_alpha = min(m_title_alpha, m_max_allowed_title_alpha);

  if (double(ms) > stay_ms)
    m_max_allowed_title_alpha = m_title_alpha;

  microseconds_t delta_microseconds = static_cast<microseconds_t>(GetDeltaMilliseconds()) * 1000;

  // The 100 term is really paired with the playback speed, but this
  // formation is less likely to produce overflow errors.
  delta_microseconds = (delta_microseconds / 100) * m_state.song_speed;

  if (m_paused)
    delta_microseconds = 0;

  // Our delta milliseconds on the first frame after state start is extra
  // long because we just reset the MIDI.  By skipping the "Play" that
  // update, we don't have an artificially fast-forwarded start.
  if (!m_first_update) {
    Play(delta_microseconds);
    Listen();
  }

  m_first_update = false;

  microseconds_t cur_time = m_state.midi->GetSongPositionInMicroseconds();

  // Delete notes that are finished playing (and are no longer available to hit)
  TranslatedNoteSet::iterator i = m_notes.begin();
  while (i != m_notes.end()) {
    TranslatedNoteSet::iterator note = i++;

    const microseconds_t window_end = note->start + (KeyboardDisplay::NoteWindowLength / 2);

    if (m_state.midi_in && note->state == UserPlayable && window_end <= cur_time) {
      TranslatedNote note_copy = *note;
      note_copy.state = UserMissed;

      m_notes.erase(note);
      m_notes.insert(note_copy);

      // Re-connect the (now-invalid) iterator to the replacement
      note = m_notes.find(note_copy);
    }

    if (note->start > cur_time)
      break;

    if (note->end < cur_time && window_end < cur_time) {

      if (note->state == UserMissed) {
        // They missed a note, reset the combo counter
        m_current_combo = 0;

        m_state.stats.notes_user_could_have_played++;
        m_state.stats.speed_integral += m_state.song_speed;
      }

      m_notes.erase(note);
    }
  }

  if(IsKeyPressed(KeyGreater))
    m_note_offset += 12;

  if(IsKeyPressed(KeyLess))
    m_note_offset -= 12;

  if (IsKeyPressed(KeyUp)) {
    m_show_duration -= 250000;

    const static microseconds_t MinShowDuration = 250000;
    if (m_show_duration < MinShowDuration)
      m_show_duration = MinShowDuration;
  }

  if (IsKeyPressed(KeyDown)) {
    m_show_duration += 250000;

    const static microseconds_t MaxShowDuration = 10000000;
    if (m_show_duration > MaxShowDuration)
      m_show_duration = MaxShowDuration;
  }

  if (IsKeyPressed(KeyLeft)) {
    m_state.song_speed -= 10;
    if (m_state.song_speed < 0)
      m_state.song_speed = 0;
  }

  if (IsKeyPressed(KeyRight)) {
    m_state.song_speed += 10;
    if (m_state.song_speed > 400)
      m_state.song_speed = 400;
  }

  if (IsKeyPressed(KeySpace))
    m_paused = !m_paused;

  if (IsKeyPressed(KeyEscape)) {
    if (m_state.midi_out)
      m_state.midi_out->Reset();

    if (m_state.midi_in)
      m_state.midi_in->Reset();

    ChangeState(new TrackSelectionState(m_state));
    return;
  }

  if (m_state.midi->IsSongOver()) {
    if (m_state.midi_out)
      m_state.midi_out->Reset();

    if (m_state.midi_in)
      m_state.midi_in->Reset();

    if (m_state.midi_in && m_any_you_play_tracks)
      ChangeState(new StatsState(m_state));

    else
      ChangeState(new TrackSelectionState(m_state));

    return;
  }
}

void PlayingState::Draw(Renderer &renderer) const {

  const Tga *key_tex[3] = { GetTexture(PlayKeyRail),
                            GetTexture(PlayKeyShadow),
                            GetTexture(PlayKeysBlack, true) };

  const Tga *note_tex[4] = { GetTexture(PlayNotesWhiteShadow, true),
                             GetTexture(PlayNotesBlackShadow, true),
                             GetTexture(PlayNotesWhiteColor, true),
                             GetTexture(PlayNotesBlackColor, true) };
  renderer.ForceTexture(0);

  m_keyboard->Draw(renderer, key_tex, note_tex, Layout::ScreenMarginX, 0, m_notes, m_show_duration,
                   m_state.midi->GetSongPositionInMicroseconds(), m_state.track_properties);

  string title_text = m_state.song_title;

  double alpha = m_title_alpha;
  if (m_paused) {
    alpha = 1.0;
    title_text = "Game Paused";
  }

  if (alpha > 0.001) {
    renderer.SetColor(0, 0, 0, int(alpha * 160));
    renderer.DrawQuad(0, GetStateHeight() / 3, GetStateWidth(), 80);
    const Color c = Renderer::ToColor(255, 255, 255, int(alpha * 0xFF));
    TextWriter title(GetStateWidth()/2, GetStateHeight()/3 + 25, renderer, true, 24);
    title << Text(title_text, c);

    // While we're at it, show the key legend
    renderer.SetColor(c);
    const Tga *keys = GetTexture(PlayKeys);
    renderer.DrawTga(keys, GetStateWidth() / 2 - 250, GetStateHeight() / 2);
  }

  int text_y = CalcKeyboardHeight() + 42;

  renderer.SetColor(White);
  renderer.DrawTga(GetTexture(PlayStatus),  Layout::ScreenMarginX - 1,   text_y);
  renderer.DrawTga(GetTexture(PlayStatus2), Layout::ScreenMarginX + 273, text_y);

  string multiplier_text = STRING(fixed << setprecision(1) << CalculateScoreMultiplier() <<
                                  " Multiplier");
  string speed_text = STRING(m_state.song_speed << "% Speed");

  TextWriter score(Layout::ScreenMarginX + 92, text_y + 5, renderer,
                   false, Layout::ScoreFontSize);
  score << static_cast<int>(m_state.stats.score);

  TextWriter multipliers(Layout::ScreenMarginX + 232, text_y + 12, renderer,
                         false, Layout::TitleFontSize+2);
  multipliers << Text(multiplier_text, Renderer::ToColor(138, 226, 52));

  int speed_x_offset = (m_state.song_speed >= 100 ? 0 : 11);
  TextWriter speed(Layout::ScreenMarginX + 412 + speed_x_offset, text_y + 12,
                   renderer, false, Layout::TitleFontSize+2);
  speed << Text(speed_text, Renderer::ToColor(114, 159, 207));

  double non_zero_playback_speed = ( (m_state.song_speed == 0) ? 0.1 : (m_state.song_speed/100.0) );
  microseconds_t tot_seconds = static_cast<microseconds_t>((m_state.midi->GetSongLengthInMicroseconds() /
                                                            100000.0) / non_zero_playback_speed);
  microseconds_t cur_seconds = static_cast<microseconds_t>((m_state.midi->GetSongPositionInMicroseconds() /
                                                            100000.0) / non_zero_playback_speed);

  if (cur_seconds < 0)
    cur_seconds = 0;

  if (cur_seconds > tot_seconds)
    cur_seconds = tot_seconds;

  int completion = static_cast<int>(m_state.midi->GetSongPercentageComplete() * 100.0);

  unsigned int tot_min = static_cast<unsigned int>((tot_seconds/10) / 60);
  unsigned int tot_sec = static_cast<unsigned int>((tot_seconds/10) % 60);
  unsigned int tot_ten = static_cast<unsigned int>( tot_seconds%10);
  const string total_time = STRING(tot_min << ":" << setfill('0') << setw(2) << tot_sec << "." << tot_ten);

  unsigned int cur_min = static_cast<unsigned int>((cur_seconds/10) / 60);
  unsigned int cur_sec = static_cast<unsigned int>((cur_seconds/10) % 60);
  unsigned int cur_ten = static_cast<unsigned int>( cur_seconds%10      );
  const string current_time = STRING(cur_min << ":" << setfill('0') << setw(2) << cur_sec << "." << cur_ten);
  const string percent_complete = STRING(" (" << completion << "%)");

  text_y += 30 + Layout::SmallFontSize;
  TextWriter time_text(Layout::ScreenMarginX + 39, text_y+2, renderer, false, Layout::SmallFontSize);
  time_text << STRING(current_time << " / " << total_time << percent_complete);

  // Draw a song progress bar along the top of the screen
  const int time_pb_width = static_cast<int>(m_state.midi->GetSongPercentageComplete() * (GetStateWidth() -
                                                                                          Layout::ScreenMarginX*2));
  const int pb_x = Layout::ScreenMarginX;
  const int pb_y = CalcKeyboardHeight() + 25;

  renderer.SetColor(0x50, 0x50, 0x50);
  renderer.DrawQuad(pb_x, pb_y, time_pb_width, 16);

  if (m_look_ahead_you_play_note_count > 0) {

    const double note_count = 1.0 * m_look_ahead_you_play_note_count;

    const int note_miss_pb_width = static_cast<int>(m_state.stats.notes_user_could_have_played /
                                                    note_count * (GetStateWidth() - Layout::ScreenMarginX*2));

    const int note_hit_pb_width = static_cast<int>(m_state.stats.notes_user_actually_played /
                                                   note_count * (GetStateWidth() - Layout::ScreenMarginX*2));

    renderer.SetColor(0xCE,0x5C,0x00);
    renderer.DrawQuad(pb_x, pb_y - 20, note_miss_pb_width, 16);

    renderer.SetColor(0xFC,0xAF,0x3E);
    renderer.DrawQuad(pb_x, pb_y - 20, note_hit_pb_width, 16);
  }

  // Show the combo
  if (m_current_combo > 5) {
    int combo_font_size = 20;
    combo_font_size += (m_current_combo / 10);

    int combo_x = GetStateWidth() / 2;
    int combo_y = GetStateHeight() - CalcKeyboardHeight() + 30 - (combo_font_size/2);

    TextWriter combo_text(combo_x, combo_y, renderer, true, combo_font_size);
    combo_text << STRING(m_current_combo << " Combo!");
  }
}

