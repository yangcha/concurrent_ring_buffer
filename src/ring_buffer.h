#ifndef _CONCURRENT_RING_BUFFER_H_
#define _CONCURRENT_RING_BUFFER_H_

#include <mutex>
#include <condition_variable>

namespace concurrent {
    /*
    * Thread-safe ring buffer/circular buffer.
    * It will override the old entry if reach the capacity. If empty, then wait for the next entry to push in. 
    * N is 2^n - 1 for better performance  
    */
    template<typename T, int N>
    class RingBuffer {
    public:

        size_t capacity() const { return N; }

        void push(const T& item) {
            std::unique_lock<std::mutex> lck(mtx_);
            buffer_[head_] = item;
            increment_(head_);
            if (empty_()) {
                increment_(tail_);
            }
            // Manual unlocking is done before notifying, to avoid waking up
            // the waiting thread only to block again (see notify_one for details)
            lck.unlock();
            if_empty_.notify_one();
        }

        void push(T&& item) {
            std::unique_lock<std::mutex> lck(mtx_);
            buffer_[head_] = std::move(item);
            increment_(head_);
            if (empty_()) {
                increment_(tail_);
            }
            lck.unlock();
            if_empty_.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lck(mtx_);
            if_empty_.wait(lck, [this] { return !empty_(); });
            auto old_tail = tail_;
            increment_(tail_);
            return std::move(buffer_[old_tail]);
        }

        size_t size() {
            std::unique_lock<std::mutex> lck(mtx_);
            return (head_ - tail_ + BUFSIZE_) % BUFSIZE_;
        }

        bool empty() {
            std::unique_lock<std::mutex> lck(mtx_);
            return empty_();
        }

        void clear() {
            std::unique_lock<std::mutex> lck(mtx_);
            tail_ = head_ = 0U;
        }

    private:

        static constexpr size_t BUFSIZE_ = N + 1U;

        bool empty_() const { return head_ == tail_; }

        void increment_(size_t& value) { value = (value + 1) % BUFSIZE_; }

        T buffer_[BUFSIZE_];
        size_t head_{ 0U };
        size_t tail_{ 0U };

        std::condition_variable if_empty_;
        std::mutex mtx_;
    };
}

#endif
