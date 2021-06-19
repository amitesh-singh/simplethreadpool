#ifndef _THREAD_POOL_H__
#define _THREAD_POOL_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <queue>
#include <iostream>
#include <atomic>

namespace nonstd
{
   class result
     {
        std::mutex m;
        std::condition_variable cond;
        bool waitEnabled = true;
        std::atomic<bool> workdone = false;

       public:
        void get();

        friend class thread_pool;
     };

   class thread_pool
     {
        struct job_info
          {
             std::function<void()> f;
             result *fobject;
             job_info(std::function<void()> &_f, result *_fobject): f(_f), fobject(_fobject) {}
          };

        std::mutex m;
        std::condition_variable cond;
        std::vector<std::thread *> thread_list;
        std::queue<job_info *> jobs;
        bool terminate = false;
        int MAX_THREADS;

       public:
        thread_pool(int n = 1);

        void worker(int id);

        void start();

        void stop();

        void addJob(std::function<void()> f, result &fobject);

        void addJob(std::function<void()> f);
     };
}
#endif
