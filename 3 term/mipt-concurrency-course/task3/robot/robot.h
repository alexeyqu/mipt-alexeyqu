#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>

class robot
{
  std::condition_variable turn;
  std::mutex mutex;
  std::atomic<bool> start;

public:
  explicit robot() { start.store(true); };

  void step(int id);
};

void robot::step(int id)
{
  std::unique_lock<std::mutex> locker(mutex);

  while()

  std::cout << std::this_thread::get_id() << ' '  << id << std::endl;

  turn.notify_one();
}
