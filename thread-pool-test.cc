/**
 * File: thread-pool-test.cc
 * -------------------------
 * Simple test in place to verify that the ThreadPool class
 * works.
 */

#include <string>
#include <iostream>
#include <functional>
#include "thread-pool.h"
//#include "thread-utils.h"
#include "ostreamlock.h"
//#include "semaphore.h"
using namespace std;

static const size_t kNumThreads = 12;
static const size_t kNumFunctions = 1000;
int main(int argc, char *argv[])
{
  ThreadPool pool(kNumThreads);
  for (size_t id = 0; id < kNumFunctions; id++)
  {
    pool.schedule([id] {
      cout << oslock << "Thread (ID: " << id << ") has started." << endl
           << osunlock;
      size_t sleepTime = (id % 3) * 10;
     this_thread::sleep_for(chrono::milliseconds(sleepTime));
      cout << oslock << "Thread (ID: " << id << ") has finished." << endl
           << osunlock;
    });
  }
  pool.wait();
  cout << "All done!" << endl;
  return 0;
}
