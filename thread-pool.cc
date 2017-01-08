/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include <iostream>
#include <thread>

#include "thread-pool.h"
#include "ostreamlock.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads)
    : workers(numThreads),
    sem_wait_task(new semaphore(0)),
    sem_worker_res(new semaphore(numThreads)),
    sem_wait(new semaphore(0)),
    num_of_task(0)
{
  for (auto &worker : workers){
      worker.is_working = false;
      worker.is_active = false;
      worker.sem_wait_task.reset(new semaphore(0));
      worker.thunk = NULL;
  }
  thread t([this]() -> void { this->dispatcher(); });
  t.detach();
}
void ThreadPool::schedule(const function<void(void)> &thunk)
{
  unique_lock<mutex> lock(this->m);
  tasks.emplace(thunk);
  num_of_task++;
  sem_wait_task->signal();
}

void ThreadPool::wait()
{
  sem_wait->wait();
}

void ThreadPool::dispatcher(void)
{
  while(true){
    sem_wait_task->wait();
    sem_worker_res->wait();
    worker_mutex.lock();
    size_t worker_len = workers.size();
    size_t idx;
    for (size_t i = 0; i < worker_len; i++) {
      idx = i;
      if(!workers[i].is_working){
        workers[i].is_working = true; //cout << oslock << "notify worker" << endl << osunlock;
        workers[i].thunk = tasks.front();
        tasks.pop();
        workers[i].sem_wait_task->signal();
        if(!workers[i].is_active){
          //cout << oslock << "new a active worker " <<  &worker << endl << osunlock;
          workers[i].is_active = true;
          thread t([this](size_t idx) -> void { do_task(idx); }, idx);
          t.detach();
        }
        break;
      }
    } // end of for loop
    worker_mutex.unlock();
  } // end of while loop
}

void ThreadPool::do_task(const size_t index)
{
  while(true){
    task_mutex.lock();
    unique_ptr<semaphore>& sem_copy = workers[index].sem_wait_task;
    task_mutex.unlock();
    sem_copy->wait();
    workers[index].thunk();
    workers[index].is_working = false;
    sem_worker_res->signal();
    num_of_task--;
    if(num_of_task == 0){
      sem_wait->signal();
    }
  }
}
