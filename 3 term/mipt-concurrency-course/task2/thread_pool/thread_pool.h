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
private:
  std::queue<Value> queue;
  std::mutex mutex;
  std::atomic<bool> is_alive;
  std::condition_variable queue_not_empty;

public:
  explicit thread_safe_queue();
  ~thread_safe_queue() {};

  void enqueue(Value new_value);
  bool pop(Value& popped_value);
  void shutdown();
};

template <class Value> thread_safe_queue<Value>::thread_safe_queue()
{
  is_alive.store(true);
}

template <class Value> void thread_safe_queue<Value>::enqueue(Value new_value)
{
  std::unique_lock<std::mutex> locker(mutex);

  if(!is_alive.load())
  {
    throw std::logic_error("The queue is dead already");
  }

  queue.push(std::move(new_value));

  queue_not_empty.notify_one();
}

template <class Value> bool thread_safe_queue<Value>::pop(Value& popped_value)
{
  std::unique_lock<std::mutex> locker(mutex);

  while(queue.empty())
  {
    if(!is_alive.load())
    {
        popped_value = Value();

        return false;
    }

    queue_not_empty.wait(locker);
  }

  popped_value = Value();

  popped_value = std::move(queue.front());

  queue.pop();

  return true;
}

template <class Value> void thread_safe_queue<Value>::shutdown()
{
  std::unique_lock<std::mutex> locker(mutex);

  is_alive.store(false);

  queue_not_empty.notify_all();
}

// THREAD_POOL

template <class Value>
class thread_pool
{
private:
  size_t my_num_workers;
  std::atomic<bool> is_alive;

  std::mutex my_shutdown_mutex;
  std::condition_variable pool_is_empty;

  std::vector<std::thread> workers;
  thread_safe_queue<std::packaged_task<Value()>> task_queue;

  void run_worker();

public:
  explicit thread_pool(size_t num_workers);
  ~thread_pool();

  std::future<Value> submit(std::function<Value()>);

  void shutdown();
};

template <class Value> thread_pool<Value>::thread_pool(size_t num_workers)
{
  my_num_workers = num_workers;

  is_alive.store(true);

  for(size_t index = 0; index < my_num_workers; index++)
  {
    workers.emplace_back(std::thread([this]() { this->run_worker(); }));
  }
}

template <class Value> thread_pool<Value>::~thread_pool()
{
  shutdown();
}

template <class Value> void thread_pool<Value>::run_worker()
{
  while(1)
  {
    std::packaged_task<Value()> task;

    if(!task_queue.pop(task))
    {
      pool_is_empty.notify_one();

      return;
    }

    task();
  }
}

template <class Value>  std::future<Value> thread_pool<Value>::submit(std::function<Value()> function)
{
  std::packaged_task<Value()> task(function);
  std::future<Value> future = task.get_future();

  task_queue.enqueue(std::move(task));

  return future;
}

template <class Value> void thread_pool<Value>::shutdown()
{
  std::unique_lock<std::mutex> lock_mutex(my_shutdown_mutex);

  task_queue.shutdown();

  is_alive.store(false);

  pool_is_empty.wait(lock_mutex, [this]() { return !(this->is_alive.load()); } );

  for(size_t index = 0; index < my_num_workers; index++)
  {
    if(workers[index].joinable())
    {
      workers[index].join();
    }
  }
}
