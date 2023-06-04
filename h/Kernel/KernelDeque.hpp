#ifndef _Kernel_Deque_hpp_
#define _Kernel_Deque_hpp_

#include "MemoryAllocator.hpp"
#include "../C++_API/syscall_cpp.hpp"

template<typename T>
class KernelDeque
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
    KernelDeque()
        :
        head(nullptr),
        tail(nullptr)
    {
    }

    KernelDeque(const KernelDeque<T>&) = delete;
    KernelDeque<T>& operator=(const KernelDeque<T>&) = delete;

    void addFirst(T data) volatile
    {
        auto newNode = static_cast<Node*>(MemoryAllocator::alloc(sizeof(Node)));
        new (newNode) Node(data, head);

        head = newNode;
        if (!tail) tail = head;
    }

    void addLast(T data) volatile
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

    T removeFirst() volatile
    {
        auto nodeToRemove = head;
        head = head->next;
        if (!head) tail = nullptr;

        auto ret = nodeToRemove->data;
        MemoryAllocator::free(nodeToRemove);
        return ret;
    }

    T peekFirst() const volatile
    {
        return head->data;
    }

    T removeLast() volatile
    {
        Node* prev = nullptr;
        for (auto cur = head; cur && cur != tail; cur = cur->next)
        {
            prev = cur;
        }

        auto nodeToRemove = tail;
        if (prev) prev->next = nullptr;
        else head = nullptr;
        tail = prev;

        auto ret = nodeToRemove->data;
        MemoryAllocator::free(nodeToRemove);
        return ret;
    }

    int remove(T value) volatile
    {
        Node* prev = nullptr;
        for (auto cur = head; cur; cur = cur->next)
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

    T peekLast() const volatile
    {
        return tail->data;
    }

    bool contains(T value) const volatile
    {
        for (auto cur = head; cur; cur = cur->next)
        {
            if(cur->data == value) return true;
        }

        return false;
    }

    bool isEmpty() const volatile { return head == nullptr; }
};

#endif // _Kernel_Deque_hpp_
