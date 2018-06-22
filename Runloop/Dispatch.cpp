//
//  Dispatch.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/21.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "Dispatch.hpp"
#include <map>
#include <queue>

class QueueBase;
using QueuePtr = std::shared_ptr<QueueBase>;


class QueueBase
{
public:
    void after(double delay, Functor&& func);
    void async(Functor&& func, Functor&& complete);
    void sync(Functor&& func);
    void repeat(size_t num, Functor&& func);
    void barrierAsync(Functor&& func);
    void barrierSync(Functor&& func);
};



// 串行、并行   &&  同步、异步
// 串行&异步 并行&同步 并行&异步
// 串行&同步 ❌ 死锁
// 串行队列、 并行队列
// 每个线程一个队列， 一个runloop?
// 除了主线程，其他线程默认不开启runloop.

/*
    串行队列、并行队列，  
    串行就是一个个任务以此执行，并行就是多个任务‘并行’执行，并行数量和work线程数量一致
    
    定时操作，创建定时器，转移到runloop里，到时后，转移到制定线程执行。

    或者独立出一个执行runloop的线程，用于定时操作。定时器到时后，到指定线程执行.
*/

template <typename QUEUE>
class Async 
{

};



class Queue
{
public:
    void after(double delay, Functor&& func) 
    {
        runloop_->performAfter(delay, std::move(func));
    }
    void async(Functor&& func, Functor&& complete)
    {
        runloop_->performImmediately(std::move(func));
    }
    void sync(Functor&& func);
    void repeat(size_t num, Functor&& func);
    void barrierAsync(Functor&& func);
    void barrierSync(Functor&& func);
    
    static QueuePtr getCurrentQueue();
    static QueuePtr getMainQueue();
    static QueuePtr getGlobalQueue(long priority);
    static QueuePtr queueCreate(const std::string& label, int attr);

private:
    RunloopPtr runloop_;
};

struct OperatorProfile
{
    Functor func;
    QueuePtr queue;
};


namespace Dispatch
{
using Handler = std::function<void()>;

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


QueuePtr getQueue(DispatchQueue queue)
{
    static std::map<DispatchQueue, QueuePtr> queues;
    return queues[queue];
}

void dispatchAfter(double delay, DispatchQueue queue, Functor&& func)
{   
 
}

void dispatchAsync(DispatchQueue queue, Functor&& func);
void dispatchSync(DispatchQueue queue, Functor&& func);
void dispatchRepeat(size_t num, DispatchQueue queue, Functor&& func);
void dispatchBarrierAsync(DispatchQueue queue, Functor&& func);
void dispatchBarrierSync(DispatchQueue queue, Functor&& func);
DispatchQueue dispatchGetCurrentQueue();
DispatchQueue dispatchGetMainQueue();
DispatchQueue dispatchGetGlobalQueue(long priority);
DispatchQueue dispatchQueueCreate(const std::string& label, int attr);
}
