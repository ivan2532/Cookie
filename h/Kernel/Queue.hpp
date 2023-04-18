#ifndef __KernelQueue__
#define __KernelQueue__

#include "../../lib/hw.h"

template <typename T>
class Queue
{
public:
    ~Queue()
    {
        Node* nextIter = queueList;

        while (nextIter != nullptr)
        {
            Node* nodeToDelete = nextIter;
            nextIter = nextIter->next;
            delete nodeToDelete;
        }
    }

    uint32 size () { return sizeCounter; }

    int enqueue (T newElement)
    {
        Node* newNode = new Node;
        newNode->data = newElement;

        if(newNode == nullptr)
        {
            return -1;
        }

        if(lastElement != nullptr)
        {
            lastElement->next = newNode;
        }

        lastElement = newNode;

        if(queueList == nullptr)
        {
            queueList = newNode;
        }

        sizeCounter++;
        return 0;
    }

    T dequeue ()
    {
        T returnValue = queueList->data;
        Node* nodeToDelete = queueList;
        queueList = queueList->next;

        delete nodeToDelete;

        sizeCounter--;
        return returnValue;
    }

private:
    struct Node
    {
        T data;
        Node* next = nullptr;
    };

    Node* queueList = nullptr;
    Node* lastElement = nullptr;
    uint32 sizeCounter = 0;
};


#endif //__KernelQueue__
