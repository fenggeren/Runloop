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
#include "asio.hpp"


class timer_manager
{
public:
    
    using Handler = std::function<void()>;
    using basic_timer = asio::basic_waitable_timer<std::chrono::steady_clock>;
    
    timer_manager(asio::io_context& io)
    :io_context_(io)
    {}
    
    struct timer_profile
    {
        timer_profile(asio::io_context& io):
        timer_(new basic_timer(io)){}
        
        ~timer_profile() { delete timer_; }
        
        basic_timer* timer_;
        Handler handler_;
        int repeats_;
        int cur_repeat_;
        double delay;
        double interval;
        int timer_id;
        bool cancel_perform_{false}; // 取消后是否执行.
    };
    
    int runAfter(double delay, double interval, int repeats, Handler&& handler)
    {
        timer_profile* timer = get_basic_timer();
        timer->repeats_ = repeats;
        timer->cur_repeat_ = 0;
        timer->interval = interval;
        timer->timer_id = get_next_id();
        timer->handler_ = handler;
        timer->timer_->expires_from_now(std::chrono::milliseconds(int(delay * 1000)));
        timer->timer_->async_wait(std::bind(&timer_manager::perform_handler, this, std::placeholders::_1, timer));
        valid_timers_.insert(timer);
        return timer->timer_id;
    }
    
    
    
    void perform_handler(asio::error_code ec, timer_profile* timer)
    {
        timer->cur_repeat_++;
        if (ec == asio::error::operation_aborted) { // 取消了
            if (timer->cancel_perform_ && timer->handler_) {
                timer->handler_();
            }
            valid_timers_.erase(timer);
            invalid_basic_timer(timer);
        } else if (!ec) {
            if (timer->handler_) {
                timer->handler_();
            }
            if (timer->cur_repeat_ >= timer->repeats_ &&
                (timer->repeats_ != -1)) {
                valid_timers_.erase(timer);
                invalid_basic_timer(timer);
            } else {
                timer->timer_->expires_from_now(std::chrono::milliseconds(int(timer->interval * 1000)));
                timer->timer_->async_wait(std::bind(&timer_manager::perform_handler, this, std::placeholders::_1, timer));
            }
        }
    }

private:
    
    timer_profile* get_basic_timer()
    {
        timer_profile *timer = nullptr;
        if (invalid_timers_.size() > 0) {
            timer = *invalid_timers_.begin();
        } else {
            timer = new timer_profile(io_context_);
        }
        
        return timer;
    }
    
    void invalid_basic_timer(timer_profile* timer)
    {
        timer->handler_ = nullptr;
        if (capacity_ <= valid_timers_.size() + invalid_timers_.size()) {
            delete timer;
        } else {
            invalid_timers_.insert(timer);
        }
    }
    
    int get_next_id()
    {
        static int id = 0;
        return id++;
    }
    
private:
    std::unordered_set<timer_profile*> valid_timers_;
    std::unordered_set<timer_profile*> invalid_timers_;
    
    asio::io_context& io_context_;
    
    // 有效+无效 <= capacity_
    int capacity_;
};





using deadline_timer = asio::basic_waitable_timer<std::chrono::steady_clock>;

void test_timers()
{
    asio::io_context io;
    auto guard = asio::make_work_guard(io);
    constexpr int duration = 1;
    deadline_timer timer(io);
    timer.expires_from_now(std::chrono::milliseconds(1111));
    
    std::function<void(asio::error_code)> handler =
    [&](asio::error_code ec)
    {
        std::cout << " async wait" << std::endl;
//        timer.expires_from_now(std::chrono::seconds(duration));
//        timer.async_wait(handler);
    };
    
    timer.async_wait(handler);
    io.run();
}


void test_manager()
{
    asio::io_context io;
    auto guard = asio::make_work_guard(io);
    
    timer_manager manager(io);
    
    for (int i = 0; i < 100; i++)
    {
        manager.runAfter(1, 1, 1000, []{
            std::cout << std::this_thread::get_id() << "   =======" << std::endl;
        });
    }
    
    
    io.run();
}


void test_io()
{
    asio::io_context io;
    auto guard = asio::make_work_guard(io);
    
    asio::ip::tcp::acceptor acceptor(io,
                                     asio::ip::tcp::endpoint(asio::ip::tcp::v4(),10080));
    acceptor.async_accept([](asio::error_code ec, asio::ip::tcp::socket socket)
    {
        if (!ec) {
            std::cout << " ======== ";
        }
    });
    
    io.run();
}


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


int main(int argc, const char * argv[]) {

//    test_timers();
    
    std::thread thread([]{
       testRunloop2();
    });

    thread.join();
    return 0;
}












