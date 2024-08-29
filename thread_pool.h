#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <iostream>
#include <chrono>

enum class PoolMode {
    MODE_FIXED,
    MODE_CACHED
};


// abstract class for users to define their own function
class Task
{
public:
    virtual void run() = 0;
};

class Thread
{
public:
    using threadFunc = std::function<void()>;

    Thread(threadFunc);

    ~Thread();

    void start();
private:
    threadFunc func_;
};

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    void setMode(PoolMode mode);
    void start(int initThreadNums = 4);
    void setTaskCapacity(size_t capacity);
    void submitTask(std::shared_ptr<Task> task);
private:
    void threadFunc();
private:
    std::vector<std::unique_ptr<Thread>> threads_;
    size_t initThreadNums_;

    std::queue<std::shared_ptr<Task>> taskQueue_;
    std::atomic_uint taskNums_;
    size_t taskCapacity_;
    std::mutex taskQueueMtx_;

    std::condition_variable notFull_;
    std::condition_variable notEmpty_;

    PoolMode poolMode_;
};