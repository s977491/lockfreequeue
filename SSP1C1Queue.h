/*
 * SSP1C1Queue.h
 *
 *  Created on: Oct 18, 2017
 *      Author: laia
 */

#ifndef SSP1C1QUEUE_H_
#define SSP1C1QUEUE_H_

#define ELESHIFT 4
#define AHEAD_STEP Q_SIZE /4
#include "common.h"
#include <atomic>

using namespace std;
template<class T, unsigned long Q_SIZE = QUEUE_SIZE>
class SSP1C1Queue {
private:
	char p0[64];
	int capacity;
	char p01[64];
		int mask;
	char p1[64];
	T* buffer[Q_SIZE << ELESHIFT];
	char p2[64];
	std::atomic<int64_t> tail;
	char p3[64];
	std::atomic<int64_t> head;
	char p4[64];
	int tailCache;
	char p5[64];
	int headCache;
	char p6[64];

	int tailLimit;
public:
	SSP1C1Queue() {

		capacity = SSP1C1Queue::findNextPositivePowerOfTwo(Q_SIZE);
		if (capacity != Q_SIZE)
			cout << capacity << " diff " << Q_SIZE << endl;
		mask = capacity-1;
		tailCache = 0;
		headCache = 0;
		tail.store(0);
		head.store(0);
		for (int i = 0; i < Q_SIZE << ELESHIFT; ++i ) {
			buffer[i]= (NULL);
		}


	};
	virtual ~SSP1C1Queue(){};

	inline int id(int index, int mask) {
		return (index & mask) << ELESHIFT;
	}
//
//	inline bool offerSlowPath() {
//		const int tgt = id(tailCache + AHEAD_STEP, mask);
//		if (buffer[tgt].load(memory_order_acquire) == NULL)
//			tailLimit = tailCache + AHEAD_STEP;
//		else {
//			const int offset = id(tailCache, mask);
//			if (buffer[offset].load(memory_order_acquire) != NULL)
//				return false;
//		}
//	 return true;
//	}
	inline bool
	pushImpl(T *x)
	{
		//cout << "pushing " << endl;
		const int64_t curTail = tail.load(memory_order_relaxed);
		const int64_t wrap = curTail - capacity +32;
		if (headCache <= wrap) {
			headCache = head.load(memory_order_acquire);
			if (headCache <=wrap) {
				return false;
			}
		}

		const int tgt = id(curTail, mask);
		buffer[tgt] = x;
		tail.store(curTail +1, memory_order_release);

		return true;
	}
	inline void push(T *x) {
		while (!pushImpl(x)) {
			asm volatile("rep; nop" ::: "memory");
		}
	}
	inline T* popImpl() {
		const int64_t curHead = head.load(memory_order_relaxed);
		if (curHead >=tailCache) {
			tailCache = tail.load(memory_order_acquire);
			if (curHead >= tailCache) {
				return NULL;
			}
		}
		const int tgt = id(curHead,mask);
		T* ret = buffer[tgt];

		buffer[tgt]=NULL;
		head.store(curHead + 1, memory_order_release);
		return ret;
	}

	inline T *
	pop()
	{
		//cout << "poping " << endl;
		T *x;
		while (( x= popImpl()) == NULL) {
			asm volatile("rep; nop" ::: "memory");
		}
		return x;
	}
private:
	static int findNextPositivePowerOfTwo(int value) {
		int i = 0;
		cout << "findNextPositivePowerOfTwo" << value << endl;
		while ( value>0) { ++i;value /=2; cout << "v:" << value<< endl;}
        return 1 << (i-1);
    }

};

#endif /* SSP1C1QUEUE_H_ */
