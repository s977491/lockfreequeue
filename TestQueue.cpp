//============================================================================
// Name        : TestQueue.cpp
// Author      : Arthur
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <sys/time.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>

#include "P1C1Queue.h"
#include "SP1C1Queue.h"
#include "SSP1C1Queue.h"

using namespace std;

typedef int	q_type;
q_type x[REPETITIONS];

std::chrono::time_point<std::chrono::high_resolution_clock> start;
//#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>





template<class T, unsigned long Q_SIZE = QUEUE_SIZE>
class BoostQueue {
public:
	BoostQueue() { f = 0; e = 0;}
	void
	push(T *x)
	{
		//cout << "pushing " << endl;
		while (!q_.push(x)) {
			asm volatile("rep; nop" ::: "memory");
			++f;
		}
	}

	T *
	pop()
	{
		//cout << "poping " << endl;
		T *x;
		while (!q_.pop(x)) {
			asm volatile("rep; nop" ::: "memory");
			++e;
		}
		return x;
	}

	int f;
	int e;

private:
	boost::lockfree::spsc_queue<T *, boost::lockfree::capacity<Q_SIZE>> q_;
};


template<class Q>
struct Worker {
	Worker(Q *q, size_t id = 0)
		: q_(q),
		thr_id_(id)
	{}

	Q *q_;
	size_t thr_id_;
};

template<class Q>
struct Producer : public Worker<Q> {
	Producer(Q *q, size_t id)
		: Worker<Q>(q, id)
	{}

	void operator()()
	{
		 start = std::chrono::high_resolution_clock::now();
		//set_thr_id(Worker<Q>::thr_id_);
		for (auto i = 0; i < REPETITIONS; ++i) {
			Worker<Q>::q_->push(x + i);
		}

	}
};

template<class Q>
long long performanceRun(int runNumber, Q& q) {
		thread thrP = thread(Producer<Q>(&q, 1));
	for (auto i = 0; i < REPETITIONS; ++i) {
		q_type * p = q.pop();
//		if (*p != i)
//			cout << "error bug in quque" << endl;

	}

	thrP.join();

	auto finish = std::chrono::high_resolution_clock::now();

	long long ttt = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
	long long ops = REPETITIONS * 1000000000LL / ttt;
	std::cout << ops << " ops" << std::endl;
	return ops;
}

int main()
{
//    using namespace boost::lambda;
//    typedef std::istream_iterator<int> in;
//
//    std::for_each(
//        in(std::cin), in(), std::cout << (_1 * 3) << " " );
	cout << "size of pointer here :" << sizeof(q_type*) << endl;
	for (int i = 0; i < REPETITIONS; ++i)
		x[i] = i;
	{
		{
			cout << "P1C1Queue, simple lock free queue" << endl;
			P1C1Queue<q_type> b_q1;
			for (int i =0; i < 5; ++i)
				performanceRun(1, b_q1);
		}
		{
			cout << "SP1C1Queue, sparse lock free queue" << endl;
			SP1C1Queue<q_type> b_q2;
			for (int i =0; i < 5; ++i)
				performanceRun(1, b_q2);
		}

		{
			cout << "SSP1C1Queue, sparse, but use head tail instead of element check" << endl;
			SSP1C1Queue<q_type> b_q3;
					for (int i =0; i < 5; ++i)
						performanceRun(1, b_q3);
		}
		cout << "boost" << endl;
		BoostQueue<q_type> b_qboost;

		for (int i =0; i < 5; ++i)
			performanceRun(1, b_qboost);
	}

	return 0;
}
