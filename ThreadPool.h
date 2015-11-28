#ifndef THREADPOOL
#define THREADPOOL
#include<thread>
#include<inttypes.h>
#include<cstdint>
#include<queue>
#include<vector>
#include<future>
#include<atomic>
#include<functional>
#include<condition_variable>
#include<assert.h>
#include"Timer.h"
namespace Yuan_FrameWork {
#define Log printf
	/**
	*default ThreadPoolPriority
	*/
	struct ThreadPoolPriority {
		static const uint8_t __default_low = 1;
		static const uint8_t __default_normal = 2;
		static const uint8_t __default_high = 3;
	};
	/**
	*warning:this implemention can only be used in real-time thread
	*For the tasks of large amount of computation,it maybe works in very low efficiency!
	*make a threadpool
	*surporting several thread on one CPU core with a local thread queue
	*surporting overload self balancing,in the other word,make the task num in every thread equal
	*supporting everything which is callable
	*surporting const/mutable parameter input
	*/
	
	class ThreadPool {
#define GenericType void()
	public :
		static int ThreadPolicyDynamicNum ;
	public:
		std::atomic_bool _isThreadPoolWorking;
		std::condition_variable _globalThreadTaskCV;
		std::mutex _globalThreadPoolMtx;
		static thread_local std::queue<std::packaged_task<void()>> localThreadTaskQueue;
		Yuan_FrameWork::Timer _ticker;
	public:
		uint8_t _usedCPUCore;
		uint8_t _CPUCore;
		uint8_t _allocatedCPUCore;
		std::queue<std::function<GenericType>> _globalThreadTaskQueue;
		std::vector<std::thread> _threadVector;
	public:
		ThreadPool() :
			_isThreadPoolWorking(false)
			
			, _CPUCore(std::thread::hardware_concurrency())
			, _allocatedCPUCore(_CPUCore > 4 ? _CPUCore / 2 : _CPUCore) 
			, _usedCPUCore(_allocatedCPUCore)
		    , _ticker(){
			ThreadPool::ThreadPolicyDynamicNum = 2;
		}
		ThreadPool(const ThreadPool &) = delete;
		ThreadPool &operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool &&other) {
			*this = std::move(other);
		}
		ThreadPool&operator=(const ThreadPool&& other) {
			if (&other != this) {
				this->_CPUCore = other._CPUCore;
			}
			return *this;
		}
		void StartWork();
		template<typename _Callable, typename... _Args>
		auto addNewTask(_Callable && newTask, _Args &&... args)
			->std::future<typename  std::result_of<_Callable(_Args...)>::type>;
		void stopWork();
		~ThreadPool();
	};

	ThreadPool::~ThreadPool() {
		if (_isThreadPoolWorking) {
			stopWork();
		}
	}
	/**
	*stop the thread pool
	*/
	void ThreadPool::stopWork() {
		_isThreadPoolWorking = false;
		_globalThreadTaskCV.notify_all();
		for (auto &taskProcessor : _threadVector)
		{
			taskProcessor.join();
		}
	}

	/**
	*start Running your taskQueue!
	*supporting admin thread dynamicly!
	*supporting race!
	*/
	void ThreadPool::StartWork() {
		if (_isThreadPoolWorking) {
			Log("the thread pool is already working!\n");
			return;
		}
		_isThreadPoolWorking = true;
		for (uint8_t threadIndex = 0; threadIndex < this->_allocatedCPUCore; threadIndex++) {
			_threadVector.emplace_back(
				[this] {
				for (;;) {
					std::function<GenericType> task;
					{
						std::unique_lock<std::mutex> lock(this->_globalThreadPoolMtx);
						this->_globalThreadTaskCV.wait(lock, [this]() {
							return !_isThreadPoolWorking || !this->_globalThreadTaskQueue.empty()
								; }
						);
						if (!_isThreadPoolWorking) {
							return;
						}
						task = std::move(this->_globalThreadTaskQueue.front());
						this->_globalThreadTaskQueue.pop();
					}

					task();
				}
			}
			);
			_ticker.setTimerHandler([this]() {
				std::lock_guard<std::mutex> lck(this->_globalThreadPoolMtx);
				if (_usedCPUCore < this->_globalThreadTaskQueue.size()) {

					for (size_t __threadIndex = 0; __threadIndex < ThreadPool::ThreadPolicyDynamicNum; __threadIndex++) {
						this->_threadVector.emplace_back(this->_threadVector.front());
					}
					_usedCPUCore += ThreadPool::ThreadPolicyDynamicNum;
					return;
				}
				if (_usedCPUCore > this->_globalThreadTaskQueue.size() + ThreadPool::ThreadPolicyDynamicNum - 1) {
					for (size_t __threadIndex = 0; __threadIndex < ThreadPool::ThreadPolicyDynamicNum; __threadIndex++) {
						this->_threadVector.end()



					}
					_usedCPUCore -= ThreadPool::ThreadPolicyDynamicNum;
					return;
				}
			}
				);
		}

	}
	/*
	*add Tasks:add everything callable
	*/
	template<typename _Callable, typename... _Args>
	auto ThreadPool::addNewTask(_Callable && newTask, _Args &&... args)
		-> std::future<typename  std::result_of<_Callable(_Args...)>::type> {
		assert(_isThreadPoolWorking.load());
		using FutureType = typename std::result_of<_Callable(_Args...)>::type;
		auto taskBinder = std::bind(std::forward<_Callable>(newTask), std::forward<_Args>(args)...);
		auto realTaskWrapper = std::make_shared<std::packaged_task<FutureType()>>(taskBinder);
		{
			std::lock_guard<std::mutex> lock(_globalThreadPoolMtx);
			_globalThreadTaskQueue.emplace([realTaskWrapper]() {
				(*realTaskWrapper)();
			});
		}
		_globalThreadTaskCV.notify_all();
		return realTaskWrapper->get_future();
	}

}

#endif // THREADPOOL

