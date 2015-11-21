//////////////////////////////////////////////////////////////////////
// pcoroutine.cpp -- Implementation of Coroutine Library (for testing)
// Date: Thu Nov 19 20:04:21 2015  (C) Warren W. Gay VE3WWG 
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "pcoroutine.hpp"

//////////////////////////////////////////////////////////////////////
// Coroutine Mutex Class CR_Mutex
//////////////////////////////////////////////////////////////////////

CR_Mutex::CR_Mutex(bool preemptive_sched) : preemptivef(preemptive_sched) {
	int rc;

	rc = pthread_mutex_init(&mutex,0);
	assert(!rc);
	rc = pthread_cond_init(&cond,0);
	assert(!rc);
	exec = 0;
}

CR_Mutex::~CR_Mutex() {
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

void
CR_Mutex::release() {
	pthread_t self = pthread_self();
	size_t ux;
	int rc;

	rc = pthread_mutex_lock(&mutex);
	assert(!rc);
	assert(exec == self);

	for ( ux=0; ux<threads.size(); ++ux )
		if ( threads[ux] == self )
			break;

	assert(ux < threads.size());	
	for ( ; ux + 1 < threads.size(); ++ux )
		threads[ux] = threads[ux+1];
	threads.resize(threads.size()-1);

	exec = 0;
	rc = pthread_mutex_unlock(&mutex);
	assert(!rc);
	rc = pthread_cond_signal(&cond);
	assert(!rc);
}

void
CR_Mutex::yield() {
	if ( preemptivef )
		return;

	pthread_t self = pthread_self();
	size_t ux;
	int rc;

	rc = pthread_mutex_lock(&mutex);
	assert(!rc);

	for ( ux=0; ux<threads.size(); ++ux )
		if ( threads[ux] == self )
			break;
	if ( ux >= threads.size() )
		threads.push_back(self);

	if ( exec == self ) {
		// Schedule next thread
		exec = threads[(ux + 1) % threads.size()];

		rc = pthread_mutex_unlock(&mutex);
		assert(!rc);
		rc = pthread_cond_broadcast(&cond);
		assert(!rc);
		rc = pthread_mutex_lock(&mutex);
		assert(!rc);
	}

	for (;;) {
		if ( !exec || exec == self ) {
			exec = self;
			pthread_mutex_unlock(&mutex);
			return;
		}
		pthread_cond_wait(&cond,&mutex);
	}
}

//////////////////////////////////////////////////////////////////////
// A thunk routine to invoke PCoroutine::start() for the new thread
//////////////////////////////////////////////////////////////////////

static void *
thunk(void *arg) {
	PCoroutine *obj = (PCoroutine *)arg;

	obj->start();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// pthreads coroutine implementation
//////////////////////////////////////////////////////////////////////

PCoroutine::PCoroutine(CR_Mutex& mutex,void *(*func)(void *arg),void *arg,size_t stack_size) : mutex(mutex) {
	int rc;
	
	this->arg = arg;
	this->func = func;

	rc = pthread_attr_init(&attr);
	assert(!rc);
	rc = pthread_attr_setstacksize(&attr,stack_size);
	assert(!rc);
	
	mutex.yield();
	rc = pthread_create(&id,&attr,thunk,this);
	assert(!rc);
	mutex.yield();
}

PCoroutine::~PCoroutine() {
	pthread_attr_destroy(&attr);
}

void
PCoroutine::start() {
	if ( !mutex.is_preemptive() )
		mutex.yield();
	this->func(this->arg);
	if ( !mutex.is_preemptive() )
		mutex.release();
}	

void *
PCoroutine::join() {
	void *retval = 0;

	pthread_join(id,&retval);
	return retval;
}

// End pcoroutine.cpp
