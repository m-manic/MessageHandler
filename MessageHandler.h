#pragma once
#include <array>

/*
* Defining number of messages in the message pool.
*/
constexpr int NUM_OF_MESSAGES = 10;

/*
* Defining the size of data array in message.
*/
constexpr int DATA_SIZE = 255;

/*
* Message structure.
*/
typedef struct {
    uint8_t len;
    std::array<uint8_t, DATA_SIZE> data = {};
} message_t;

/*
* Function to get new message from the message pool.
* If there are available messages return pointer to the first message that is available.
* If there are no available messages return nullptr.
* Provided message will be in status unavailable.
*/
message_t* NewMessage();

/*
* Function to delete the message provided as parameter.
* Provided message will be in status available.
* Message can be obtained again by calling the NewMessage() function.
*/
void DeleteMessage(message_t* mess);

/*
* Function to send message to thread by providing thread id.
* Provided message will be in status pending.
* Message can be retreived by the destination thread.
* If sending is successful return 0, othervise return -1.
*/
int SendMessage(uint8_t destination, message_t* mess);

/*
* Function to receive message by providing thread id.
* If receiving is successful return 0, otherwise return -1.
*/
int ReceiveMessage(uint8_t id, message_t** mess);

/*
* Get status of all messages in the message pool.
*/
void GetMessagesStatus();