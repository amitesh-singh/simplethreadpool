#include <iostream>
#include "threadpool.h"
#include <chrono>

#define F_INIT std::cout << std::this_thread::get_id() << ": started " << __PRETTY_FUNCTION__ << std::endl
#define F_END std::cout << std::this_thread::get_id() << ": ended " << __PRETTY_FUNCTION__ << std::endl

void doWork()
{
   F_INIT;
   std::this_thread::sleep_for(std::chrono::seconds(5));
   F_END;
}

void setNum(int &x)
{
   x++;
}

int main()
{
   nonstd::thread_pool<14> pool;
   pool.start();

   nonstd::result res[10];

   for (int i = 0; i < 10; ++i)
     pool.addJob(doWork, res[i]);

   int x = 10;
   nonstd::result res2;
   //waiting for one of thread.
   pool.addJob(std::bind(setNum, std::ref(x)), res2);
   res2.get();
   std::cout << "x--. " << x << std::endl;
   pool.addJob(doWork);
   pool.addJob(std::bind(setNum, std::ref(x)));

   while (1)
     {
        std::cout << "main loop" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "x---> " << x << std::endl;
     }

   pool.stop();

   return 0;
}
