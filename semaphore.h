#ifndef _semaphore__
#define _semaphore__

#include <mutex>
#include <condition_variable>

class semaphore {
  public:
    semaphore(int value = 0);
    void wait();
    void signal();
  private:
    int value;
    std::mutex m;
    std::condition_variable_any cv;
    semaphore(const semaphore& orig) = delete;
    const semaphore& operator=(const semaphore& rhs) const = delete;
};

#endif
