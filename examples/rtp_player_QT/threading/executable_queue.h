//@file executable_queue.h

#ifndef EXECUTABLE_QUEUE_H
#define EXECUTABLE_QUEUE_H

#include <queue>
#include <functional>
#include <mutex>

#ifndef NAMESPDEF_H
#include "../namespdef.h"
#endif


NAMESP_BEGIN

typedef std::function<void()> exec_object_t;

class ExecutableQueue
{
public:
    ExecutableQueue()
    {
    }

    void post(exec_object_t& f)
    {
        std::lock_guard<std::mutex> g(_mutex);
        _exec_queue.push(f);
    }

    void post(exec_object_t&& f)
    {
        std::lock_guard<std::mutex> g(_mutex);
        _exec_queue.push(f);
    }

    void exec()
    {
        exec_object_t func = nullptr;
        _mutex.lock();
        if(_exec_queue.size() > 0)
        {
            func = _exec_queue.front();
            _exec_queue.pop();
        }
        _mutex.unlock();

        if(func != nullptr)
            func();
    }

private:
    std::queue<exec_object_t> _exec_queue;
    std::mutex _mutex;
};

NAMESP_END
#endif // EXECUTABLE_QUEUE_H
