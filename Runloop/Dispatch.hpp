//
//  Dispatch.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/21.
//  Copyright © 2018年 huanao. All rights reserved.
//


#pragma once

#include <string>
#include <functional>
#include "RunLoop.hpp"

namespace Dispatch 
{

using DispatchQueue = int;
using DispatchGroup = int;
using Port = int;
const static int DISPATCH_QUEUE_SERIAL = 1;
const static int DISPATCH_QUEUE_CONCURRENT = 2;
 
void dispatchAfter(double delay, DispatchQueue queue, Functor&& func);
void dispatchAsync(DispatchQueue queue, Functor&& func);
void dispatchSync(DispatchQueue queue, Functor&& func);
void dispatchRepeat(size_t num, DispatchQueue queue, Functor&& func);
void dispatchBarrierAsync(DispatchQueue queue, Functor&& func);
void dispatchBarrierSync(DispatchQueue queue, Functor&& func);
DispatchQueue dispatchGetCurrentQueue();
DispatchQueue dispatchGetMainQueue();
DispatchQueue dispatchGetGlobalQueue(long priority);
DispatchQueue dispatchQueueCreate(const std::string& label, int attr);

class Group
{
public:
private:
};

DispatchGroup dispatchGroupCreate();
void dispatchGroupAsync(DispatchGroup group, DispatchQueue queue, Functor&& func);
long dispatchGroupWait(DispatchGroup group, double timeout);
void dispatchGroupNotify(DispatchGroup group, DispatchQueue queue, Functor&& func);
void dispatchGroupEnter(DispatchGroup group);
void dispatchGroupLeave(DispatchGroup group);

using DispatchData = int; // ??
using ReadWriteHeadler = std::function<void(DispatchData, int error)>;
void dispatchRead(Port fd, size_t length, DispatchQueue, ReadWriteHeadler&& handler);
void dispatchWrite(Port fd, DispatchData data, DispatchQueue queue, ReadWriteHeadler&& handler);
// dispatchIO....

class Semaphore
{
public:
    long wait(double timeout);
    long signal();
private:
    int value_;
};

}






