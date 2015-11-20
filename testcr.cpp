//////////////////////////////////////////////////////////////////////
// testcr.cpp -- Test the Coroutine Library
// Date: Thu Nov 19 20:56:54 2015  (C) Warren W. Gay VE3WWG 
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "pcoroutine.hpp"

static CR_Mutex mutex;

//////////////////////////////////////////////////////////////////////
// Coroutine 1
//////////////////////////////////////////////////////////////////////

void *
cr1(void *arg) {
	int sx = *(int *)arg;
	
	for ( int x=sx; x<sx+12; ++x ) {
		printf("cr1: x=%d\n",x);
		mutex.yield();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Coroutine 2
//////////////////////////////////////////////////////////////////////

void *
cr2(void *arg) {
	int sx = *(int *)arg;
	
	for ( int x=sx; x<sx+12; ++x ) {
		printf("cr2: x=%d\n",x);
		mutex.yield();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Coroutine 3
//////////////////////////////////////////////////////////////////////

void *
cr3(void *arg) {
	int sx = *(int *)arg;
	
	for ( int x=sx; x<sx+12; ++x ) {
		printf("cr3: x=%d\n",x);
		mutex.yield();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Coroutine 0 (main program)
//////////////////////////////////////////////////////////////////////

int
main(int argc,char **argv) {
	static const int sx1 = 14;
	static const int sx2 = 26;
	static const int sx3 = 95;
	PCoroutine cr01(mutex,cr1,(void *)&sx1), cr02(mutex,cr2,(void *)&sx2), cr03(mutex,cr3,(void *)&sx3);

	// Do our own print of a bunch of numbers
	for ( int x=100; x < 120; ++x ) {
		printf("mx = %d\n",x);
		mutex.yield();
	}

	cr01.join();
	cr02.join();
	cr03.join();

	return 0;
}

// End testcr.cpp
