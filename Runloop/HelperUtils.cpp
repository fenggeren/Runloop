//
//  HelperUtils.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/20.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "HelperUtils.hpp"


int64_t usecNow()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec;
}
