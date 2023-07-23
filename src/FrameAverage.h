// -*- mode: c++; coding: utf-8 -*-

// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __FRAME_AVERAGE_H
#define __FRAME_AVERAGE_H

#include <vector>

using namespace std;

class FrameAverage {
public:

  /* averages frame times over N frames
   * @param[in] n  the number of frames to average
   */
  FrameAverage(int n);

  /**
   * reports a frame time to the smoother
   * 
   * @param[in] delta_us frame time in microseconds
   */
  void Frame(unsigned long delta_us);

  /**
   * returns the average time of the last frames
   * in microseconds
   */ 
  unsigned long GetAverage();

private:
  unsigned long sum;
  vector<unsigned long> durations;
  int pos;
};

#endif // __FRAME_AVERAGE_H
