#include "reactor.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/event.h>

Reactor::Reactor() {
    kqueue_fd = kqueue();
    if (kqueue_fd < 0) {
        perror("kqueue");
        exit(1);
    }
}

Reactor::~Reactor() {
    stopAllProactorThreads();
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

pthread_t Reactor::startProactor(int fd, std::function<void(int)> threadFunc) {
    std::thread clientThread([=]() {
        threadFunc(fd);
        close(fd);
    });

    pthread_t tid = clientThread.native_handle();
    {
        std::lock_guard<std::mutex> lock(threadsMutex);
        clientThreads[clientThread.get_id()] = std::move(clientThread);
    }
    return tid;
}

int Reactor::stopProactor(pthread_t tid) {
    std::lock_guard<std::mutex> lock(threadsMutex);
    for (auto it = clientThreads.begin(); it != clientThreads.end(); ++it) {
        if (pthread_equal(tid, it->second.native_handle())) {
            if (it->second.joinable()) {
                it->second.detach();
            }
            clientThreads.erase(it);
            return 0;
        }
    }
    return -1;  // Thread not found
}

void Reactor::stopAllProactorThreads() {
    std::lock_guard<std::mutex> lock(threadsMutex);
    for (auto& [id, thread] : clientThreads) {
        if (thread.joinable()) {
            thread.detach();
        }
    }
    clientThreads.clear();
}
