#include "ostreamlock.h"
#include <mutex>
std::mutex Oslock::m;
std::ostream &operator<<(std::ostream &os, const Oslock &o)
{
    (o.lock) ? Oslock::m.lock() : Oslock::m.unlock();
    return os;
}
