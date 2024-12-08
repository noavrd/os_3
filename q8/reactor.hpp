#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <map>
#include <functional>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <vector>

class Reactor {
    int kqueue_fd;
    std::map<int, std::function<void(int)>> callbacks;
    std::unordered_map<std::thread::id, std::thread> clientThreads;
    std::mutex threadsMutex;

    void eventLoop();

public:
    Reactor();
    ~Reactor();

    int addFd(int fd, std::function<void(int)> callback);
    int removeFd(int fd);
    void start();
    void stop();

    // Proactor additions
    pthread_t startProactor(int fd, std::function<void(int)> threadFunc);
    int stopProactor(pthread_t tid);
    void stopAllProactorThreads();
};

#endif
