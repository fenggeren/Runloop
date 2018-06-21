//
//  PipeInterrupter.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/19.
//  Copyright © 2018年 huanao. All rights reserved.
//


#pragma once


class PipeInterrupter
{
public:

    PipeInterrupter();
    ~PipeInterrupter();

    void recreate();
    
    void interrupt();

    bool reset();

    int readDescriptor() const 
    {
        return readDescriptor_;
    }


private:

    void openDescriptors();
    void closeDescriptors();

    int readDescriptor_;
    int writeDescriptor_;
};





