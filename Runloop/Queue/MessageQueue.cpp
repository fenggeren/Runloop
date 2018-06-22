//
//  MessageQueue.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/22.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "MessageQueue.hpp"

SerialQueue::SerialQueue()
:cancel_(false)
{
    // worker_ = global::getworker();
}

bool SerialQueue::setWorker(SerialWorkerPtr worker)
{
    std::lock_guard<std::mutex> lock(work_mutex_);
    if (worker_) return false;
    worker_ = worker;
    worker_->setQueue(shared_from_this());
    return true;
}

void SerialQueue::put(Handler&& handler)
{
    std::unique_lock<std::mutex> lock(mutex_);
    handlers_.push(handler);
    condition_.notify_one();
}

Handler SerialQueue::get()
{
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [&]{ return handlers_.size() > 0;});
    Handler handler(std::move(handlers_.front()));
    handlers_.pop();
    return handler;
}

void SerialQueue::cancel()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cancel_ = true;
    while(handlers_.size() > 0)
    {
        handlers_.pop();
    }
    condition_.notify_one();
    worker_->idle();
    
}





ConcurrentQueue::ConcurrentQueue()
:cancel_(false)
{
    
}

bool ConcurrentQueue::setWorker(ConcurrentWorkerPtr worker)
{
    std::lock_guard<std::mutex> lock(work_mutex_);
    if (worker_) return false;
    
    worker_ = worker;
    worker_->setQueue(shared_from_this());
    return true;
}

void ConcurrentQueue::put(Handler&& handler)
{
    std::unique_lock<std::mutex> lock(mutex_);
    handlers_.push(handler);
    condition_.notify_one();
}

Handler ConcurrentQueue::get()
{
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [&]{ return handlers_.size() > 0;});
    Handler handler(std::move(handlers_.front()));
    handlers_.pop();
    return handler;
}

void ConcurrentQueue::cancel()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cancel_ = true;
    while(handlers_.size() > 0)
    {
        handlers_.pop();
    }
    condition_.notify_all();
    worker_->idle();
}




