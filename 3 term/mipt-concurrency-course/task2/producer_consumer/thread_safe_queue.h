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
    is_alive = true;
  }

  ~thread_safe_queue() {};

  void enqueue(Value new_value)
  {
    std::unique_lock<std::mutex> lock_mutex(my_mutex);

    if(!is_alive)
    {
      throw std::logic_error("The queue is dead already");
    }

    while(my_container.size() >= my_capacity)
    {
      queue_not_full.wait(lock_mutex);
    }

    my_container.push_back(std::move(new_value));

    queue_not_empty.notify_one();
  }

  void pop(Value& popped_value)
  {
    std::unique_lock<std::mutex> lock_mutex(my_mutex);

    if(!is_alive && my_container.empty())
    {
      throw std::logic_error("The queue is dead already");
    }

    while(my_container.empty())
    {
      queue_not_empty.wait(lock_mutex);
    }

    popped_value = std::move(my_container.front());

    my_container.pop_front();

    queue_not_full.notify_one();
  }

  void shutdown()
  {
    is_alive = false;
  }

private:
  Container my_container;
  std::mutex my_mutex;
  size_t my_capacity;
  bool is_alive;
  std::condition_variable queue_not_empty;
  std::condition_variable queue_not_full;
};
