#include "MessageHandler.h"
#include <iostream>
#include <mutex>

/*
* Message pool represented as array of pairs.
* Pairs contain message and message status.
* Message status: (0) - available, (-1) - unavailable, (>0) pending.
*/
std::array<std::pair<message_t, int>, NUM_OF_MESSAGES> messages = {};

/*
* Mutex for security.
* Only one thread can read/write at the same time.
*/
std::mutex mux;

message_t* NewMessage() {
    mux.try_lock();
    for (auto &el : messages) {
        if (el.second == 0) {
            el.second = -1;
            return &el.first;
        }
    }
    std::cout << "There are no free messages in the message pool." << std::endl;
    mux.unlock();
    return nullptr;
}

void DeleteMessage(message_t* mess) {
    mux.try_lock();
    for (auto &el : messages) {
        if (&el.first == mess) {
            el.second = 0;
            el.first.len = 0;
            std::fill(std::begin(el.first.data), std::end(el.first.data), 0);
        }
    }
    mux.unlock();
}

int SendMessage( unsigned int destination, message_t* mess) {
    mux.try_lock();
    for (auto &el : messages) {
        if (&el.first == mess) {
            el.second = destination;
            mux.unlock();
            return 0;
        }
    }
    mux.unlock();
    return -1;
}

int ReceiveMessage(unsigned int id, message_t** mess) {
    mux.try_lock();
    for (auto &el : messages) {
        if (el.second == id) {
            *mess =&el.first;
            mux.unlock();
            return 0;
        }
    }
    mux.unlock();
    return -1;
}

void GetMessagesStatus() {
    int available = 0;
    int unavailable = 0;
    int pending = 0;
    mux.try_lock();
    for (auto &el : messages) {
        if (el.second == 0) {
            ++available;
        }
        else if (el.second < 0) {
            ++unavailable;
        }
        else if (el.second > 0) {
            ++pending;
        }
    }
    mux.unlock();
    std::cout << "Number of available messages: " << available << std::endl;
    std::cout << "Number of unavailable messages: " << unavailable << std::endl;
    std::cout << "Number of pending messages: " << pending << std::endl;
}