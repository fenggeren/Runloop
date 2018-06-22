//
//  Worker.hpp
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
#include "MessageQueue.hpp"

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



/*
 worker应该独立于queue，
 worker只负责有任务就执行，没任务就等待。
 由中转机构负责给其任务执行.
 */
class Worker
{
    
};


// Serial Worker
class SerialWorker
{
public:
    
    SerialWorker();
    void setQueue(SerialQueuePtr queue);
    
    void idle();
    
    void wait();
    
private:
    friend class SerialQueue;
    
    std::thread thread_;
    std::condition_variable condition_;
    std::mutex mutex_;
    bool idle_;
    SerialQueuePtr queue_;
};

class ConcurrentWorker
{
public:
    ConcurrentWorker(int num);
    
    void setQueue(ConcurrentQueuePtr queue);
    
    void idle();
    
    void wait();
    
    
private:
    friend class ConcurrentQueue;
     
    std::condition_variable condition_;
    std::mutex mutex_;
    bool idle_;
    ConcurrentQueuePtr queue_;
    std::vector<std::thread> threads_;
};




