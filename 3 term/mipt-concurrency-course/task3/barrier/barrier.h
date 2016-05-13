#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>

class barrier
{
public:
  explicit barrier(size_t num_threads_) : num_threads(num_threads_)
  {
    cur_waiting.store(0);
    epoch.store(0);
  }

  void enter()
  {
    std::unique_lock<std::mutex> locker(mutex);

    size_t cur_epoch = epoch.load();

    if(++cur_waiting == num_threads)
    {
      epoch++;
      cur_waiting.store(0);
      all_here.notify_all();
    }
    else
    {
      all_here.wait(locker, [this, cur_epoch]() { return (cur_epoch != this->epoch.load()); } );
    }
  }

private:
  size_t num_threads;
  std::atomic<size_t> epoch;
  std::atomic<size_t> cur_waiting;
  std::mutex mutex;
  std::condition_variable all_here;
};
