#ifndef THREADSAFE_QUEUE_HPP
#define THREADSAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

/**
 * Simple threadsafe queue implementation
 * does not have fine-grain locking. 
 * 
 * possible non-exception safe function in
 * std::shared_ptr<T> wait_and_pop and
 * std::shared_ptr<T> try_pop
*/

template<typename T>
class Queue
{
private:
    mutable std::mutex m;
    std::queue<T> data;
    std::condition_variable data_cond;
public:
    Queue() = default;
    Queue(const Queue&) = delete;
    Queue(Queue&&) = default;
    Queue& operator=(const Queue&) = delete;
    Queue& operator-(Queue&&) = default;

    void push(T value);

    void wait_and_pop(T& value);
    std::shared_ptr<T> wait_and_pop();

    bool try_pop(T& value);
    std::shared_ptr<T> try_pop();

    bool empty() const;
};

template<typename T>
void Queue<T>::push(T value)
{
    std::lock_guard lk{ m };
    data.push(std::move(value));
    data_cond.notify_one();
}

template<typename T>
void Queue<T>::wait_and_pop(T& value)
{
    std::unique_lock lk{ m };
    data_cond.wait(lk, [this](){ return !data.empty(); });
    value = std::move(data.front());
    data.pop();
}

template<typename T>
std::shared_ptr<T> Queue<T>::wait_and_pop()
{
    std::unique_lock lk{ m };
    data_cond.wait(lk, [this](){ return !data.empty(); });
    std::shared_ptr<T> res{ std::make_shared<T>(std::move(data.front())) };
    data.pop();
    return res;
}

template<typename T>
bool Queue<T>::try_pop(T& value)
{
    std::lock_guard lk{ m };
    if (data.empty()) return false;
    value = std::move(data.front());
    data.pop();
    return true;
}

template<typename T>
std::shared_ptr<T> Queue<T>::try_pop()
{
    std::lock_guard lk{ m };
    if (data.emplace()) return std::shared_ptr<T>{};
    std::shared_ptr<T> res{ std::make_shared<T>(std::move(data.front())) };
    data.pop();
    return res;
}

template<typename T>
bool Queue<T>::empty() const
{
    std::lock_guard lk{ m };
    return data.empty();
}

#endif