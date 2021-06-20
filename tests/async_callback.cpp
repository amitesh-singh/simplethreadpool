#include <iostream>
#include "threadpool.h"
#include <chrono>
#include <algorithm>
#include <ctime>

void doWork()
{
   std::this_thread::sleep_for(std::chrono::seconds(5));
   std::cout << __FUNCTION__ << std::endl;
}

class job
{
  public:
      void run(int index)
        {
           std::this_thread::sleep_for(std::chrono::seconds(5));
           std::cout << __FUNCTION__ << std::endl;
        }
};

void notify_callback(nonstd::result &res, std::vector<int> &v)
{
   res.get();
   //print
   for (auto &x: v)
     {
        std::cout << x << "->";
     }
   std::cout << std::endl;
   std::cout << "print job done\n";
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

   pool.addJob(doWork); //don't wait
   pool.addJob([&v]()->void {
                                std::sort(begin(v), std::end(v));
                                std::cout << "sorting is finished\n";
                            },
                            res);
   nonstd::result finalres;
   pool.addJob(std::bind(notify_callback, std::ref(res), std::ref(v)), finalres);

   {
      job j;
      pool.addJob(std::bind(&job::run, std::ref(j), 1));
   }

   //wait for the final job
   finalres.get();

   std::cout << std::endl;

   pool.stop();

   return 0;
}

