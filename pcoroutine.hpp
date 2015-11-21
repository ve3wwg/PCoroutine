//////////////////////////////////////////////////////////////////////
// pcoroutine.hpp -- Portable Coroutine Library (for testing)
// Date: Thu Nov 19 20:00:21 2015   (C) Warren Gay ve3wwg
///////////////////////////////////////////////////////////////////////
// 
// This is a POSIX portable class to simulate cooperative
// coroutines using the pthreads library. All PCoroutine
// objects using a common CR_Mutex object, will behave as
// if single threaded. Only one PCoroutine launched thread
// will run at any one time.
// 
// When a coroutine calls it's CR_Mutex::yield() method (or
// PCoroutine::yield()) the next coroutine in round robin
// fashion will be permitted to execute.
// 
// This is not designed for efficiency. It is designed to allow
// testing of MCU configurations that run as coroutines in
// one thread of execution. In this manner, testing on POSIX
// platforms can be debugged and identify yield problems and
// opportunities.
///////////////////////////////////////////////////////////////////////
// LICENSED UNDER THE LGPL 2
///////////////////////////////////////////////////////////////////////

#ifndef PCOROUTINE_HPP
#define PCOROUTINE_HPP

#include <vector>
#include <pthread.h>

class PCoroutine;	// Forward declaration


//////////////////////////////////////////////////////////////////////
// Coroutine Mutex to prevent more than one thread running at one
// time (i.e. simulate a real cooperative thread system)
//////////////////////////////////////////////////////////////////////

class CR_Mutex {
	friend class PCoroutine;

	bool		preemptivef;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	pthread_t	exec;
	std::vector<pthread_t> threads;

	void release();	// Used by exiting threads only

public:	CR_Mutex(bool preemptive_sched=false);
	~CR_Mutex();

	bool is_preemptive() const { return preemptivef; }

	void yield();
};


//////////////////////////////////////////////////////////////////////
// pthreads simulated coroutine
//////////////////////////////////////////////////////////////////////

class PCoroutine {
	void		*arg;
	void *		(*func)(void *arg);

	pthread_t	id;
	pthread_attr_t	attr;
	CR_Mutex&	mutex;

public:	PCoroutine(CR_Mutex& mutex,void *(*func)(void *arg),void *arg,size_t stack_size=4*1024*1024);
	~PCoroutine();

	void start();				// Start the coroutine
	inline void yield() { mutex.yield(); }	// Yield round robin
	void *join();				// Join
};


#endif // PCOROUTINE_HPP

// End pcoroutine.hpp
