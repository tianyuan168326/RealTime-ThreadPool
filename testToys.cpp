#include"ThreadPool.h"
#include<memory>
#include<iostream>
int main() {
	std::unique_ptr<Yuan_FrameWork::ThreadPool> threadPool(new Yuan_FrameWork::ThreadPool());
	threadPool->StartWork();
	//add task with args
	threadPool->addNewTask([](int i) {
		//std::this_thread::sleep_for(std::chrono::seconds(5));
		std::cout <<"one param test case :"<< i << std::endl;
	}, 10
		);
	threadPool->addNewTask([](int i,int j) {
		for (int i = 0; i < 1000000; i++);
		std::cout <<"two params test case :"<< i <<" "<<j<< std::endl;
	}, 10,100
		);
	//add task with no params
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(11));
		std::cout << "no param test cas11" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << "no param test case10" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(9));
		std::cout << "no param test case9" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
		std::cout << "no param test case8" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(7));
		std::cout << "no param test case7" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(6));
		std::cout << "no param test case6" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		std::cout << "no param test case5" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(4));
		std::cout << "no param test case4" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
		std::cout << "no param test case3" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		std::cout << "no param test case2" << std::endl;
	});
	threadPool->addNewTask([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::cout << "no param test case1111" << std::endl;
	});
	//If the pragme terminate now,our stdout won't be printed,so we add
	int a;
	std::cin >> a;
	return 0;
}
