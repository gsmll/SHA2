#ifndef THREADSAFE_LIST_HPP
#define THREADSAFE_LIST_HPP

#include <mutex>
#include <condition_variable>
#include <shared_mutex>
#include <memory>

/**
 * Simple threadsafe list implementation
 * it is high granularity for concurrent access
*/

template<typename T>
class List
{
private:

    struct node
    {
        mutable std::shared_mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;

        node() : next{} {}
        node(node&& mv) : m{}, data{ std::move(mv.data) }, next{ std::move(mv.next) } {}
        node(const T& value) : data{ std::make_shared<T>(value) } {}
    };

    node head; // dummy head
    std::size_t _size = 0;

public:

    List() = default;
    List(const List&) = delete;
    List(List&&) = default;
    List& operator=(const List&) = delete;
    List& operator=(List&&) = default;
    ~List();

    void push_front(const T& value);
    std::shared_ptr<T> pop_front();

    template<typename Function>
    void for_each(Function&& func);

    template<typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate&& predicate);

    template<typename Predicate>
    void remove_if(Predicate&& predicate);

    template<typename Predicate>
    bool remove_first_if(Predicate&& predicate);

    std::size_t size() const;
    bool empty() const;

};

template<typename T>
List<T>::~List()
{
    remove_if([](const node&) { return true; }); // remove all nodes
}

template<typename T>
void List<T>::push_front(const T& value)
{
    std::unique_ptr<node> new_head{ new node{ value } };
    std::unique_lock lk{ head.m };
    new_head->next = std::move(head.next);
    head.next = std::move(new_head);
    ++_size;
}

template<typename T>
std::shared_ptr<T> List<T>::pop_front()
{
    std::unique_lock lk{ head.m };
    if (!head.next) return std::shared_ptr<T>{}; // return nullptr
    std::shared_ptr<T> res;
    std::shared_ptr<node> curr_head = std::move(head.next);
    {
        std::unique_lock head_lk{ curr_head->m };
        res = std::move(curr_head->data);    
        head.next = std::move(curr_head->next);
    }
    --_size;
    return res;
}

template<typename T>
template<typename Function>
void List<T>::for_each(Function&& func)
{
    node* curr = &head;
    std::unique_lock lk{ head.m };
    while (node* const next = curr->next.get())
    {
        std::unique_lock next_lk{ next->m };
        lk.unlock();
        std::forward<Function>(func)(*next->data);
        curr = next;
        lk = std::move(next_lk);
    }
}

template<typename T>
template<typename Predicate>
std::shared_ptr<T> List<T>::find_first_if(Predicate&& predicate)
{
    node* curr = &head;
    std::shared_lock lk{ head.m };
    while (node* const next = curr->next.get())
    {
        std::shared_lock next_lk{ next->m };
        lk.unlock();
        if (std::forward<Predicate>(predicate)(*next->data))
        {
            return next->data;
        }
        curr = next;
        lk = std::move(next_lk);
    }
    return std::shared_ptr<T>{};
}

template<typename T>
template<typename Predicate>
void List<T>::remove_if(Predicate&& predicate)
{
    node* curr = &head;
    std::unique_lock lk{ head.m };
    while (node* const next = curr->next.get())
    {
        std::unique_lock next_lk{ next->m };
        if (std::forward<Predicate>(predicate)(*next->data))
        {
            std::unique_ptr<node> old_head = std::move(curr->next);
            curr->next = std::move(next->next);
            next_lk.unlock();
            --_size;
        }
        else
        {
            lk.unlock();
            curr = next;
            lk = std::move(next_lk);
        }
    }
}

template<typename T>
template<typename Predicate>
bool List<T>::remove_first_if(Predicate&& predicate)
{
    node* curr = &head;
    std::unique_lock lk{ head.m };
    while (node* const next = curr->next.get())
    {
        std::unique_lock next_lk{ next->m };
        if (std::forward<Predicate>(predicate)(*next->data))
        {
            std::unique_ptr<node> old_head = std::move(curr->next);
            curr->next = std::move(next->next);
            next_lk.unlock();
            --_size;
            return true; // terminate early
        }
        else
        {
            lk.unlock();
            curr = next;
            lk = std::move(next_lk);
        }
    }
    return false;
}

template<typename T>
std::size_t List<T>::size() const 
{
    std::shared_lock lk{ head.m };
    return _size;
}

template<typename T>
bool List<T>::empty() const
{
    std::shared_lock lk{ head.m };
    return _size == 0;
}

#endif