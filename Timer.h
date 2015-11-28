#ifndef TIMER_H
#define TIMER_H
#include<chrono>
#include<thread>
#include<condition_variable>
#include<atomic>
#include<iostream>
namespace Yuan_FrameWork {
	class Timer {
	private:
		std::chrono::system_clock::time_point __beg_timePoint;
		std::chrono::system_clock::time_point __end_timePoint;
		std::chrono::milliseconds __interval;
		std::condition_variable_any __cv;
		std::mutex __mtx;
		std::function<void()> __callBack_function;
		std::atomic_bool __isTicking;
		std::thread __tickingThread;
	public:
		Timer();
		~Timer();
		/*
		*to set time interval(ms)
		**/
		void start(std::chrono::milliseconds _interval);
		void stop();
		template<typename _Callable >
		void setTimerHandler(_Callable);
		void wakeUp();
	};
	Timer::Timer()
		:__isTicking(true)
		, __callBack_function(std::move(
			[]() {}
			)) {
	}
	void Timer::start(std::chrono::milliseconds _interval) {
		__interval = _interval;
		__beg_timePoint = std::chrono::system_clock::now();
		__end_timePoint = __beg_timePoint + _interval;
		__tickingThread = std::move(
			std::thread(
				[this]() {
			while (__isTicking) {
					std::unique_lock<std::mutex> lck(__mtx);
					__cv.wait_for(lck, __interval);
					if (__callBack_function) {
						__callBack_function();
					}
				__end_timePoint += __interval;
			}
		}));
		
	}
	void Timer::wakeUp() {

	}
	void Timer::stop() {
		__isTicking = false;
		__tickingThread.join();
	}

	template<typename _Callable >
	void Timer::setTimerHandler(_Callable _callBack_function) {
		__callBack_function = std::move(_callBack_function);
	}
	Timer::~Timer() {
		if (__isTicking) {
			stop();
		}
	}
}
#endif
