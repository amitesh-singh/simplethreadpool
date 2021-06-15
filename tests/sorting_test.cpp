#include <iostream>
#include "threadpool.h"
#include <chrono>
#include <algorithm>
#include <ctime>


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
   std::srand(std::time(nullptr));
   nonstd::thread_pool pool(4);
   pool.start();

   nonstd::result res;

   std::vector<int> v;
   for (int i = 0; i < 10000000; ++i)
      v.push_back(std::rand() % 15001);

   pool.addJob([&v]()->void { std::sort(begin(v), std::end(v)); }, res);

   res.get();

   for (auto &x: v)
     {
        std::cout << x << "->";
     }
   std::cout << std::endl;

   pool.stop();

   return 0;
}
