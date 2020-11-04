#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>
#include <unistd.h>

namespace sudoku {

class Timer {
public:

	Timer() {
		Reset();
	}

	void Reset() {
		gettimeofday(&time_start_, &time_zone_);
	}

	double Elapsed() {
		struct timeval time_end;
		gettimeofday(&time_end, &time_zone_);
		double t1, t2;
		t1 =  (double)time_start_.tv_sec + (double)time_start_.tv_usec / (1000 * 1000);
		t2 =  (double)time_end.tv_sec + (double)time_end.tv_usec / (1000 * 1000);
		return t2 - t1;
	}

private:
	struct timeval time_start_;
	struct timezone time_zone_;
};

inline void SleepSeconds(double sec) { 
	usleep(sec * 1000000);
}

}

#endif // _TIMER_H_