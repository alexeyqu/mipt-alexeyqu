#include "striped_hash_set.h"
#include <thread>

const int MAX_TEST = 50000;

int main()
{
  std::cin.get();

  striped_hash_set<int, std::hash<int>> my_hash_set(1);

  std::thread([&my_hash_set]()
    {
      for (int i = 0; i < MAX_TEST; ++i)
      {
        std::cerr << i << std::endl;

        my_hash_set.contains(i);

        my_hash_set.add(i);

        if(!my_hash_set.contains(i)) throw;

        my_hash_set.remove(i);

        if(my_hash_set.contains(i)) throw;

        my_hash_set.add(i);

        if(!my_hash_set.contains(i)) throw;
      }
    }
  ).join();

  return 0;
}
