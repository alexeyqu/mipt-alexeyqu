#include "tree_mutex.h"

int main()
{
  tree_mutex mtx(3);

  mtx.lock(0);
  // critical section
  mtx.unlock(0);

  return 0;
}
