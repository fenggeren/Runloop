//
//  PipeInterrupter.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/19.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "PipeInterrupter.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <exception>


PipeInterrupter::PipeInterrupter()
{
    openDescriptors();
}

void PipeInterrupter::openDescriptors() 
{
    int pipefds[2];
    if (pipe(pipefds) == 0)
    {
        readDescriptor_ = pipefds[0];
        fcntl(readDescriptor_, F_SETFL, O_NONBLOCK);
        writeDescriptor_ = pipefds[1];
        fcntl(writeDescriptor_, F_SETFL, O_NONBLOCK);

        fcntl(readDescriptor_, F_SETFD, FD_CLOEXEC);
        fcntl(writeDescriptor_, F_SETFD, FD_CLOEXEC);
    }
    else 
    {

    }
}

PipeInterrupter::~PipeInterrupter()
{
    closeDescriptors();
}

void PipeInterrupter::closeDescriptors()
{
    if (readDescriptor_ != -1)
    {
        close(readDescriptor_);
    }
    if (writeDescriptor_ != -1)
    {
        close(writeDescriptor_);
    }
}

void PipeInterrupter::recreate()
{
    closeDescriptors();
    
    writeDescriptor_ = -1;
    readDescriptor_ = -1;

    openDescriptors();
}

void PipeInterrupter::interrupt()
{
    char byte = 0;
    ssize_t result = write(writeDescriptor_, &byte, 1);
    (void)result;
}

bool PipeInterrupter::reset()
{
    for(;;)
    {
        char data[1024];
        ssize_t bytesRead = read(readDescriptor_, data, sizeof(data));
        if (bytesRead < 0 /*&& errno == EINTR*/)
            continue;
        bool wasInterrupted = (bytesRead > 0);
        while(bytesRead == sizeof(data))
        {
            bytesRead = read(readDescriptor_, data, sizeof(data));
        }
        return wasInterrupted;
    }
}