#include "stdio.h"
#include "timer.h"
#include "sys/time.h"

//! Grab the current time using a system-specific timer
void getTime(cl_time* time)
{

#ifdef _WIN32
    int status = QueryPerformanceCounter((LARGE_INTEGER*)time);
    if(status == 0) {
        perror("QueryPerformanceCounter");
        exit(-1);
    }
#else
    // Use gettimeofday to get the current time
    struct timeval curTime;
    gettimeofday(&curTime, NULL);

    // Convert timeval into double
    *time = curTime.tv_sec * 1000 + (double)curTime.tv_usec/1000;
#endif
}

