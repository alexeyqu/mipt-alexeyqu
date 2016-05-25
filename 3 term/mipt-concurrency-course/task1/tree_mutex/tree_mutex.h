#include <iostream>
#include <array>
#include <atomic>
#include <thread>
#include <vector>
#include <assert.h>
#include <math.h>

const int LEFT = 0;
const int RIGHT = 1;

class peterson_mutex
{
public:
    peterson_mutex()
    {
        want[0].store(false);
        want[1].store(false);
        victim.store(0);
    }

    void lock(int t)
    {
        want[t].store(true);
        victim.store(t);
        while (want[1 - t].load() && victim.load() == t)
        {
            std::this_thread::yield();
        }
     }

    void unlock(int t)
    {
        want[t].store(false);
    }

 private:
    std::array<std::atomic<bool>, 2> want;
    std::atomic<int> victim;
};

class tree_mutex
{
public:
    tree_mutex(std::size_t _num_threads) : tournament_tree(pow(2, ceil(log2(_num_threads))) - 1)
    {
        assert(_num_threads > 0);

        num_threads = _num_threads;
    }

    void lock(std::size_t thread_index)
    {
        assert(thread_index >= 0 && thread_index < num_threads);

        size_t index = thread_index;

        for(size_t i = tournament_tree.size() + index; i > 0; )
        {
            i = (i - 1)/2;
            tournament_tree[i].lock(index % 2);
            index /= 2;
        }
    }

    void unlock(std::size_t thread_index)
    {
        assert(thread_index >= 0 && thread_index < num_threads);

        size_t left_bound = 0, right_bound = tournament_tree.size() + 1;

        for(size_t i = 0; i < tournament_tree.size(); )
        {
            size_t which_child = 0;

            if((left_bound + right_bound) / 2 > thread_index)
            {
                which_child = LEFT;
            }
            else
            {
                which_child = RIGHT;
            }

            tournament_tree[i].unlock(which_child);

            i = (2 * i + which_child + 1);

            if(which_child == RIGHT)
            {
                left_bound = (left_bound + right_bound) / 2;
            }
            else
            {
                right_bound = (left_bound + right_bound) / 2;
            }
        }
    }

    private:
        std::size_t num_threads;
        std::vector<peterson_mutex> tournament_tree;
};
