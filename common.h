/*
 * COMMON.h
 *
 *  Created on: Jan 18, 2016
 *      Author: laia
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>

#define ELESHIFT 3

#define REPETITIONS	100 * 1000 *1000
#define QUEUE_SIZE	8192


using namespace std;

inline static int findNextPositivePowerOfTwo(int value) {
	int i = 0;
	cout << "findNextPositivePowerOfTwo" << value << endl;
	while ( value>0) { ++i;value >>= 1;}
	int ret = 1 << (i-1);
	cout << "findNextPositivePowerOfTwo" << ret << endl;
	return ret;
}


#endif /* COMMON_H_ */
