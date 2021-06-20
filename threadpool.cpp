/*
 * Copyright 2021 Amitesh Singh <singh.amitesh@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to deal in the Software without restriction, including without 
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "threadpool.h"

namespace nonstd
{
    void result::get()
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

    thread_pool::thread_pool(int n):MAX_THREADS(n)
    {
        if (MAX_THREADS <= 0) MAX_THREADS = 1;
    }

    void thread_pool::worker(int id)
    {
#ifdef DEBUG
        std::cout << "starting thread: " << std::this_thread::get_id() <<  "local id: " << id << std::endl;
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
               if (job->fobject->waitEnabled)
                 {
                    job->fobject->m.lock();
                 }
               job->f();
               job->fobject->workdone = true;
               if (job->fobject->waitEnabled)
                 {
                    job->fobject->m.unlock();
                    job->fobject->cond.notify_all();
                 }

               if (!job->fobject->waitEnabled)
                 delete job->fobject;
               delete job;
            }
        }
    }

    void thread_pool::start()
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

    void thread_pool::stop()
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
        delete x;

        while (!jobs.empty())
        {
            auto job = jobs.front();
            jobs.pop();
            delete job;
        }
    }

    void thread_pool::addJob(std::function<void()> f, result &fobject)
    {
        fobject.waitEnabled = true;
        fobject.workdone = false;
        std::unique_lock<std::mutex> l(m);
        jobs.push(new job_info(f, &fobject));
        cond.notify_one();
    }

    void thread_pool::addJob(std::function<void()> f)
    {
        result *dummyobj = new result();
        dummyobj->waitEnabled = false;
        std::unique_lock<std::mutex> l(m);
        jobs.push(new job_info(f, dummyobj));
        cond.notify_one();
    }
}
