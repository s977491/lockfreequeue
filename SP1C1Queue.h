/*
 * SP1C1Queue.h
 *
 * S stand for sparse, data are now separate by cachesize to prevent false sharing
 * * this use whether object exist there to check for pop
 * for push, here try to check a look ahead
 *
 *  Created on: Jan 18, 2016
 *      Author: laia
 */

#ifndef SP1C1QUEUE_H_
#define SP1C1QUEUE_H_

#define AHEAD_STEP Q_SIZE /4
#include "common.h"
#include <atomic>

using namespace std;
template<class T, unsigned long Q_SIZE = QUEUE_SIZE>
class SP1C1Queue {
private:
	char p0[64];
	int capacity;
	char p01[64];
		int mask;
	char p1[64];
	std::atomic<T*> buffer[Q_SIZE << ELESHIFT];
//	char p2[64];
//	std::atomic<int64_t> tail;
//	char p3[64];
//	std::atomic<int64_t> head;
//	char p4[64];
	int tailCache;
	char p5[64];
	int headCache;
	char p6[64];

	int tailLimit;
public:
	SP1C1Queue() {

		capacity = findNextPositivePowerOfTwo(Q_SIZE);
		if (capacity != Q_SIZE)
			cout << capacity << " diff " << Q_SIZE << endl;
		mask = capacity-1;
		tailCache = 0;
		headCache = 0;
		tailLimit = tailCache + capacity;
		for (int i = 0; i < Q_SIZE << ELESHIFT; ++i ) {
			buffer[i].store(NULL);
		}


	};
	virtual ~SP1C1Queue(){};

	inline int id(int index, int mask) {
		return (index & mask) << ELESHIFT;
	}

	inline bool offerSlowPath() {
		const int tgt = id(tailCache + AHEAD_STEP, mask);
		if (buffer[tgt].load(memory_order_acquire) == NULL)
			tailLimit = tailCache + AHEAD_STEP;
		else {
			const int offset = id(tailCache, mask);
			if (buffer[offset].load(memory_order_acquire) != NULL)
				return false;
		}
	 return true;
	}
	inline bool
	pushImpl(T *x)
	{
//		if (tailCache >= tailLimit && !offerSlowPath()) {
//			return false;
//		}

		const int tgt = id(tailCache, mask);
		if (buffer[tgt].load(memory_order_acquire) != NULL)
			return false;
		buffer[tgt].store(x, memory_order_release);
		++ tailCache;
		return true;
	}
	inline void push(T *x) {
		while (!pushImpl(x)) {
			asm volatile("rep; nop" ::: "memory");
		}
	}
	inline T* popImpl() {

		const int tgt = id(headCache,mask);
		T* ret = buffer[tgt].load(memory_order_acquire);
		if (ret == NULL)
			return NULL;
		buffer[tgt].store(NULL, memory_order_release);
		++headCache;
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

};

#endif /* SP1C1QUEUE_H_ */
