#include "DpmsThread.h"

#include <iostream>
#include <SDL.h>

bool DpmsThread::expectedState() const
{
  return !(m_pause_saver || m_is_keyboard_active);
}

void DpmsThread::run()
{
  if (!SDL_IsScreenSaverEnabled())
    SDL_EnableScreenSaver();


  while (!m_should_exit)
  {
    // Required for wait_for
    std::unique_lock<std::mutex> cv_lock(m_should_exit_cv_m);

    // Sleep for 15 seconds or until something changes
    // std::condition_variable::wait_for unlocks mutex
    m_should_exit_cv.wait_for(cv_lock, std::chrono::seconds(15));

    // Handle exit (m_should_exit == true)
    if (m_should_exit)
      break;

    bool expected = expectedState();
    bool current = SDL_IsScreenSaverEnabled();

    // Reset value
    m_is_keyboard_active = false;

    if (expected == current)
      continue;

    if (expected)
      SDL_EnableScreenSaver();
    else
      SDL_DisableScreenSaver();
  }
}

DpmsThread::DpmsThread() :
  m_thread(&DpmsThread::run, this)
{
}

DpmsThread::~DpmsThread()
{
  {
    std::unique_lock<std::mutex> cv_lock(m_should_exit_cv_m);
    m_should_exit = true;
  }
  m_should_exit_cv.notify_all();
  m_thread.join();
}

void DpmsThread::handleKeyPress()
{
  {
    std::unique_lock<std::mutex> cv_lock(m_should_exit_cv_m);
    m_is_keyboard_active = true;
  }
  m_should_exit_cv.notify_all();
}

void DpmsThread::pauseScreensaver(bool paused)
{
  {
    std::unique_lock<std::mutex> cv_lock(m_should_exit_cv_m);
    m_pause_saver = paused;
  }
  m_should_exit_cv.notify_all();
}
