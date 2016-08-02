/*
 * GenericQueue.hpp
 *
 *  Created on: 2 Aug 2016
 *      Author: yang
 */

#ifndef GENERICQUEUE_HPP_
#define GENERICQUEUE_HPP_

//empty base class for 2 implementation of queues.
template <typename T>
class GenericQueue
{
public:
		//constructor
		GenericQueue(){}
		//default constructor
		virtual ~GenericQueue(){}
		virtual void pop(T& data){}
        virtual bool pop_try(T& data){return false;}
        virtual bool push(T data){return false;}
        virtual bool isEmpty(void) const{return false;}
        virtual int getMaxSize(void) const{return 0;}
};
#endif /* GENERICQUEUE_HPP_ */
