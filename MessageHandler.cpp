#include "MessageHandler.h"
#include <iostream>
#include <mutex>
#include <algorithm>

/*
* Defining message status flag - AVAILABLE.
*/
constexpr int AVAILABLE = 0;

/*
* Defining message status flag - UNAVAILABLE.
*/
constexpr int UNAVAILABLE = -1;

/*
* Defining enums for return values.
*/
enum class RETURN {
    NOK = -1,
    OK = 0
};

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
    std::lock_guard<std::mutex> lck (mux);
    auto element = std::find_if(std::begin(messages), std::end(messages),
                                [] (const std::pair<message_t, int> &el) {return (el.second == AVAILABLE); } );
    if (element != std::end(messages)) {
        element->second = UNAVAILABLE;
        return &element->first;
    }
    else {
        std::cout << "There are no free messages in the message pool." << std::endl;
        return nullptr;
    }
}

void DeleteMessage(message_t* mess) {
    std::lock_guard<std::mutex> lck (mux);
    if (mess != nullptr) {
        auto element = std::find_if(std::begin(messages), std::end(messages),
                                    [&mess] (const std::pair<message_t, int> &el) {return (&el.first == mess); } );
        if (element != std::end(messages)) {
            element->second = AVAILABLE;
            element->first.len = 0;
            std::fill(std::begin(element->first.data), std::end(element->first.data), 0);
        }
    }
    else {
        std::cout << "Tried to delete invalid message." << std::endl;
    }
}

int SendMessage(uint8_t destination, message_t* mess) {
    std::lock_guard<std::mutex> lck (mux);
    auto element = std::find_if(std::begin(messages), std::end(messages),
                                [&mess] (const std::pair<message_t, int> &el) {return (&el.first == mess); } );
    if (element != std::end(messages)) {
        element->second = destination;
        return static_cast<int>(RETURN::OK);
    }
    else {
        return static_cast<int>(RETURN::NOK);
    }
}

int ReceiveMessage(uint8_t id, message_t** mess) {
    std::lock_guard<std::mutex> lck (mux);
    auto element = std::find_if(std::begin(messages), std::end(messages),
                                [&id] (const std::pair<message_t, int> &el) {return (el.second == id); } );
    if (element != std::end(messages)) {
        *mess = &element->first;
        return static_cast<int>(RETURN::OK);
    }
    else {
        return static_cast<int>(RETURN::NOK);
    }
}

void GetMessagesStatus() {
    int available = 0;
    int unavailable = 0;
    int pending = 0;
    std::lock_guard<std::mutex> lck (mux);
    for (auto &el : messages) {
        if (el.second == AVAILABLE) {
            ++available;
        }
        else if (el.second == UNAVAILABLE) {
            ++unavailable;
        }
        else if (el.second > 0) {
            ++pending;
        }
    }
    std::cout << "Number of available messages: " << available << std::endl;
    std::cout << "Number of unavailable messages: " << unavailable << std::endl;
    std::cout << "Number of pending messages: " << pending << std::endl;
}