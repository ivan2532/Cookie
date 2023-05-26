#ifndef _syscall_cpp_
#define _syscall_cpp_

#include "../C_API/syscall_c.hpp"

void* operator new (size_t size);
void* operator new[] (size_t size);
void* operator new (size_t size, void* ptr);

void operator delete (void* ptr);
void operator delete[] (void* ptr);

class Thread
{
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();
    int start();
    void join();
    static void dispatch();
    static int sleep(time_t);

protected:
    Thread();
    virtual void run() { }
    thread_t getMyHandle() { return myHandle; }

private:
    static void runWrapper(void* args);

    thread_t myHandle;
    void (*body)(void*);
    void* arg;
};

class Semaphore
{
public:
    explicit Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();

private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate ();

protected:
    explicit PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    [[noreturn]] void run() override;

private:
    time_t period;
};

#endif
