#include <iostream>
#include <vector>
#include <atomic>

#define CACHE_LINE_SIZE 64

template <class Value>
class node_t
{
public:
    Value value;
    char pad[CACHE_LINE_SIZE];
};

template <class Value>
class spsc_ring_buffer
{
public:
    explicit spsc_ring_buffer(size_t size_)
    {
        head.store(0);
        tail.store(0);

        buffer = std::vector<node_t<Value>> (size_ + 1);
    }

    bool enqueue(Value v)
    {
        size_t curr_head = head.load(std::memory_order_acquire); // мы должны быть уверенными, что dequeue уже всё позаписывал
        size_t curr_tail = tail.load(std::memory_order_relaxed); // в него пишет только producer, который сейчас и исполняется => можно не беспокоиться

        if(is_full(curr_head, curr_tail)) return false;

        buffer[curr_tail].value = v;
        tail.store(next(curr_tail), std::memory_order_release); // мы должны сделать sw-отношение между потоками

        return true;
    }

    bool dequeue(Value& v)
    {
        size_t curr_tail = tail.load(std::memory_order_acquire); // всё
        size_t curr_head = head.load(std::memory_order_relaxed); // тут

        if(is_empty(curr_head, curr_tail)) return false;

        v = buffer[curr_head].value;
        head.store(next(curr_head), std::memory_order_release); // симметрично

        return true;
    }

private:
    inline bool is_full(size_t head, size_t tail)
    {
        return (next(tail) == head);
    }

    inline bool is_empty(size_t head, size_t tail)
    {
        return (head == tail);
    }

    inline size_t next(size_t index)
    {
        return (index + 1) % buffer.size();
    }

    std::atomic<size_t> tail;
    std::vector<node_t<Value>> buffer;
    std::atomic<size_t> head;
};
