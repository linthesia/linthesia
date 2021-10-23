#ifndef __DPMS_THREAD_H
#define __DPMS_THREAD_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class DpmsThread
{
public:
  DpmsThread();

  ~DpmsThread();

  void handleKeyPress();

  /**
   * sets if the screensaver should be paused
   * @param[in] paused, if true the screensaver is paused
   */ 
  void pauseScreensaver(bool paused);

private:
  bool                    m_is_keyboard_active = false;
  bool                    m_pause_saver = false;
  bool                    m_should_exit = false;
  std::condition_variable m_should_exit_cv;
  std::mutex              m_should_exit_cv_m;

  std::thread m_thread;

  bool expectedState() const;
  void run();
};

#endif // __DPMS_THREAD_H
