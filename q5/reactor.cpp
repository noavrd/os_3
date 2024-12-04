#include "reactor.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>

Reactor::Reactor() {
    kqueue_fd = kqueue();
    if (kqueue_fd < 0) {
        perror("kqueue");
        exit(1);
    }
}

Reactor::~Reactor() {
    close(kqueue_fd);
}

int Reactor::addFd(int fd, std::function<void(int)> callback) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);

    if (kevent(kqueue_fd, &ev, 1, nullptr, 0, nullptr) < 0) {
        perror("kevent");
        return -1;
    }

    callbacks[fd] = callback;
    return 0;
}

int Reactor::removeFd(int fd) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);

    if (kevent(kqueue_fd, &ev, 1, nullptr, 0, nullptr) < 0) {
        perror("kevent");
        return -1;
    }

    callbacks.erase(fd);
    return 0;
}

void Reactor::start() {
    eventLoop();
}

void Reactor::stop() {
    close(kqueue_fd);
}

void Reactor::eventLoop() {
    struct kevent events[10];

    while (true) {
        int n = kevent(kqueue_fd, nullptr, 0, events, 10, nullptr);
        if (n < 0) {
            perror("kevent");
            break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].ident;
            if (callbacks.find(fd) != callbacks.end()) {
                callbacks[fd](fd);
            }
        }
    }
}
