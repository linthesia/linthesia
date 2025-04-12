// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include <iostream>
#include "FrameAverage.h"

using namespace std;

/* averages frame times over N frames
 * @param[in] n  the number of frames to average
 */
FrameAverage::FrameAverage(int n) :
  sum(0),
  durations(n),
  pos(0)
{

}

/**
 * reports a frame time 
 * 
 * @param[in] delta_us frame time in microseconds
 */
void FrameAverage::Frame(long delta_us) {

  if (delta_us < 0)
    return;

  sum += delta_us;
  sum -= durations[pos];
  durations[pos] = delta_us;
  pos = (pos + 1) % durations.size();
}


unsigned long FrameAverage::GetAverage()
{
  return sum / durations.size();
}
