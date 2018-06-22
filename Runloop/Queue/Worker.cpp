//
//  Worker.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/22.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "Worker.hpp"

SerialWorker::SerialWorker()
: idle_(true)
{
    thread_ = std::thread([this]{
        while(true)
        {
            if(idle_ || queue_ == nullptr)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock);
            } else {
                auto handler = queue_->get();
                if (handler) {handler();}
            }
        }
    });
}

void SerialWorker::setQueue(SerialQueuePtr queue)
{
    std::lock_guard<std::mutex> lock(mutex_);
    idle_ = false;
    queue_ = queue;
    condition_.notify_one();
}


void SerialWorker::idle()
{
    idle_ = true;
    queue_ = nullptr;
}

void SerialWorker::wait()
{
    thread_.join();
}






ConcurrentWorker::ConcurrentWorker(int num)
: threads_(num)
{
    for(size_t i = 0; i < threads_.size(); ++i)
    {
        threads_[i] = std::thread([this]{
            while(true)
            {
                if(idle_ || queue_ == nullptr)
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    condition_.wait(lock);
                } else {
                    auto handler = queue_->get();
                    if (handler) {handler();}
                }
            }
        });
    }
}

void ConcurrentWorker::setQueue(ConcurrentQueuePtr queue)
{
    std::lock_guard<std::mutex> lock(mutex_);
    idle_ = false;
    queue_ = queue;
    condition_.notify_all();
}

void ConcurrentWorker::idle()
{
    idle_ = true;
    queue_ = nullptr;
}

void ConcurrentWorker::wait()
{
    for(auto& thread : threads_)
    {
        thread.join();
    }
}




