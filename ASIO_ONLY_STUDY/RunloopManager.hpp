//
//  Runloop.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/14.
//  Copyright © 2018年 huanao. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <functional>
#include <list>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <set>
#include <unordered_set>
#include <string>
#include <sys/socket.h>

namespace RLDemo
{
using Handler = std::function<void()>;


class CustomMessageQueue
{
    
};

class TimerMessageQueue
{
    
};

class MessageQueue;




class MessageQueue
{
public:
    
    void put(Handler&& handler)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        handlers_.push(handler);
        condition_.notify_one();
    }
    
    Handler get()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&]{ return handlers_.size() > 0;});
        Handler handler(std::move(handlers_.front()));
        handlers_.pop();
        return handler;
    }
    
private:
    std::queue<Handler> handlers_;
    std::mutex mutex_;
    std::condition_variable condition_;
};


class ThreadPool
{
public:
    
    ThreadPool(size_t num)
    :threads_(num)
    {
        config();
    }
    
    void config()
    {
        for(size_t i = 0; i < threads_.size(); ++i)
        {
            threads_[i] = std::thread([this]{
                while (true) {
                    Handler handler(queue_.get());
                    handler();
                }
            });
        }
    }
    
    void dispatch(Handler&& handler)
    {
        queue_.put(std::forward<Handler>(handler));
    }
    
    void join()
    {
        for(auto& thread : threads_)
        {
            if(thread.joinable())
            {
                thread.join();
            }
        }
    }
 
private:
    std::vector<std::thread> threads_;
    MessageQueue queue_;
};



enum
{
    kRunloopRunFinished = 1,
    kRunloopRunStopped = 2,
    kRunloopRunTimedOut = 3,
    kRunloopRunHandledSource = 4,
};

class RunloopManager
{
public:
    struct Runloop;
    struct RunloopMode;
    struct RunloopSource;
    struct RunloopTimer;
    struct RunloopObserver;
    
    using RunloopRef = std::shared_ptr<Runloop>;
    using RunloopWRef = std::weak_ptr<Runloop>;
    using RunloopModeRef = std::shared_ptr<RunloopMode>;
    using RunloopSourceRef = std::shared_ptr<RunloopSource>;
    using RunloopTimerRef = std::shared_ptr<RunloopTimer>;
    using RunloopObserverRef = std::shared_ptr<RunloopObserver>;
    
    using Port = int;
    
    enum RunloopActivity
    {
        kRunloopEntry = (1UL << 0),
        kRunloopBeforeTimers = (1UL << 1),
        kRunloopBeforeSources = (1UL << 2),
        kRunloopBeforeWaiting = (1UL << 5),
        kRunloopAfterWaiting = (1UL << 6),
        kRunloopExit = (1UL << 7),
        kRunloopAllActivities = 0x0FFFFFFFU
    };
    
    struct Runloop
    {
        pthread_mutex_t lock_;
        Port wakeupPort_;
        pthread_t pthread_;
        std::unordered_set<RunloopModeRef> modes_;
//        std::unordered_set<RunloopMode> modeItems_;
        std::shared_ptr<RunloopMode> currentMode_;
        std::list<std::function<void()>> handlers_;
    };
    
    struct RunloopMode
    {
        pthread_mutex_t lock_;
        std::string name_;
        bool stopped_;
        long long observerMask_;
        
        std::unordered_set<RunloopSourceRef> sources0_;
        std::unordered_set<RunloopSourceRef> sources1_;
        std::unordered_set<RunloopObserverRef> observers_;
        std::vector<RunloopTimerRef> timers_;
    };
    
    struct RunloopSource
    {
        uint32_t bits_;
        pthread_mutex_t lock_;
        std::function<void()> schedule_;
        std::function<void()> handler_;
        std::function<void()> cancel_;
        std::function<Port()> getPort_;
    };
    
    
    struct RunloopTimer
    {
        uint16_t bits_;
        pthread_mutex_t lock_;
        RunloopWRef runloop_;
        std::unordered_set<RunloopModeRef> modes_;
        double nextFireDate_;
        double interval_;
        double tolerance_;
        uint64_t firTSR_;
        std::function<void()> handler_;
    };
    
    struct RunloopObserver
    {
        pthread_mutex_t lock_;
        RunloopWRef runloop_;
        unsigned int activities_;
        std::function<void(RunloopActivity)> handler_;
    };
    
public:
    
    int run(RunloopRef runloop, RunloopModeRef mode)
    {
        if (mode->stopped_) {
            mode->stopped_ = false;
            return kRunloopRunStopped;
        }
        
        
        do
        {
            if (mode->observerMask_ & kRunloopBeforeTimers) {
                doObservers(runloop, mode, kRunloopBeforeTimers);
            }
            if (mode->observerMask_ & kRunloopBeforeSources) {
                doObservers(runloop, mode, kRunloopBeforeSources);
            }
            doBlocks(runloop, mode);
            
            if (doServicePort(runloop->wakeupPort_))
            {
                goto handle_msg;
            }
            
            if (mode->observerMask_ & kRunloopBeforeWaiting) {
                doObservers(runloop, mode, kRunloopBeforeWaiting);
            }
            
            if (mode->observerMask_ & kRunloopAfterWaiting) {
                doObservers(runloop, mode, kRunloopAfterWaiting);
            }
            
        handle_msg:;
            
        } while(true);
    }
    
    void doObservers(RunloopRef runloop, RunloopModeRef mode, RunloopActivity activity)
    {
        std::list<RunloopObserverRef> observers;
        for (auto const& obs : mode->observers_)
        {
            if (obs->activities_ & activity) {
                observers.push_back(obs);
            }
        }
        
        for (auto& rlo : observers)
        {
            if (rlo->handler_)
            {
                rlo->handler_(activity);
            }
        }
    }
    
    void doBlocks(RunloopRef runloop, RunloopModeRef mode)
    {
        for(auto& h : runloop->handlers_)
        {
            h();
        }
    }
    
    bool doServicePort(Port port)
    {
        return false;
    }
    
private:
    
    
    static RunloopRef createRunloop(pthread_t t)
    {
        return nullptr;
    }
    
    
    
private:
    
    MessageQueue messageQueue_;
    
};
 

}
 










