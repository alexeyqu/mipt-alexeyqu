#include "spsc_ring_buffer.h"
#include <stdint.h>
#include <future>

const size_t N = 10000000;

uint64_t producer_work_loop(spsc_ring_buffer<int> &my_buffer)
{
  uint64_t sum = 0;

  for(size_t i = 0; i < N; i++)
  {
    sum += i;
    while(!my_buffer.enqueue(i))
    {
      std::this_thread::yield();
    }
  }

  return sum;
}

uint64_t consumer_work_loop(spsc_ring_buffer<int> &my_buffer)
{
  uint64_t sum = 0;
  int curr_num = 0;

  for(size_t i = 0; i < N; i++)
  {
    while(!my_buffer.dequeue(curr_num))
    {
      std::this_thread::yield();
    }

    sum += curr_num;
  }

  return sum;
}

int main()
{
  std::cin.get();

  spsc_ring_buffer<int> my_buffer(1024);

  std::future<uint64_t> produced_sum = std::async(std::launch::async, producer_work_loop, std::ref(my_buffer));

  std::future<uint64_t> consumed_sum = std::async(std::launch::async, consumer_work_loop, std::ref(my_buffer));

  produced_sum.wait();
  consumed_sum.wait();

  std::cout << produced_sum.get() << std::endl;
  std::cout << consumed_sum.get() << std::endl;

  return 0;
}
