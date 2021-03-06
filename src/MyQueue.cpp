//============================================================================
// Name        : MyQueue.cpp
// Author      : yang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <cassert>
#include <thread>
#include <future>
#include <algorithm>
#include "MyQueue.hpp"
#include "MyLFQueue.hpp"
#include "GenericQueue.hpp"

namespace {
struct someData 
{
	int i;
	std::string str;
	bool operator==(const someData& rhs) 
	{
		return i == rhs.i && str == rhs.str;
	}
};

//unility function to get randome number
int getRandom(int min, int max) 
{
	return (std::rand() % static_cast<int>(max + 1 - min) + min);
}

//create a queue and add a few items
MyQueue<someData> createQueue(int number) 
{
	assert(number > 1);
	int max = number > 100 ? number : 100;
	MyQueue<someData> dataQue(max);
	for (int i = 1; i <= number; ++i) 
	{
		std::string text = "item " + std::to_string(i);
		dataQue.push( { i, text });
	}
	return dataQue;
}

//test constructors and assignment operators in single thread
void simpleTestCase() 
{
	std::cout << "simple test case..." << std::endl;
	std::vector < MyQueue<someData> > queVec;
	for (int i = 0; i < 3; ++i) 
	{
		//test move constructor
		queVec.push_back(createQueue(3));
	}

	//default constrctor
	MyQueue<someData> queue1;

	//test move assignment operator
	queue1 = createQueue(3);

	//test assignment operator
	MyQueue<someData> queue2;
	queue2 = queVec[2];

	//test copy constructor
	MyQueue<someData> queue3(queue2);

	//check values after multiple copies.
	someData testData;
	for (int i = 0; i < 3; ++i) 
	{
		queue3.pop(testData);
	}

	assert(testData.i == 3);
	assert(testData.str == "item 3");
	assert(queue3.getMaxSize() == 100);
	assert(queue3.isEmpty() == true);

	//try to pop when the que is empty
	bool result = queue3.pop_try(testData);
	assert(result == false);

	std::cout << "simple tests passed." << std::endl;
}

/*Test case with 2 threads.  One thread push data to the queue and other thread pop data.
 *two threads start at the same time, and the timing between each pop and push
 *operation is random, to simulate real life scenario.
 */
void TwoThreadTestCase(GenericQueue<someData>& testQueue) 
{
	int number = 10;
        std::vector<someData> inDataVec(number);
        int value = 1;
	//generate values for input vector       
        std::generate(begin(inDataVec), end(inDataVec), [&value]()
                        {
                                std::string text = "item " + std::to_string(value);
                                someData tem = {value, text};
                                value++;
                                return tem;
                        });

	srand(static_cast<unsigned int>(time(NULL))); //random seed
	
	std::promise<void> start_test, push_finished;
	std::shared_future<void> start_test_fut(start_test.get_future());
	std::shared_future<void> push_finished_fut(push_finished.get_future());
	std::future<void> push_fut;
	std::future < std::vector<someData> > pop_fut;

	try 
	{
		push_fut = std::async(std::launch::async,
						[&testQueue, &inDataVec, start_test_fut, &push_finished]()
						{
							start_test_fut.wait();
							for (auto i : inDataVec)
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(getRandom(0, 20)));
								testQueue.push(i);
							}
							push_finished.set_value();
						});

		pop_fut = std::async(std::launch::async,
						[&testQueue, start_test_fut, push_finished_fut]()
						{
							start_test_fut.wait();
							std::vector<someData> outDataVec;

							while (true)
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(getRandom(0, 20)));
								if (testQueue.isEmpty() &&
										push_finished_fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
								{
									break;
								}
								someData tem;
								bool isblocking = static_cast<bool> (getRandom(0,1));
								if(isblocking)
								{
									testQueue.pop(tem);
									outDataVec.push_back(tem);
								}

								else
								{
									if(testQueue.pop_try(tem))
									{
										outDataVec.push_back(tem);
									}
								}
							}
							return outDataVec;
						});
		start_test.set_value();
		push_fut.get();
		std::vector<someData> resultDataVec = pop_fut.get();

		assert(testQueue.isEmpty());
		assert(resultDataVec.size() == inDataVec.size());
                assert(std::equal(begin(inDataVec), end(inDataVec), begin(resultDataVec)));

		std::cout << "2 threads tests passed" << std::endl;
	} 
	
	catch (...) 
	{
		start_test.set_value();
		throw;
	}
}

}//end of namespace


int main() 
{
	while (true) 
	{
    	simpleTestCase();
    	MyLFQueue<someData> queue_no_lock;
    	MyQueue<someData> queue_with_lock;  	
    	GenericQueue<someData>& Q1 = queue_no_lock;
    	GenericQueue<someData>& Q2 = queue_with_lock;
    	
    	std::cout << "Start testing lock free queue" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		TwoThreadTestCase(Q1);
		
		std::cout << "Start testing queue with lock" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		TwoThreadTestCase(Q2);
		
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "Press ctr + C to end the test" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
	return 0;
}
