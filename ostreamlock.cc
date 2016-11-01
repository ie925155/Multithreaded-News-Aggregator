#include "ostreamlock.h"
#include <mutex>
std::mutex Oslock::m;
std::ostream &operator<<(std::ostream &os, const Oslock &o)
{
    if (o.lock)
        Oslock::m.lock();
    else
        Oslock::m.unlock();
    return os;
}
