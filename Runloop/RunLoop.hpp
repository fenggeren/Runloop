//
//  RunLoop.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/19.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <string>
#include <functional>
#include <thread>
#include <list>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <memory>
#include "Callbacks.h"
#include "Timer.hpp"
#include "TimerQueue.hpp"
#include "PipeInterrupter.hpp"

using namespace Callback;

class RunLoop;
using RunloopPtr =  std::shared_ptr<RunLoop>;

class RunLoop
{
public:

    enum
    {
        Init = 0,
        Running = 1,
        BeforeTimers = 2,
        BeforeWaiting = 3,
        AfterWaiting = 4,
        Exit = 5,
    };

    static RunLoop& currentRunLoop();

//    static RunloopPtr mainRunLoop();

    ~RunLoop();

    void run();

    void performImmediately(Functor&& cb);
    void perform(Functor&& cb); 


    Timer* performAfter(double delay, Functor&& cb);
    Timer* performEvery(double interval, Functor&& cb);
    Timer* performAt(double time, Functor&& cb);
    
    void addTimer(Timer* timer);
    
    bool isInLoopThread() const { return threadID_ == pthread_self();}

    void stopRun();

    void wakeUp();
    

private:
    
    RunLoop();
    
    void getTimeout(long usec, timespec& ts);

    
    void performHandlers();

    void performTimers(std::list<Timer*>&& timers);
     
    
private:
    RunLoop(const RunLoop&) = delete ;
    RunLoop(RunLoop&&) = delete ;
    const RunLoop& operator=(const RunLoop&) = delete;
    const RunLoop& operator=(RunLoop&&) = delete;
    
private:
    
    const pthread_t threadID_;
    TimerQueue timerQueue_;

    int kqueueFd_;

    PipeInterrupter interrupter_;

    std::mutex mutex_;
    std::list<Functor> handlers_;
    std::atomic<bool> shutdown_;
    std::atomic<int> state_;
};




















