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
private:
  std::array<std::atomic<bool>, 2> want;
  std::atomic<int> victim;

public:
   peterson_mutex()
   {
     want[0].store(false);
     want[1].store(false);
     victim.store(0);
   }

   void lock(int t)
   {
    //  std::clog << "locking " << t << '\n';

     want[t].store(true);
     victim.store(t);
     while (want[1 - t].load() && victim.load() == t) {
       std::this_thread::yield();
     }

    //  std::clog << "locked " << t << '\n';
   }

   void unlock(int t)
   {
    //  std::clog << "unlocking " << t << '\n';

     want[t].store(false);

    //  std::clog << "unlocked " << t << '\n';
   }
};

class tree_mutex
{
private:
  std::size_t num_threads;
  std::vector<peterson_mutex> tournament_tree;

public:
  tree_mutex(std::size_t _num_threads) : tournament_tree(pow(2, ceil(log2(_num_threads))) - 1)
  {
    assert(_num_threads > 0);

    num_threads = _num_threads;
  }

  void lock(std::size_t thread_index);
  void unlock(std::size_t thread_index);
};

void tree_mutex::lock(std::size_t thread_index)
{
  assert(thread_index >= 0 && thread_index < num_threads);

  std::atomic<std::size_t> index(thread_index);

  for(std::atomic<std::size_t> i(tournament_tree.size() + index.load()); i.exchange((i.load() - 1)/2) > 0; )
  {
    tournament_tree[i.load()].lock(index.load() % 2);
    index.store(index.load() / 2);
  }
}

void tree_mutex::unlock(std::size_t thread_index)
{
  assert(thread_index >= 0 && thread_index < num_threads);

  std::atomic<std::size_t> left_bound(0), right_bound(tournament_tree.size() + 1);

  for(std::atomic<std::size_t> i(0); i.load() < tournament_tree.size(); )
  {
    std::atomic<std::size_t> which_child(0);

    if((left_bound.load() + right_bound.load()) / 2 > thread_index)
    {
      which_child.store(LEFT);
    }
    else
    {
      which_child.store(RIGHT);
    }

    tournament_tree[i.load()].unlock(which_child.load());

    i.store(2 * i.load() + which_child.load() + 1);

    if(which_child.load() == RIGHT)
    {
      left_bound.store((left_bound.load() + right_bound.load()) / 2);
    }
    else
    {
      right_bound.store((left_bound.load() + right_bound.load()) / 2);
    }
  }
}
