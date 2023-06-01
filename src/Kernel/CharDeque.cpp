#include "../../h/Kernel/CharDeque.hpp"

CharDeque::CharDeque()
        :
        head(nullptr),
        tail(nullptr)
{
}

char CharDeque::removeFirst()
{
    auto nodeToRemove = head;
    head = head->next;
    if (!head) tail = nullptr;

    auto ret = nodeToRemove->data;
    MemoryAllocator::free(nodeToRemove);
    return ret;
}

void CharDeque::addFirst(char data)
{
    auto newNode = static_cast<Node*>(MemoryAllocator::alloc(sizeof(Node)));
    new (newNode) Node(data, head);

    head = newNode;
    if (!tail) tail = head;
}

void CharDeque::addLast(char data)
{
    auto newNode = static_cast<Node*>(MemoryAllocator::alloc(sizeof(Node)));
    new (newNode) Node(data, nullptr);

    if (tail)
    {
        tail->next = newNode;
        tail = newNode;
    }
    else head = tail = newNode;
}

char CharDeque::peekFirst() const
{
    return head->data;
}

char CharDeque::removeLast()
{
    Node* prev = nullptr;
    for (Node* cur = head; cur && cur != tail; cur = cur->next)
    {
        prev = cur;
    }

    Node* nodeToRemove = tail;
    if (prev) prev->next = nullptr;
    else head = nullptr;
    tail = prev;

    auto ret = nodeToRemove->data;
    MemoryAllocator::free(nodeToRemove);
    return ret;
}

int CharDeque::remove(char value)
{
    Node* prev = nullptr;
    for (Node* cur = head; cur; cur = cur->next)
    {
        if(cur->data != value)
        {
            prev = cur;
            continue;
        }

        if(prev) prev->next = cur->next;

        if(cur == head && head == tail)
        {
            head = nullptr;
            tail = nullptr;
        }
        else if(cur == head) head = cur->next;
        else if(cur == tail) tail = prev;

        MemoryAllocator::free(cur);
        return 0;
    }

    return -1;
}

char CharDeque::peekLast() const
{
    return tail->data;
}

bool CharDeque::contains(char value) const
{
    for (Node* cur = head; cur; cur = cur->next)
    {
        if(cur->data == value) return true;
    }

    return false;
}

bool CharDeque::isEmpty() const
{
    return head == nullptr;
}
