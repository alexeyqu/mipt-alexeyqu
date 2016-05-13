#include <thread>
#include <iostream>
#include <vector>

#include "flag.h"

int main() {
    blocking_flag f;

    std::vector<std::thread> threads;

    int num_workers = 10;

    for(int i = 0; i < num_workers; i++)
    {
      threads.emplace_back([&f] {
            f.wait();
            std::cout << "ready!" << std::endl;
       } );
    }

    f.set();

    for(int i = 0; i < num_workers; i++)
    {
      if(threads[i].joinable())
      {
        threads[i].join();
      }
    }

    return 0;
}
