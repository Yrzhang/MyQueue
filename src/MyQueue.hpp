/*
 * MyQueue.hpp
 *
 *  Created on: 29 Jul 2016
 *      Author: Yangran Zhang
 *
 */

#ifndef MYQUEUE_HPP_
#define MYQUEUE_HPP_
#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class MyQueue
{
public:
	MyQueue(int maxSize = 100): m_max_size(maxSize){}
	//copy constructor;
	MyQueue(const MyQueue<T>&);
	virtual ~MyQueue(){}
	//disallow assignment operator
	MyQueue& operator=(const MyQueue<T>&) = delete;

	//move constructor
	//todo

	virtual void pop(T& data);
	virtual bool pop_try(T& data);
	virtual bool push(T data);
	virtual bool isEmpty(void) const;
	virtual int getMaxSize(void) const;

private:
	size_t m_max_size;
	std::queue<T> m_queue;
	mutable std::mutex m_mutex;  // define as mutable so that it can be locked in const functions.
	std::condition_variable m_cond;
};

template<typename T>
MyQueue<T>::MyQueue(const MyQueue<T>& inQueue)
{
	std::unique_lock<std::mutex> lock(inQueue.m_mutex);
	m_queue = inQueue.m_queue;
	m_max_size = inQueue.m_max_size;
}

template<typename T>
bool MyQueue<T>::push(T data)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_queue.size() > m_max_size)
	{
		return false;
	}
	m_queue.push(data);
	m_cond.notify_one();
	return true;
}

template<typename T>
void MyQueue<T>::pop(T& data)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cond.wait(lock, [this]{return !isEmpty(); });
	data = m_queue.front();
	m_queue.pop();
}

template<typename T>
bool MyQueue<T>::pop_try(T& data)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (isEmpty())
	{
		return false;
	}
	data = m_queue.front();
	m_queue.pop();
	return true;
}

template<typename T>
bool MyQueue<T>::isEmpty() const
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

template<typename T>
int MyQueue<T>::getMaxSize() const
{
	return m_max_size;
}

#endif /* MYQUEUE_HPP_ */
