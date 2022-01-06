#ifndef OCFBNJ_PIXEL_ENGINE_TASK_QUEUE_H
#define OCFBNJ_PIXEL_ENGINE_TASK_QUEUE_H

#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>

class TaskQueue {
public:
    using Task = std::function<void()>;

    void push(Task task);
    std::size_t pull();

private:
    std::queue<Task> que;
    std::mutex mtx;
};

#endif
