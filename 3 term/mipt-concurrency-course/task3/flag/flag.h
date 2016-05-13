#include <mutex>
#include <condition_variable>
#include <atomic>

class blocking_flag {
public:
    blocking_flag()
        : ready_(false)
    {}

    void wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        while (!ready_.load()) {
            ready_cond_.wait(lock);
        }
    }

    void set() {
        ready_.store(true);
        ready_cond_.notify_all();
    }

private:
    std::atomic<bool> ready_;
    std::mutex mtx_;
    std::condition_variable ready_cond_;
};
