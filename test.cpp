#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void print_id(int id) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return ready; });
    std::cout << "Thread ID: " << id << std::endl;
}

void set_ready() {
    std::unique_lock<std::mutex> lock(mtx);
    ready = true;
    cv.notify_all();
}

int main() {
    std::thread t1(print_id, 1);
    std::thread t2(print_id, 2);
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    set_ready();
    
    t1.join();
    t2.join();
    return 0;
}