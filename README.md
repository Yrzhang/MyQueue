The Problem:

Coding Exercise

Produce a C++ template FIFO class for a Linux-based system which can be used to pass work items (for example, a struct defining some work to be done) from a high priority, timing critical thread (the writer) to a low-priority background thread (the reader) where the work is done. You should make use of the STL and compile with C++11.

The class should be able to queue a number of objects in a thread-safe manner for a single reader & writer.

The class should provide the following functions :

1. pop

The reader thread calls this function to fetch the next available item. If no items are available, the background thread should be put to sleep until an item becomes available.

2. pop_try

The reader thread calls this function to fetch the next available item. If no items are available, the function will return immediately and indicate to the calling thread that no items are available.

3. push

The writer thread calls this function to push an item into the queue. If there is no room in the queue for the item, the function should return immediately and indicate to the calling thread that the item was not pushed to the queue.

Please provide your solution in a git repository, with a build system and instructions on how to build and run your app


Build Instruction:


