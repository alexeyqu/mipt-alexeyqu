#include "thread_safe_queue.h"

const int nOfTasks = 10;
const int maxNumber = 100;
const int nOfConsumers = 2;
/*
bool primeTester(int number)
{
  for(int divisor = 2; divisor <= number/2 + 1; divisor++)
  {
    if(number % divisor == 0) return false;
  }

  return true;
}

void producer(ParallelQueue<int> &taskQueue)
{
  std::random_device randomDevice;
  std::mt19937_64 primesGenerator(randomDevice());
  std::uniform_int_distribution<int> distribution(1, maxNumber);

  for(int i = 0; i < nOfTasks; i++)
  {
    int newNumber = random() % maxNumber;//distribution(primesGenerator);
    // std::cout << "Thread " << std::this_thread::get_id() << " created " << newNumber << '\n';
    taskQueue.push(newNumber);
  }

  taskQueue.push(taskQueue.getPoison());
}

void consumer(ParallelQueue<int> &taskQueue)
{
  int currentNumber;

  while(taskQueue.pop(currentNumber))
  {
    std::cout << std::this_thread::get_id() << '\t' << currentNumber << " is " << std::flush;
    std::cout << ((primeTester(currentNumber)) ? "prime!\n" : "NOT prime..\n") << std::flush;
  }
}
*/
int main()
{
  thread_safe_queue<int> task_queue(10);

  // std::thread producerThread = std::thread(producer, std::ref(task_queue));
  // std::vector<std::thread> consumerThreads;
  //
  // for(int index = 0; index < nOfConsumers; index++)
  // {
  //   consumerThreads.push_back(std::thread(consumer, std::ref(taskQueue)));
  // }
  //
  // for(int index = 0; index < nOfConsumers; index++)
  // {
  //   consumerThreads[index].join();
  // }
  //
  // producerThread.join();

  return 0;
}
