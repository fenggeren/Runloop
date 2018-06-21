#pragma once

#include "Callbacks.h"
#include <atomic>
#include <memory>
#include "HelperUtils.hpp"

using namespace Callback;

class Timer 
{
public:

    Timer(const TimerCallback&& cb, long when, long interval)
    : callback_(cb),
      expiry_(when),
      interval_(interval),
      repeat_(interval > 0.0)
    {

    }

    void perform()
    {
        callback_();
    }

    long expiry() const { return expiry_; }
    bool repeat() const { return repeat_;}

    void restart(long now)
    {
        if (repeat_)
        {
            expiry_ = now + interval_;
        }
        heapIndex_ = INT32_MAX;
    }
    
    std::size_t heapIndex() const { return heapIndex_; }
    void setHeapIndex(std::size_t index) { heapIndex_ = index; }
 
private:
    const TimerCallback callback_;
    long expiry_;
    const bool repeat_;
    const long interval_;

    std::size_t heapIndex_; 
 
};


class TimerID 
{
public:
    TimerID()
    : timer_(nullptr), 
    sequence_(0)
    {}

    TimerID(Timer* timer, int64_t seq)
    : timer_(timer), sequence_(seq)
    {}

private:
    Timer* timer_;
    int64_t sequence_;
};



