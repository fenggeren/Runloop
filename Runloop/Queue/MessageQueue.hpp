//
//  MessageQueue.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/22.
//  Copyright © 2018年 huanao. All rights reserved.
//
#pragma once
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <map>
#include <condition_variable>
#include <thread>
#include <memory>
#include "Worker.hpp"



/*

串行队列，  一个个任务扔到队列里，由同一个worker执行。  只要该worker还有任务执行，该worker就和该队列绑定.

并行队列， 一个个任务扔到队列里，由剩余多个worker执行。 

queue 和 worker   1对多关系.

当队列没有任务执行时，就将worker还给worker_pool。

*/

class Queue;
class Worker;
class SerialWorker;
class SerialQueue;
class ConcurrentQueue;
class ConcurrentWorker;

using QueuePtr = std::shared_ptr<Queue>;
using SerialQueuePtr = std::shared_ptr<SerialQueue>;
using ConcurrentQueuePtr = std::shared_ptr<ConcurrentQueue>;
using WorkerPtr = std::shared_ptr<Worker>;
using SerialWorkerPtr = std::shared_ptr<SerialWorker>;
using ConcurrentWorkerPtr = std::shared_ptr<ConcurrentWorker>;
using Handler = std::function<void()>;



class QueueBase
{
public:
    
};

// 串行队列
// 有任务扔到队列， 
class SerialQueue : public std::enable_shared_from_this<SerialQueue>
{ 
public:
    SerialQueue();

    bool setWorker(SerialWorkerPtr worker);

    void put(Handler&& handler);
    
    Handler get();
    
    void cancel();
private: 
    std::queue<Handler> handlers_; 
    bool cancel_; 
    SerialWorkerPtr worker_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::mutex work_mutex_;
};
// 并行队列
class ConcurrentQueue : public std::enable_shared_from_this<ConcurrentQueue>
{
public:
    void sync(Handler&& handler);
    void async(Handler&& handler);
    void after(double delay, Handler&& handler);

public:
    ConcurrentQueue();

    bool setWorker(ConcurrentWorkerPtr worker);

    void put(Handler&& handler);
    
    Handler get();
    
    void cancel();
private: 
    std::queue<Handler> handlers_; 
    bool cancel_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::mutex work_mutex_;
    
    ConcurrentWorkerPtr worker_;
};



class Dispatch 
{
public:

    static QueuePtr getQueue(int queue_t);

    void async(Handler&& handler, int queue_t)
    {
 
    }
    
    void sync(Handler&& handler, int queue_t)
    {

    }

private:
    std::vector<QueuePtr> queues_;
    std::vector<Worker> workers_;
};






