#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "MessageHandler.h"

/*
* Thread function for Message Handler testing.
* Request message from the message pool.
* If message is available, write some data to it and send by calling SendMessage function.
* Once there are no more available messages, stop requesting new ones and start receiveng messages.
* If there are messages, in the message pool, to be received by current thread, print the data in the message and delete the message so it can be used again.
* If there are no new messages to be received for NUM_OF_MESSAGES cycles, terminate thread.
*/
void* MessagingThread(void *arg)
{
    int id = *(int*) arg;
    int status = 1;
    bool exitFlag = false;
    while (exitFlag == false) {
        std::cout << "******" << id << "******" << std::endl;
        GetMessagesStatus();
        message_t* message;
        if (status > 0) {
            message = NewMessage();
        }

        if (message != nullptr && status > 0) {
            std::fill(std::begin(message->data), std::begin(message->data) + id, id);
            message->len = id;
            if (SendMessage((rand() % 3 + 1), message) == 0) {
                std::cout << "Id: " << id << "| Message sending succeed." << std::endl;
            }
            else {
                std::cout << "Id: " << id << "| Message sending failed." << std::endl;
            }
        }
        else {
            if (ReceiveMessage(id, &message) == 0) {
                status = 0;
                std::cout << "Id: " << id << "| Message receiving succeed. Id: " << id << ", length: " << static_cast<unsigned int>(message->len) << ", data: ";
                for (auto el = 0; el < message->len; ++el) {
                    std::cout << static_cast<unsigned int>(message->data.at(el));
                }
                std::cout << std::endl;
                DeleteMessage(message);
            }
            else {
                 --status;
                if (status == -NUM_OF_MESSAGES) {
                    exitFlag = true;
                }
                std::cout << "Id: " << id << "| Message receiving failed." << std::endl;
            }
        }
        std::cout << "======" << id << "======" << std::endl;
        sleep(1);
    }
    pthread_exit(nullptr);
}

int main() {
    int threadIdOne = 1;
    int threadIdTwo = 2;
    int threadIdThree = 3;
    pthread_t threadOne;
    pthread_t threadTwo;
    pthread_t threadThree;

    pthread_create(&threadOne, nullptr, &MessagingThread, (void*) &threadIdOne);
    pthread_create(&threadTwo, nullptr, &MessagingThread, (void*) &threadIdTwo);
    pthread_create(&threadThree, nullptr, &MessagingThread, (void*) &threadIdThree);

    pthread_join(threadOne, nullptr);
    pthread_join(threadTwo, nullptr);
    pthread_join(threadThree, nullptr);

    return 0;
}

