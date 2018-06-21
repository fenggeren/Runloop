//
//  RunLoop.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/19.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include <map>
#include <memory>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include "RunLoop.hpp"
#include "HelperUtils.hpp"

#define runloops    runloop_map()
static std::map<pthread_t, RunLoop*>& runloop_map()
{
   static std::map<pthread_t, RunLoop*>* map = new std::map<pthread_t, RunLoop*>;
   return *map;
}

RunLoop::RunLoop()
:threadID_(pthread_self()),
kqueueFd_(kqueue()), 
interrupter_(),
state_(Init),
shutdown_(false)
{
    struct kevent events[1];
    EV_SET(&events[0], interrupter_.readDescriptor(), EVFILT_READ, EV_ADD, 0, 0, &interrupter_);
    if (kevent(kqueueFd_, events, 1, 0, 0, 0) == -1)
    {
        // ..eror
    }
}

RunLoop& RunLoop::currentRunLoop()
{
    auto iter = runloops.find(pthread_self());
    if (iter != runloops.end())
    {
        return *iter->second;
    }
    else 
    {
        runloops[pthread_self()] = new RunLoop();
        return *runloops[pthread_self()];
    }
}

void RunLoop::run()
{
    if (state_ == Init || state_ == Exit) {
        std::cout << "初始化" << std::endl;
    } else {
        return;
    }
    
    state_ = Running;
    
    while(state_ != Exit)
    {
        timespec timeout = {0, 0};
        getTimeout(-1, timeout);
        struct kevent events[128];
        state_ = BeforeWaiting;
        int numEvents = kevent(kqueueFd_, 0, 0, events, 128, &timeout);
        state_ = AfterWaiting;
        
        for (int i = 0; i < numEvents; ++i)
        {
            void* ptr = reinterpret_cast<void*>(events[i].udata);
            if (ptr == &interrupter_)  // 用于唤醒runloop
            {
                interrupter_.reset();
            }
            else 
            {
                /// 处理读写事件
            }
        }
        
        state_ = BeforeTimers;
        auto readyTimers = timerQueue_.getReadyTimers();
        performTimers(std::move(readyTimers));

        performHandlers();
    }
}

void RunLoop::getTimeout(long usec, timespec& ts)
{
    const static long maxUsec = 5 * 60 * 1000 * 1000;
    usec = timerQueue_.waitDurationUsec((usec < 0 || maxUsec < usec) ? maxUsec : usec);
    ts.tv_sec = usec / kMicroSecondsPerSecond;
    ts.tv_nsec = (usec % kMicroSecondsPerSecond) * 1000;
}


// 唤醒runloop
void RunLoop::wakeUp()
{
    interrupter_.interrupt();
}

void RunLoop::stopRun()
{
    state_ = Exit;
}

void RunLoop::performImmediately(Functor&& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else 
    {
        perform(std::move(cb));
    }
}

void RunLoop::perform(Functor&& cb)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.push_back(cb);
    }
    wakeUp();
}

void RunLoop::performHandlers()
{   
    std::list<Functor> runHandlers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        runHandlers.swap(handlers_);
    }

    for(const auto& handler : runHandlers)
    {
        handler();
    }
}

void RunLoop::performTimers(std::list<Timer*>&& timers)
{
    long now = usecNow();
    std::list<Timer*> finished;
    std::list<Timer*> repeated;
    for(const auto& timer : timers)
    {
        timer->perform();
        if (timer->repeat())
        {
            timer->restart(now);
            repeated.push_back(timer);
        }
        else 
        {
            finished.push_back(timer);
        }
    }

    for (auto timer : repeated)
    {
        timerQueue_.addTimer(timer);
    }
    
    for (auto& timer : finished)
    {
        delete timer;
        timer = nullptr;
    }
}

///////////////////////////

void RunLoop::addTimer(Timer* timer)
{
    if (isInLoopThread())
    {
        timerQueue_.addTimer(timer);
        wakeUp();
    }
    else
    {
        perform([&]{
            timerQueue_.addTimer(timer);
        });
    }
}

Timer* RunLoop::performAfter(double delay, Functor&& cb)
{
    long timePoint = usecNow() + delay * kMicroSecondsPerSecond;
    std::cout << " performAfter " << timePoint << std::endl;
    Timer* timer = new Timer(std::move(cb), timePoint, 0);
    addTimer(timer);
    return timer;
}
Timer* RunLoop::performAt(double time, Functor&& cb)
{
    Timer* timer = new Timer(std::move(cb), time * kMicroSecondsPerSecond, 0);
    addTimer(timer);
    return timer;
}
Timer* RunLoop::performEvery(double interval, Functor&& cb)
{ 
    Timer* timer = new Timer(std::move(cb), 
                    usecNow() + interval * kMicroSecondsPerSecond, 
                    interval * kMicroSecondsPerSecond);
    addTimer(timer);
    return timer;
}


