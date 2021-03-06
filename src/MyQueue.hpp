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
#include "GenericQueue.hpp"

template <typename T>
class MyQueue : public GenericQueue<T>
{
public:
        using simpleLock = std::lock_guard<std::mutex>;
        using uniqueLock = std::unique_lock<std::mutex>;

        MyQueue(int maxSize = 100): m_max_size(maxSize){} //default constructor
        MyQueue(const MyQueue<T>&);                       //copy constructor;
        MyQueue(MyQueue<T> &&) noexcept;                  //move constructor
        MyQueue& operator=(const MyQueue<T>&);            //assignment
        MyQueue& operator=(MyQueue<T>&& rhs) noexcept;    //move assignment
        virtual ~MyQueue(){}

        virtual void pop(T& data) override;
        virtual bool pop_try(T& data) override;
        virtual bool push(T data) override;
        virtual bool isEmpty(void) const override;
        virtual int getMaxSize(void) const override;

private:
        size_t m_max_size;
        std::queue<T> m_queue;
        mutable std::mutex m_mutex;  // define as mutable so that it can be locked in const functions.
        std::condition_variable m_cond;

};

//copy constructor
template<typename T>
MyQueue<T>::MyQueue(const MyQueue<T>& src)
{
        simpleLock lock(src.m_mutex);
        m_queue = src.m_queue;
        m_max_size = src.m_max_size;
}

//move constructor
template<typename T>
MyQueue<T>::MyQueue(MyQueue<T> && src) noexcept
{
        simpleLock lock(src.m_mutex);
        m_queue = std::move(src.m_queue);
        m_max_size = src.m_max_size;
}

//assignment
template<typename T>
MyQueue<T>& MyQueue<T>::operator=(const MyQueue<T>& rhs)
{
        //check self assignment
        if (this == &rhs)
        {
                return *this;
        }
        std::lock(m_mutex, rhs.m_mutex);
        simpleLock lock(m_mutex, std::adopt_lock);
        simpleLock lock_rhs(rhs.m_mutex, std::adopt_lock);
        m_queue = rhs.m_queue;
        m_max_size = rhs.m_max_size;
        return *this;
}

//move assignment
template<typename T>
MyQueue<T>& MyQueue<T>::operator=(MyQueue<T>&& rhs) noexcept
{
        //check self assignment
        if (this == &rhs)
        {
                return *this;
        }
        std::lock(m_mutex, rhs.m_mutex);
        simpleLock lock(m_mutex, std::adopt_lock);
        simpleLock lock_rhs(rhs.m_mutex, std::adopt_lock);
        m_queue = std::move(rhs.m_queue);
        m_max_size = rhs.m_max_size;
        return *this;
}

template<typename T>
bool MyQueue<T>::push(T data)
{
        simpleLock lock(m_mutex);
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
        uniqueLock lock(m_mutex);
        m_cond.wait(lock, [this]{return !m_queue.empty(); });
        data = m_queue.front();
        m_queue.pop();
}

template<typename T>
bool MyQueue<T>::pop_try(T& data)
{
        simpleLock lock(m_mutex);
        if (m_queue.empty())
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
        simpleLock lock(m_mutex);
        return m_queue.empty();
}

template<typename T>
int MyQueue<T>::getMaxSize() const
{
        return m_max_size;
}

#endif /* MYQUEUE_HPP_ */
