/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include <iostream>

#include "thread-pool.h"
#include "ostreamlock.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads)
    : stop(false)
{
    for (size_t i = 0; i < numThreads; ++i)
        workers.emplace_back([this] {
            for (;;)
            {
                function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m);
                    //wait task to come in
                    this->task_condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                        return;
                    task = move(this->tasks.front());
                    this->tasks.pop();
                }
                // do the task
                task();
            }
        });
}

void ThreadPool::schedule(const function<void(void)> &thunk)
{
    cout << oslock << __func__ << endl
         << osunlock;
    unique_lock<mutex> lock(this->m);
    tasks.emplace(thunk);
    task_condition.notify_one();
}

void ThreadPool::wait()
{
    cout << oslock << __func__ << endl
         << osunlock;
    unique_lock<mutex> lock(this->m);
    this->stop = true;
    lock.unlock();
    task_condition.notify_all();
    for (auto &worker : workers)
        worker.join();
}
