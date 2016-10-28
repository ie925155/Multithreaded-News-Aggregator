#ifndef OSTREAMLOCK
#define OSTREAMLOCK

#include <iostream>
#include <mutex>

class Oslock
{
  private:
    static std::mutex m;
    bool lock;

  public:
    Oslock(bool l) : lock(l) {}
    friend std::ostream &operator<<(std::ostream &os, const Oslock &o);
};
static Oslock oslock(true);
static Oslock osunlock(false);

#endif