#ifndef _Kernel_List_hpp_
#define _Kernel_List_hpp_

#include "kernel_allocator.h"
#include "../C++_API/syscall_cpp.hpp"

template<typename T>
class KernelList
{
public:
    struct Node
    {
        T data;
        Node* next;

        Node(T data, Node* next) : data(data), next(next) {}
    };

    Node* head;
    Node* tail;
    KernelList()
        :
        head(nullptr),
        tail(nullptr)
    {
    }

    KernelList(const KernelList<T>&) = delete;
    KernelList<T>& operator=(const KernelList<T>&) = delete;

    void addFirst(T data)
    {
        auto newNode = static_cast<Node*>(kernel_alloc(sizeof(Node)));
        new (newNode) Node(data, head);

        head = newNode;
        if (!tail) tail = head;
    }

    void addLast(T data)
    {
        auto newNode = static_cast<Node*>(kernel_alloc(sizeof(Node)));
        new (newNode) Node(data, nullptr);

        if (tail)
        {
            tail->next = newNode;
            tail = newNode;
        }
        else head = tail = newNode;
    }

    T removeFirst()
    {
        Node* nodeToRemove = head;
        head = head->next;
        if (!head) tail = nullptr;

        auto ret = nodeToRemove->data;
        kernel_free(nodeToRemove);
        return ret;
    }

    T peekFirst()
    {
        return head->data;
    }

    T removeLast()
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
        kernel_free(nodeToRemove);
        return ret;
    }

    int remove(T value)
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

            kernel_free(cur);
            return 0;
        }

        return -1;
    }

    T peekLast()
    {
        return tail->data;
    }

    bool contains(T value)
    {
        for (Node* cur = head; cur; cur = cur->next)
        {
            if(cur->data == value) return true;
        }

        return false;
    }

    bool isEmpty() { return head == nullptr; }
};

#endif // _Kernel_List_hpp_
