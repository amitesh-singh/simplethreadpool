#include <iostream>
#include "threadpool.h"
#include <chrono>

#define F_INIT std::cout << std::this_thread::get_id() << ": started " << __PRETTY_FUNCTION__ << std::endl
#define F_END std::cout << std::this_thread::get_id() << ": ended " << __PRETTY_FUNCTION__ << std::endl


int main()
{
   nonstd::thread_pool<14> pool;
   pool.start();

   pool.stop();

   return 0;
}
