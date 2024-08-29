#include "thread_pool.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <functional>

const int TASK_CAPACITY = 1024;

ThreadPool::ThreadPool()
    : initThreadNums_(4), taskNums_(0), taskCapacity_(TASK_CAPACITY), poolMode_(PoolMode::MODE_FIXED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode)
{
    poolMode_ = mode;
}

void ThreadPool::setTaskCapacity(size_t capacity)
{
    taskCapacity_ = capacity;
}

void ThreadPool::submitTask(std::shared_ptr<Task> task)
{
    // get mutex
    std::unique_lock<std::mutex> lck(taskQueueMtx_);
    // communication between threads
    if (!notFull_.wait_for(lck, std::chrono::seconds(1), [&]() -> bool {
        return taskQueue_.size() < static_cast<size_t>(taskCapacity_);
    })) {
        std::cerr << "Task queue is full, submit task failed!\n";
        return;
    }
    // if there is free space, put task into queue
    taskQueue_.emplace(task);
    taskNums_++;
    
    // There must be some tasks in queue, notify threads to execute them
    notEmpty_.notify_all();
}

void ThreadPool::start(int initThreadNums)
{
    initThreadNums_ = initThreadNums;

    for (int i = 0; i < initThreadNums_; i++) {
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        threads_.emplace_back(std::move(ptr));
    }

    for (int i = 0; i < initThreadNums_; i++) {
        threads_[i]->start();
    }
}

void ThreadPool::threadFunc()
{
    std::cout << "Begin threadFunc tid: " << std::this_thread::get_id() << std::endl;

    for (;;) {
        std::shared_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lck(taskQueueMtx_);
            notEmpty_.wait(lck, [&]() -> bool {
                return taskQueue_.size() > 0;
            });

            task = taskQueue_.front();
            taskQueue_.pop();
            taskNums_--;

            if (taskQueue_.size() > 0) {
                notEmpty_.notify_all();
            }

            notFull_.notify_all();
        }

        if (task != nullptr) {
            task->run();
        }

    }

    std::cout << "End threadFunc tid: " << std::this_thread::get_id() << std::endl;
}

Thread::Thread(threadFunc func)
    : func_(func) {}

Thread::~Thread() {}

void Thread::start()
{
    std::thread t(func_);
    t.detach();

    std::this_thread::sleep_for(std::chrono::seconds(5));
}