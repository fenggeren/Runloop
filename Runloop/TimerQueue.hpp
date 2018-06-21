//
//  TimerQueue.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/20.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once
#include <vector>
#include <list>
#include "Callbacks.h"
#include "Timer.hpp"

class TimerQueue 
{
public: 

    TimerQueue()
    :heap_()
    {}

    bool addTimer(Timer* time)
    {
        if (time->heapIndex() != INT32_MAX)
        {
            time->setHeapIndex(heap_.size());
            heap_.push_back(time);
            upHeap(heap_.size() - 1);
            return true;
        }
        return false;
    }

    bool empty() const 
    {
        return heap_.empty();
    }

    int64_t waitDurationUsec(int64_t duration) const 
    {
        if (heap_.empty())
        {
            return duration;
        }

        int64_t induration = heap_[0]->expiry() - usecNow();
        return induration < duration ? induration : duration;
    }

    std::size_t cancelTimer(Timer* timer)
    {
        removeTimer(timer);
        return 0;
    }

    std::list<Timer*> getReadyTimers()
    {
        std::list<Timer*> timers;
        if (!heap_.empty())
        {
            const int64_t now = usecNow();
            while (!heap_.empty() && heap_[0]->expiry() <= now)
            {
                timers.push_back(heap_[0]);
                removeTimer(heap_[0]);
            }
        }
        return timers;
    }

private: 

    void upHeap(std::size_t index)
    {
        while (index > 0)
        {
            std::size_t parent = (index - 1) / 2;
            if (heap_[index]->expiry() >= heap_[parent]->expiry())
                break;
            swapHeap(index, parent);
            index = parent;
        }
    }

    void downHeap(std::size_t index)
    {
        std::size_t child = index * 2 + 1;
        while (child < heap_.size())
        {
            std::size_t minChild = (child + 1 == heap_.size() || 
                                    heap_[child]->expiry() < heap_[child+1]->expiry())
                                    ? child : child + 1;
            if (heap_[index]->expiry() < heap_[minChild]->expiry())
                break;
            swapHeap(index, minChild);
            index = minChild;
            child = index * 2 + 1;
        }
    }

    void swapHeap(std::size_t index1, std::size_t index2)
    {
        std::swap(heap_[index1], heap_[index2]);
        heap_[index1]->setHeapIndex(index1);
        heap_[index2]->setHeapIndex(index2);
    }


    void removeTimer(Timer* timer)
    {
        std::size_t index = timer->heapIndex();
        if (!heap_.empty() && index < heap_.size())
        {
            if (index == heap_.size() - 1)
            {
                heap_.pop_back();
            }
            else 
            {
                swapHeap(index, heap_.size() - 1);
                heap_.pop_back();
                if (index > 0 && heap_[index]->expiry() < heap_[(index-1)/2]->expiry())
                {
                    upHeap(index);
                }
                else 
                {
                    downHeap(index);
                }
            }
        }
    }
 


private:
    std::vector<Timer*> heap_;
};


