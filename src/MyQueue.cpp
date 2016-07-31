//============================================================================
// Name        : MyQueue.cpp
// Author      : yang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MyQueue.hpp"

using namespace std;

int main() {
	cout << "Start test..." << endl; // prints !!!Hello World!!!
	MyQueue<int> intQueue(100);
	intQueue.push(10);
	intQueue.push(20);
	intQueue.push(20);
	intQueue.push(20);
	intQueue.push(20);
	intQueue.push(20);
	int data = 0;
	while (intQueue.pop_try(ref(data)))
	{
		cout << "data = " << data << endl;
	}
	return 0;
}
