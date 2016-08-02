/*
 * MyLFQueue.hpp
 *
 *  Created on: 29 Jul 2016
 *      Author: Yangran Zhang
 *
 */

#ifndef MyLFQueue_HPP_
#define MyLFQueue_HPP_
#include <vector>
#include <atomic>
#include <condition_variable>
#include <memory>
#include "GenericQueue.hpp"

template <typename T>
class MyLFQueue : public GenericQueue<T>
{
public:
		MyLFQueue(int maxSize = 100): m_max_size(maxSize), m_vec(std::vector<T> (m_max_size)),
									  m_cur_push(0), m_cur_pop(0)
		{
			m_is_Blocking.store(false);
			pop_ready_ptr = std::make_shared<std::promise<void> >();
			pop_ready_fut = pop_ready_ptr->get_future();
		}

		//example of these operations can be found in MyQueue class.  Delete them here.
		MyLFQueue(const MyLFQueue<T>&) = delete;               //copy constructor;
		MyLFQueue(MyLFQueue<T> &&) = delete;                  //move constructor
		MyLFQueue& operator=(const MyLFQueue<T>&) = delete;            //assignment
		MyLFQueue& operator=(MyLFQueue<T>&& rhs) = delete;    //move assignment
        
		virtual ~MyLFQueue(){}
		virtual void pop(T& data) override;
        virtual bool pop_try(T& data) override;
        virtual bool push(T data) override;
        virtual bool isEmpty(void) const override;
        virtual int getMaxSize(void) const override;

private:
        size_t m_max_size;
        std::vector<T> m_vec;
        std::atomic<size_t> m_cur_push;
        std::atomic<size_t> m_cur_pop;
        std::atomic<bool> m_is_Blocking;
        std::shared_ptr<std::promise<void> > pop_ready_ptr;
        std::shared_future<void> pop_ready_fut;
};

template<typename T>
bool MyLFQueue<T>::push(T data)
{
	size_t cur_push = m_cur_push.load();

	//Return to the front of the queue if the index reaches the end.
	size_t cur_push_next = (cur_push + 1) % m_max_size;

	if (cur_push_next == m_cur_pop.load())
	{
		//queue is full
		return false;
	}

	//push the data;
	m_vec[cur_push] = data;
	//advance the current push index
	m_cur_push.store(cur_push_next);

	bool isBlocking = m_is_Blocking.load();
	if (isBlocking)
	{
		//unblock the pop operation
		m_is_Blocking.store(false);
		pop_ready_ptr->set_value();
		
		//create another set of promise and future.
		pop_ready_ptr = std::make_shared<std::promise<void> >();
		pop_ready_fut = pop_ready_ptr->get_future();
	}
	return true;
}

template<typename T>
void MyLFQueue<T>::pop(T& data)
{
	size_t cur_pop = m_cur_pop.load();
	if (cur_pop == m_cur_push.load())
	{
		//que is empty, wait here.
		m_is_Blocking.store(true);
		pop_ready_fut.wait();
	}

	//pop item
	data = m_vec[cur_pop];
	//Return to the front of the queue if the index reaches the end.
	size_t cur_pop_next = (cur_pop + 1) % m_max_size;
	m_cur_pop.store(cur_pop_next);
}

template<typename T>
bool MyLFQueue<T>::pop_try(T& data)
{
	size_t cur_pop = m_cur_pop.load();

	if (cur_pop == m_cur_push.load())
	{
		//que is empty
		return false;
	}
	//pop item
	data = m_vec[cur_pop];

	//Return to the front of the queue if the index reaches the end.
	size_t cur_pop_next = (cur_pop + 1) % m_max_size;
	m_cur_pop.store(cur_pop_next);
    return true;
}

template<typename T>
bool MyLFQueue<T>::isEmpty() const
{
	return (m_cur_push.load() == m_cur_pop.load());
}

template<typename T>
int MyLFQueue<T>::getMaxSize() const
{
        return m_max_size;
}

#endif /* MyLFQueue_HPP_ */
