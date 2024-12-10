#include "reactor.hpp"
#include <unistd.h>
#include <sys/event.h>
#include <iostream>
#include <cstring>

// Initializes the Reactor and creates a kqueue instance
Reactor::Reactor() {
    kqueue_fd = kqueue();
    if (kqueue_fd < 0) {
        perror("kqueue");
        exit(1);
    }
}

// Destructor: Closes the kqueue to release resources
Reactor::~Reactor() {
    close(kqueue_fd);
}

// Adds a file descriptor to the kqueue for monitoring and connect it with a callback function
int Reactor::addFd(int fd, std::function<void(int)> callback) {
    struct kevent ev;
    // Monitor fd for read events
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);

    if (kevent(kqueue_fd, &ev, 1, nullptr, 0, nullptr) < 0) {
        perror("kevent");
        return -1;
    }
    // Store the callback associated with the fd
    callbacks[fd] = callback;
    return 0;
}

// Removes a file descriptor from the kqueue and its connected callback
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

// Starts the event loop to handle events
void Reactor::start() {
    // Begin monitoring and processing events
    eventLoop(); 
}


// Stops the Reactor by closing the kqueue file descriptor
void Reactor::stop() {
    // This will end the event loop as kqueue becomes invalid
    close(kqueue_fd); 
}

// Main event loop: Monitors file descriptors and invokes their callbacks on events
void Reactor::eventLoop() {
    // Array to store triggered events
    struct kevent events[10];
    while (true) {
        // Wait for events on the kqueue
        int n = kevent(kqueue_fd, nullptr, 0, events, 10, nullptr);
        if (n < 0) {
            perror("kevent");
            break;
        }

         // Process each triggered event
        for (int i = 0; i < n; ++i) {
            // Get the file descriptor associated with the event
            int fd = events[i].ident;
            if (callbacks.find(fd) != callbacks.end()) {                
                // Invoke the callback associated with the fd
                callbacks[fd](fd);
            }
        }
    }
}
