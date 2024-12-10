#include "reactor.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/event.h>

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
    stopAllProactorThreads();
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
        for (int i = 0; i < n; i++) {
            // Get the file descriptor associated with the event
            int fd = events[i].ident;
            if (callbacks.find(fd) != callbacks.end()) {
                // Invoke the callback associated with the fd
                callbacks[fd](fd);
            }
        }
    }
}

// Starts a new proactor thread to handle specific functionality for a file descriptor
pthread_t Reactor::startProactor(int fd, std::function<void(int)> threadFunc) {
     // Create a new thread for the given function
    std::thread clientThread([=]() {
        // Execute the function provided for this fd & Close the file descriptor when done
        threadFunc(fd);
        close(fd);
    });

    // Get the thread's native handle
    pthread_t tid = clientThread.native_handle();
    {
        // Protect thread map with mutex
        std::lock_guard<std::mutex> lock(threadsMutex);
        // Store thread
        clientThreads[clientThread.get_id()] = std::move(clientThread);
    }
    // Return the thread ID
    return tid;
}

// Stops a proactor thread by its ID
int Reactor::stopProactor(pthread_t tid) {
    // Protect thread map with mutex.
    std::lock_guard<std::mutex> lock(threadsMutex);
    for (auto it = clientThreads.begin(); it != clientThreads.end(); ++it) {
        // Match thread by ID
        if (pthread_equal(tid, it->second.native_handle())) {
            if (it->second.joinable()) {
                // Detach thread to clean up resources
                it->second.detach();
            }
            //Remove thread from map
            clientThreads.erase(it);
            // Success
            return 0;
        }
    }
    return -1;  // Thread not found
}

// Stops all active proactor threads and clears the thread map
void Reactor::stopAllProactorThreads() {
     // Protect thread map with mutex
    std::lock_guard<std::mutex> lock(threadsMutex);
     // Go over all threads
    for (auto& [id, thread] : clientThreads) {
        if (thread.joinable()) {
            // Detach each thread
            thread.detach();
        }
    }
    // Clear the thread map
    clientThreads.clear();
}
