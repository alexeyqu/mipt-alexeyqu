#include "barrier.h"

void test_barrier(size_t num_workers)
{
  barrier my_barrier(num_workers);

  std::vector<std::thread> threads;

  for(int i = 0; i < num_workers; i++)
  {
    threads.emplace_back([&my_barrier] {
      for(int i = 0; i < 1000; i++)
      {
        my_barrier.enter();
        my_barrier.enter();
      }
     } );
  }

  for(int i = 0; i < num_workers; i++)
  {
    if(threads[i].joinable())
    {
      threads[i].join();
    }
  }
}

int main()
{
  test_barrier(5);

  return 0;
}
