#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <map>       
#include <functional>

class Reactor {
    int kqueue_fd;
    std::map<int, std::function<void(int)>> callbacks;

    void eventLoop();

public:
    Reactor();
    ~Reactor();

    int addFd(int fd, std::function<void(int)> callback);
    int removeFd(int fd);
    void start();
    void stop();
};

#endif
