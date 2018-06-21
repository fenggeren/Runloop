//
//  Runloop.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/14.
//  Copyright © 2018年 huanao. All rights reserved.
//

#include "RunloopManager.hpp"
#include <map>
#include <thread>

//#define runloops    runloop_map()
//
//#define is_main_thread (pthread_main_np()!=0)
//
//
//void dispatch_main(Handler&& handler)
//{
//    if (is_main_thread) {
//        handler();
//    } else {
//        
//    }
//}
//
//static std::map<pthread_t, Runloop>& runloop_map()
//{
//    static std::map<pthread_t, Runloop>* map = new std::map<pthread_t, Runloop>;
//    return *map;
//}
//
//
//void test()
//{
//    pthread_t pid = pthread_self();
//    if (runloops.find(pid) == runloops.end()) {
//        
//    }
//    
//}

























