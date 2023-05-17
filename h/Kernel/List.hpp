#ifndef _List_hpp_
#define _List_hpp_

template<typename T>
class List
{
private:
    struct Node
    {
        T* data;
        Node* next;

        Node(T* data, Node* next) : data(data), next(next) {}
    };

    Node* head;
    Node* tail;

public:
    List()
        :
        head(nullptr),
        tail(nullptr)
    {
    }

    List(const List<T>&) = delete;
    List<T>& operator=(const List<T>&) = delete;

    void addFirst(T *data)
    {
        Node* newNode = new Node(data, head);
        head = newNode;
        if (!tail) tail = head;
    }

    void addLast(T *data)
    {
        Node* newNode = new Node(data, nullptr);
        if (tail)
        {
            tail->next = newNode;
            tail = newNode;
        }
        else head = tail = newNode;
    }

    T* removeFirst()
    {
        if (!head) return nullptr;

        Node* elem = head;
        head = head->next;
        if (!head) tail = nullptr;

        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekFirst()
    {
        if (!head) return nullptr;
        return head->data;
    }

    T* removeLast()
    {
        if (!head) return nullptr;

        Node* prev = nullptr;
        for (Node* cur = head; cur && cur != tail; cur = cur->next)
        {
            prev = cur;
        }

        Node* nodeToRemove = tail;
        if (prev) prev->next = nullptr;
        else head = nullptr;
        tail = prev;

        T* ret = nodeToRemove->data;
        delete nodeToRemove;
        return ret;
    }

    T* remove(T* value)
    {
        if (!head) return nullptr;

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

            return cur->data;
        }

        return nullptr;
    }

    T* peekLast()
    {
        if (!tail) return nullptr;
        return tail->data;
    }

    bool contains(T* value)
    {
        for (Node* cur = head; cur; cur = cur->next)
        {
            if(cur->data == value) return true;
        }

        return false;
    }

    bool isEmpty() { return head == nullptr; }
};

#endif //_List_hpp_
