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
#include "MyQueue.hpp"

using namespace std;

namespace
{
        struct someData
        {
                int i;
                string str;
        };

        //create a queue and add a few items
        MyQueue<someData> createQueue()
        {
                MyQueue<someData> dataQue(50);
                dataQue.push({10, "item 1"});
                dataQue.push({20, "item 2"});
                dataQue.push({30, "item 3"});
                return dataQue;
        }

        //test constructors and assignment operators in single thread
        void simpleTestCase()
        {
                cout << "simple test case..." << endl;
                vector<MyQueue<someData> > queVec;
                for (int i = 0; i < 3; ++i)
                {
                        queVec.push_back(createQueue());  //test move constructor
                }

                //default constrctor
                MyQueue<someData> queue1;

                //test move assignment operator
                queue1 = createQueue();

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

                assert(testData.i == 30);
                assert(testData.str == "item 3");
                assert(queue3.getMaxSize() == 50);
                assert(queue3.isEmpty() == true);

                //try to pop when the que is empty
                bool result = queue3.pop_try(testData);
                assert(result == false);

                cout << "simple tests passed." << endl;
        }
}

int main()
{
        simpleTestCase();
        return 0;
}

