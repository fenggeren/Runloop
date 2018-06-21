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

    TimerQueue() :heap_() {}

    bool addTimer(Timer* time);

    bool empty() const 
    {
        return heap_.empty();
    }

    int64_t waitDurationUsec(int64_t duration) const;

    std::size_t cancelTimer(Timer* timer);

    std::list<Timer*> getReadyTimers();

private: 

    void upHeap(std::size_t index);

    void downHeap(std::size_t index);

    void swapHeap(std::size_t index1, std::size_t index2);

    void removeTimer(Timer* timer);
 
 
private:
    std::vector<Timer*> heap_;
};


