#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

class barrier
{
public:
  explicit barrier(size_t num_threads_) : num_threads(num_threads_)
  {
    cur_waiting = 0;
    epoch = 0;
  }

  void enter()
  {
    std::unique_lock<std::mutex> locker(mutex);

    size_t cur_epoch = epoch;

    if(++cur_waiting == num_threads)
    {
      epoch++;
      cur_waiting = 0;
      all_here.notify_all();
    }
    else
    {
      all_here.wait(locker, [this, cur_epoch]() { return (cur_epoch != this->epoch); } );
    }
  }

private:
  size_t num_threads, epoch, cur_waiting;
  std::mutex mutex;
  std::condition_variable all_here;
};
