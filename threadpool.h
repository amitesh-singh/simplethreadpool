#ifndef _THREAD_POOL_H__
#define _THREAD_POOL_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <queue>
#include <iostream>

namespace nonstd
{
class result
{
  public:
  std::mutex m;
  std::condition_variable cond;
  bool waitEnabled = true;
  bool workdone = false;
  void get()
  {
    if (waitEnabled)
    {
    //blocking call
      //only wait when work is not done.
      if (!workdone)
      {
        std::unique_lock<std::mutex> l(m);
        cond.wait(l, [this]()->bool {return workdone;});
      }
    }

  }
};

template<int MAX_THREADS>
class thread_pool
{
   struct job_info
   {
     std::function<void()> f;
     result *fobject;
     job_info(std::function<void()> &_f, result *_fobject): f(_f), fobject(_fobject)
     {

     }
   };

   std::mutex m;
   std::condition_variable cond;
   std::vector<std::thread *> thread_list;
   std::queue<job_info *> jobs;
   bool terminate = false;
   
  public:
   thread_pool() {}

   void worker(int id)
     {
        #ifdef DEBUG
        std::cout << "starting thread: " << std::this_thread::get_id() << std::endl;       
        #endif
        while (1)
          {
             job_info *job;
               {
                  std::unique_lock<std::mutex> l(m);
                  cond.wait(l, [this]()->bool { return !jobs.empty() || terminate; });
                  if (!terminate)
                  {
                    job = jobs.front();
                    jobs.pop();
                  }
                  else break;
               }
            if (!terminate)
            {
              job->fobject->workdone = false;
                if (job->fobject->waitEnabled)
                {
                  job->fobject->m.lock();
                }
                job->f();
                if (job->fobject->waitEnabled)
                {
                job->fobject->m.unlock();
                job->fobject->cond.notify_one();
                }
                job->fobject->workdone = true;
                if (!job->fobject->waitEnabled)
                  delete job->fobject;
                delete job;
            }

          }
     }
   void start()
     {
        int n = MAX_THREADS;
        #ifdef DEBUG
        std::cout << "available threads are: " << n << std::endl;
        #endif
        for (int i = 0; i < n; ++i)
          {
             thread_list.push_back(new std::thread(&thread_pool::worker, this, i));
          }
     }

   void stop()
     {
       m.lock();
       terminate = true;
       m.unlock();
       cond.notify_all();

       //join all threads
       int n = MAX_THREADS;
       for (int i = 0; i < n; ++i)
          {
             thread_list[i]->join();
          }

       for (auto &x: thread_list)
         {
            delete x;
         }

       while (!jobs.empty())
         {
            auto job = jobs.front();
            jobs.pop();
            delete job;
         }
     }

   void addJob(std::function<void()> f, result &fobject)
     {
        fobject.waitEnabled = true;
        fobject.workdone = false;
        std::unique_lock<std::mutex> l(m);
        jobs.push(new job_info(f, &fobject));
        cond.notify_one();
     }

   void addJob(std::function<void()> f)
   {
       result *dummyobj = new result();
       dummyobj->waitEnabled = false;
       std::unique_lock<std::mutex> l(m);
        jobs.push(new job_info(f, dummyobj));
        cond.notify_one();
   }
};
}

#endif
