#include <queue>
#include <mutex>
#include <random>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <vector>
#include <condition_variable>
#include <future>
#include <memory>

// THREAD_SAFE_QUEUE

template <class Value>
class thread_safe_queue
{
public:
  explicit thread_safe_queue()
  {
      is_alive = true;
  }

  ~thread_safe_queue() {};

  void enqueue(Value new_value)
  {
    std::unique_lock<std::mutex> locker(mutex);

    if(!is_alive)
    {
      throw std::logic_error("The queue is dead already");
    }

    queue.push(std::move(new_value));

    queue_not_empty.notify_one();
  }

  bool pop(Value& popped_value)
  {
    std::unique_lock<std::mutex> locker(mutex);

    while(queue.empty())
    {
      if(!is_alive)
      {
          popped_value = Value();

          return false;
      }

      queue_not_empty.wait(locker);
    }

    popped_value = std::move(queue.front());

    queue.pop();

    return true;
  }

  void shutdown()
  {
    std::unique_lock<std::mutex> locker(mutex);

    is_alive = false;

    queue_not_empty.notify_all();
  }

private:
  std::queue<Value> queue;
  std::mutex mutex;
  bool is_alive;
  std::condition_variable queue_not_empty;
};

// THREAD_POOL

template <class Value>
class thread_pool
{
public:
  explicit thread_pool(size_t num_workers)
  {
    my_num_workers = num_workers;

    for(size_t index = 0; index < my_num_workers; index++)
    {
      workers.emplace_back(std::thread([this]() { this->run_worker(); }));
    }
  }

  ~thread_pool()
  {
    shutdown();
  }

  std::future<Value> submit(std::function<Value()> function)
  {
    std::unique_lock<std::mutex> lock_mutex(my_shutdown_mutex);

    std::packaged_task<Value()> task(function);
    std::future<Value> future = task.get_future();

    task_queue.enqueue(std::move(task));

    return future;
  }

  void shutdown()
  {
    std::unique_lock<std::mutex> lock_mutex(my_shutdown_mutex);

    task_queue.shutdown();

    // run_worker();

    for(size_t index = 0; index < my_num_workers; index++)
    {
      if(workers[index].joinable())
      {
        workers[index].join();
      }
    }
  }

private:
  void run_worker()
  {
    while(1)
    {
      std::packaged_task<Value()> task;

      if(!task_queue.pop(task)) return;

      task();
    }
  }

  std::mutex my_shutdown_mutex;

  size_t my_num_workers;
  std::vector<std::thread> workers;
  thread_safe_queue<std::packaged_task<Value()>> task_queue;
};
