#include "thread_pool.h"

class MyTask: public Task
{
public:
    void run()
    {
        std::cout << "Begin threadFunc tid: " << std::this_thread::get_id() << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::cout << "End threadFunc tid: " << std::this_thread::get_id() << std::endl;
    }
};

int main()
{
    ThreadPool pool;
    pool.start(4);

    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());

    std::cin.get();

    return 0;
}