#include <queue>
#include <mutex>
#include <random>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <condition_variable>

template <class Value, class Container = std::deque<Value>>
class thread_safe_queue
{
public:
  explicit thread_safe_queue(size_t capacity)
  {
    my_capacity = capacity;
    is_alive.store(true);
  }

  ~thread_safe_queue() {};

  void enqueue(Value new_value)
  {
    if(!is_alive.load())
    {
      throw std::logic_error("The queue is dead already");
    }

    std::unique_lock<std::mutex> lock_mutex(my_mutex);

    while(my_container.size() >= my_capacity)
    {
      queue_not_full.wait(lock_mutex);
    }

    my_container.push_back(std::move(new_value));

    queue_not_empty.notify_one();
  }

  void pop(Value& popped_value)
  {
    if(!is_alive.load() && my_container.empty())
    {
      throw std::logic_error("The queue is dead already");
    }

    std::unique_lock<std::mutex> lock_mutex(my_mutex);

    while(my_container.empty())
    {
      queue_not_empty.wait(lock_mutex);
    }

    popped_value = Value();

    popped_value = std::move(my_container.front());

    my_container.pop_front();

    queue_not_full.notify_one();
  }

  void shutdown()
  {
    is_alive.store(false);
  }

private:
  Container my_container;
  std::mutex my_mutex;
  size_t my_capacity;
  std::atomic<bool> is_alive;
  std::condition_variable queue_not_empty;
  std::condition_variable queue_not_full;
};
