#include <iostream>
#include <future>
#include "ring_buffer.h"

using RingBuffer15d = concurrent::RingBuffer<double, 15>;

void producer(int id, RingBuffer15d& buffer) {
    for (int i = 0; i < 20; ++i) {
        buffer.push(i);
        std::cout << "Producer " << id << " produced " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    RingBuffer15d rg;
    auto a = std::async(std::launch::async, producer, 0, std::ref(rg));

    rg.push(1.1);
    for (int i = 0; i < 20; i++) {
        std::cout << "Size is " << rg.size() << std::endl;
        std::cout << "Consumer consumed " << rg.pop() << std::endl;
    }
    a.wait();
    return 0;
}
