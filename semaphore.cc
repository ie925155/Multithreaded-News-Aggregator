#include "semaphore.h"

using namespace std;

semaphore::semaphore(int value) : value(value) {}

void semaphore::wait() {
  lock_guard<mutex> lg(m);
  cv.wait(m, [this]{ return value > 0; });
  value--;
}

void semaphore::signal() {
  lock_guard<mutex> lg(m);
  value++;
  if (value == 1) cv.notify_all();
}
