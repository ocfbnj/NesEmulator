#include <pixel_engine/TaskQueue.h>

void TaskQueue::push(Task task) {
    {
        std::lock_guard lock{mtx};
        que.emplace(std::move(task));
    }
}

std::size_t TaskQueue::pull() {
    std::vector<Task> tasks;

    {
        std::lock_guard lock{mtx};
        while (!que.empty()) {
            Task task = std::move(que.front());
            que.pop();
            tasks.emplace_back(std::move(task));
        }
    }

    for (Task& task : tasks) {
        task();
    }

    return tasks.size();
}
