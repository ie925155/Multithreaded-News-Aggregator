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
ThreadPool::~ThreadPool()
{
  cout << oslock << "sheldon destructor called.." << endl << osunlock;
  for (auto &worker : workers){
    cout << oslock << "sheldon worker is_active " << worker.is_active << endl << osunlock;
    if(worker.is_active)
      worker.sem_wait_task->signal();
  }
  if(sem_wait_task == NULL){
    cout << oslock << "sheldon sem_wait_task is null " << endl << osunlock;
    return;
  }
  sem_wait_task->signal();
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
    cout << oslock << "sheldon num_of_task " << num_of_task << endl << osunlock;
    if(num_of_task == 0){
      cout << oslock << "sheldon breakkkkkkkkkkkkkkkkkkk" << endl << osunlock;
      break;
    }
    sem_worker_res->wait();
    worker_mutex.lock();
    for (auto &worker : workers){
      if(!worker.is_working){
        worker.is_working = true;
        //cout << oslock << "notify worker" << endl << osunlock;
        worker.thunk = tasks.front();
        tasks.pop();
        worker.sem_wait_task->signal();
        if(!worker.is_active){
          //cout << oslock << "new a active worker " <<  &worker << endl << osunlock;
          worker.is_active = true;
          thread t([this, &worker]() -> void { do_task(worker); });
          t.detach();
        }
        break;
      }
    } // end of for loop
    worker_mutex.unlock();
  } // end of while loop
  cout << oslock << "sheldon leave dispatcher while loop" << endl << osunlock;
}

void ThreadPool::do_task(worker_t &worker)
{
  while(true){
    worker.sem_wait_task->wait();
    if(num_of_task == 0) break;
    //cout << oslock << "worker " << &worker << endl << osunlock;
    worker.thunk();
    worker.is_working = false;
    sem_worker_res->signal(); //cout << oslock << "avaliable worker " << &worker << endl << osunlock; num_of_task--; cout << oslock << "sheldon num_of_task " << num_of_task << endl << osunlock;
    num_of_task--;
    if(num_of_task == 0){
      cout << oslock << "sheldon signal task done " << num_of_task << endl << osunlock;
      sem_wait->signal();
    }
  };
  cout << oslock << "sheldon leave do_task while loop" << endl << osunlock;
}
