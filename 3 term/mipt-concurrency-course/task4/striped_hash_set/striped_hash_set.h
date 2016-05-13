#pragma once

#include <iostream>
#include <vector>
#include <forward_list>
#include <mutex>
#include <shared_mutex>
#include <atomic>

const int BUCKETS_PER_STRIPE = 5;
const int GROWTH_FACTOR = 2;
const int MAX_DENSITY = 500;

template <class Value, class Hash = std::hash<Value>>
class striped_hash_set
{
public:
    explicit striped_hash_set(size_t num_stripes_, int growth_factor_ = GROWTH_FACTOR, double max_density_ = MAX_DENSITY)
    {
        num_stripes = num_stripes_;
        growth_factor = growth_factor_;
        max_density = max_density_;

        storage_size.store(0);

        mutexes = std::vector<std::shared_timed_mutex> (num_stripes);
        buckets = std::vector<std::forward_list<Value>> (BUCKETS_PER_STRIPE * num_stripes);
    }

    void add(const Value& new_elem)
    {
        size_t hash_value = hasher(new_elem);

        std::unique_lock<std::shared_timed_mutex> locker(mutexes[get_stripe(hash_value)]);

        if(contains_nolock(new_elem))
        {
            return;
        }

        buckets[get_bucket(hash_value)].push_front(new_elem);

        storage_size++;

        if(get_load_factor() > max_density)
        {
            size_t old_num_buckets = buckets.size();

            locker.unlock();

            resize(old_num_buckets);
        }
    }

    void remove(const Value& elem)
    {
        size_t hash_value = hasher(elem);

        std::unique_lock<std::shared_timed_mutex> locker(mutexes[get_stripe(hash_value)]);

        if(!contains_nolock(elem))
        {
            return;
        }

        buckets[get_bucket(hash_value)].remove(elem);

        storage_size--;
    }

    bool contains(const Value& elem)
    {
        size_t hash_value = hasher(elem);

        std::shared_lock<std::shared_timed_mutex> locker(mutexes[get_stripe(hash_value)]);

        for(auto it = buckets[get_bucket(hash_value)].begin(); it != buckets[get_bucket(hash_value)].end(); it++)
        {
            if(*it == elem)
            {
                return true;
            }
        }

        return false;
    }

private:
    void resize(size_t old_num_buckets)
    {
        std::vector<std::unique_lock<std::shared_timed_mutex>> lockers;

        lockers.emplace_back(mutexes[0]);

        if(old_num_buckets != buckets.size()) // already resized
        {
            return;
        }

        for (size_t i = 1; i < num_stripes; i++)
        {
            lockers.emplace_back(mutexes[i]);
        }

        rehash();
    }

    void rehash()
    {
        std::vector<std::forward_list<Value>> old_buckets(buckets.size() * growth_factor);

        buckets.swap(old_buckets);

        for (size_t i = 0; i < old_buckets.size(); i++)
        {
            for (auto it = old_buckets[i].begin(); it != old_buckets[i].end(); it++)
            {
                size_t hash_value = hasher(*it);

                buckets[get_bucket(hash_value)].push_front(*it);
            }
        }
    }

    bool contains_nolock(const Value& elem)
    {
        size_t hash_value = hasher(elem);

        for(auto it = buckets[get_bucket(hash_value)].begin(); it != buckets[get_bucket(hash_value)].end(); it++)
        {
            if(*it == elem)
            {
                return true;
            }
        }

        return false;
    }

    size_t get_stripe(size_t hash_value) const
    {
        return hash_value % num_stripes;
    }

    size_t get_bucket(size_t hash_value) const
    {
        return hash_value % buckets.size();
    }

    inline double get_load_factor()
    {
        return (double) storage_size.load() / buckets.size();
    }

    std::vector<std::forward_list<Value>> buckets;
    std::vector<std::shared_timed_mutex> mutexes;

    std::atomic<size_t> storage_size;

    int growth_factor;
    double max_density;

    size_t num_stripes;

    Hash hasher;
};
