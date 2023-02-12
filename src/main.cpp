#include <iostream>
#include <future>
#include "ring_buffer.h"

using RingBuffer15d = concurrent::RingBuffer<double, 15>;

using RingBuffer15p= concurrent::RingBuffer<std::unique_ptr<int>, 15>;

void producer_d(int id, RingBuffer15d& buffer) {
    for (int i = 0; i < 20; ++i) {
        buffer.push(i);
        std::cout << "Producer " << id << " produced " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


void producer_p(int id, RingBuffer15p& buffer) {
    for (int i = 0; i < 20; ++i) {
        buffer.push(std::make_unique<int>(i));
        std::cout << "Producer " << id << " produced " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    RingBuffer15d rbd;
    auto ab = std::async(std::launch::async, producer_d, 0, std::ref(rbd));

    rbd.push(1.1);
    for (int i = 0; i < 20; i++) {
        std::cout << "Size is " << rbd.size() << std::endl;
        std::cout << "Consumer consumed " << rbd.pop() << std::endl;
    }
    ab.wait();

    RingBuffer15p rbp;
    auto ap = std::async(std::launch::async, producer_p, 1, std::ref(rbp));

    rbp.push(std::make_unique<int>(1.1));
    for (int i = 0; i < 20; i++) {
        std::cout << "Size is " << rbp.size() << std::endl;
        std::cout << "Consumer consumed " << *rbp.pop() << std::endl;
    }
    ap.wait();

    return 0;
}
