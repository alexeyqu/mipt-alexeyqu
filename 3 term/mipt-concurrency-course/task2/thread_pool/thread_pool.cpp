#include "thread_pool.h"

void test_thread_pool(size_t num_workers)
{
  thread_pool<std::thread::id> pool(num_workers);

  std::vector<std::future<std::thread::id>> futures;

  for(int i = 0; i < 100; i++)
  {
    futures.push_back(pool.submit([i]() { return std::this_thread::get_id(); }));
  }

  for(int i = 0; i < 100; i++)
  {
    std::cout << "Res: " << futures[i].get() << std::endl;
  }

  pool.shutdown();
}

int main()
{
  test_thread_pool(1);

  return 0;
}
