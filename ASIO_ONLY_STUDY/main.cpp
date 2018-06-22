//
//  main.cpp
//  ASIO_ONLY_STUDY
//
//  Created by huanao on 2018/6/11.
//  Copyright © 2018年 huanao. All rights reserved.
//

#define ASIO_STANDALONE 1

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <pthread.h>
 #include <unistd.h>






#include "RunloopManager.hpp"

void test()
{
    RLDemo::ThreadPool pool(10);
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout  << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout  << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout  << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout  << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout  << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.dispatch([]{
        sleep(0.5);
        std::cout  << std::this_thread::get_id() << " =========1 " << std::endl;
        sleep(0.5);
    });
    
    pool.join();
}

#include <atomic> 
#include <chrono>


#include "RunLoop.hpp"

void printNow()
{
    std::cout << "performNow:     " << usecNow() << std::endl;
}

void testRunloop()
{
    RunLoop& runloop = RunLoop::currentRunLoop();
    
    runloop.performImmediately([]{
        std::cout << "111111111" << std::endl;
    });
    
    runloop.perform([]{
        std::cout << "22222222" << std::endl;
    });
    
    runloop.performAfter(3., []{
        printNow();
        std::cout << "333333333" << std::endl;
        testRunloop();
    });
    
    runloop.performAfter(0.133, []{
        std::cout << "0.133" << std::endl;
//        testRunloop();
    });
    
    runloop.performAfter(0.233, []{
        std::cout << "0.233" << std::endl;
//        testRunloop();
    });
    
    runloop.performAfter(1.133, []{
        std::cout << "1.133" << std::endl;
//        testRunloop();
    });
    
    runloop.performAfter(2.333, []{
        printNow();
        std::cout << "2.333" << std::endl;
//        testRunloop();
    });
    
    runloop.run();
}


void testRunloop2()
{
    RunLoop& runloop = RunLoop::currentRunLoop();
    
    runloop.performAfter(0.001, []{
        printNow();
        std::cout << "0.001" << std::endl;
        // testRunloop();
    });
    
    runloop.performAfter(1, []{
        printNow();
        std::cout << "1" << std::endl;
//        testRunloop();
    });
    
    runloop.performAfter(1.33333, []{
        printNow();
        std::cout << "1.33333" << std::endl;
        //        testRunloop();
    });

    runloop.performAfter(3, []{
        printNow();
        std::cout << "3" << std::endl;
        //        testRunloop();
    });
    

    
    runloop.run();
}

#include <queue>

int main(int argc, const char * argv[]) {

//    test_timers();
    
    std::priority_queue<int> priority;
    
    std::thread thread([]{
       testRunloop2();
    });

    thread.join();
    return 0;
}












