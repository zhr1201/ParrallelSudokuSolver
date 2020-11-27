
#ifndef SUDOKU_UTILS_MUTEX_H_
#define SUDOKU_UTILS_MUTEX_H_

#include <pthread.h>

namespace sudoku {

class Mutex
{
public:
    Mutex() { pthread_mutex_init(&mutex_, NULL); }

    inline void Lock() { pthread_mutex_lock(&mutex_); }

    inline bool TryLock() { return pthread_mutex_trylock(&mutex_) == 0; }

    inline void Unlock() { pthread_mutex_unlock(&mutex_); }
 
private:
    friend class CondVariable;
    pthread_mutex_t mutex_;
};


class CondVariable {
public:
	CondVariable() {
		pthread_cond_init(&cond_, NULL);
	}
	~CondVariable() {
		pthread_cond_destroy(&cond_);
	}
	void Wait(Mutex& mutex) {
		pthread_cond_wait(&cond_, &mutex.mutex_);
	}

	void NotifyOne() {
		pthread_cond_signal(&cond_);
	}
	void NotifyAll() {
		pthread_cond_broadcast(&cond_);
	}
private:
	pthread_cond_t cond_;
};

}

#endif
