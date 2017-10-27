/*
 * P1C1Queue.h
 *
 *  Created on: Jan 18, 2016
 *      Author: laia
 */

#ifndef P1C1QUEUE_H_
#define P1C1QUEUE_H_

#include "common.h"
#include <atomic>

using namespace std;


template<class T, unsigned long Q_SIZE = QUEUE_SIZE>
class P1C1Queue {
private:
	char p0[64];
	int capacity;
	char p01[64];
	int mask;
	char p1[64];
	T* buffer[Q_SIZE];
	char p2[64];
	std::atomic<int64_t> tail;
	char p3[64];
	std::atomic<int64_t> head;
	char p4[64];
	int64_t tailCache;
	char p5[64];
	int64_t headCache;
	char p6[64];

public:
	P1C1Queue() {
		head.store(0);
		tail.store(0);
		capacity = findNextPositivePowerOfTwo(Q_SIZE);
		if (capacity != Q_SIZE)
			cout << capacity << " diff " << Q_SIZE << endl;
		mask = capacity-1;
		tailCache = 0;
		headCache = 0;


	};
	virtual ~P1C1Queue(){};

	inline bool
	pushImpl(T *x)
	{
		//cout << "pushing " << endl;
		const int64_t curTail = tail.load(memory_order_relaxed);
		const int64_t wrap = curTail - capacity;
		if (headCache <= wrap) {
			headCache = head.load(memory_order_acquire);
			if (headCache <=wrap) {
				return false;
			}
		}

		buffer[(int)curTail & mask] = x;
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
		const int index = (int) curHead & mask;
		T* e = buffer[index];
		head.store(curHead + 1, memory_order_release);
		return e;
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


};

#endif /* P1C1QUEUE_H_ */
