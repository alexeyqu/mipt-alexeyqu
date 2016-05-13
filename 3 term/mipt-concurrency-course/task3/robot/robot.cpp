#include "robot.h"

int main()
{
  robot r;

  std::thread t1(
      [&r]() {
        for(int i = 0; i < 1000; i++)
        {
          std::cout << 1 << std::endl;
          r.step(1);
        }
      }
  );

  std::thread t2(
      [&r]() {
        for(int i = 0; i < 1000; i++)
        {
          std::cout << 2 << std::endl;
          r.step(2);
        }
      }
  );

  t2.join();
  t1.join();

  return 0;
}
