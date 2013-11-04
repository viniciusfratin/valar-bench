
#include <CL/cl.h>

// The cl_time type is OS specific
//! Create a type for the time to avoid confusion
typedef cl_ulong cl_time;



//! Grab the current time using a system-specific timer
void getTime(cl_time * time);


