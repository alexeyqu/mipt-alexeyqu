#include <iostream>
#include <array>
#include <atomic>
#include <thread>
#include <vector>
#include <assert.h>
#include <math.h>

class peterson_mutex
{
public:
    peterson_mutex()
    {
        want[0].store(false, std::memory_order_relaxed); // ну просто потому, что это конструктор мьютекса и пока что конкретный порядок операций не играет роли
        want[1].store(false, std::memory_order_relaxed);
        victim.store(0, std::memory_order_relaxed);
    }

    void lock(int t)
    {
        want[t].store(true, std::memory_order_relaxed); // учитывая release после этого, можно по-максимуму расслабиться
        victim.store(t, std::memory_order_release); // логический барьер задачи -- порядок выставления victim \
        // мы не хотим, чтобы процессор поместил операцию отсюда между вызовами, относящимися к while

        while (want[1 - t].load(std::memory_order_acquire) && // по той же причине, мы храним эти операции с памятью ниже while
          victim.load(std::memory_order_relaxed) == t) // с acquire до victim.load мы можем забить на упорядочивания
        {
            std::this_thread::yield();
        }
     }

    void unlock(int t)
    {
        want[t].store(false, std::memory_order_release); // потому что мы не хотим, \
          чтобы какие-то операции над разделяемой памятью осуществлялись после unlock (выхода из CS)
    }

 private:
    std::array<std::atomic<bool>, 2> want;
    std::atomic<int> victim;
};

int main()
{
    peterson_mutex mutex;

    std::vector<std::thread> threads;

    int sum = 0;

    for(int index = 0; index < 2; index++)
    {
      threads.emplace_back([&mutex, &sum, &index] {
            for (int i = 0; i < 2; i++)
            {
                mutex.lock(index);
                // sum++;
                std::cerr << std::this_thread::get_id() << ' ' << ++sum << std::endl;
                mutex.unlock(index);
            }
       } );
    }

    for(int i = 0; i < 2; i++)
    {
      threads[i].join();
    }

    return 0;
}
