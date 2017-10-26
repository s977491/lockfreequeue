# lockfreequeue
implementation of a queue trying to beat down boost's lock free queue

boost library need to be installed first, and point the directory of it in the Makefile

change the TestQueue.cpp 's main to choose which queue to compare against, current there are 3 flavors
P1C1Queue.h
SP1C1Queue.h
SSP1C1Queue.h


They use different ways to do lock free single producer single consumer queue


in my machine, 
boost's queue  ~200M ops/sec`
Arthur Queue ~300M ops /sec
