//
//  HelperUtils.hpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/20.
//  Copyright © 2018年 huanao. All rights reserved.
//

#ifndef HelperUtils_hpp
#define HelperUtils_hpp

#include <stdio.h>
#include <sys/time.h>


static const int kMicroSecondsPerSecond = 1000 * 1000;
static const int kMilliSecondsPerSecond = 1000 ;
static const int kNanoSecondsPerSecond = 1000 * 1000 * 1000;
int64_t usecNow();


#endif /* HelperUtils_hpp */
