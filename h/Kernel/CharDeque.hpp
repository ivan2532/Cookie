#ifndef _Char_Deque_hpp_
#define _Char_Deque_hpp_

#include "MemoryAllocator.hpp"
#include "../C++_API/syscall_cpp.hpp"

class CharDeque
{
public:
    struct Node
    {
        char data;
        Node* next;

        Node(char data, Node* next) : data(data), next(next) {}
    };

    Node* head;
    Node* tail;
    CharDeque();

    CharDeque(const CharDeque&) = delete;
    CharDeque& operator=(const CharDeque&) = delete;

    void addFirst(char data);
    void addLast(char data) volatile;

    char removeFirst() volatile;
    char removeLast();
    int remove(char value);

    char peekFirst() const;
    char peekLast() const;

    bool contains(char value) const;
    bool isEmpty() const;
};

#endif // _Char_Deque_hpp_
