/*
 * eventtypes.h
 *
 * This file added by Perhaad since the event formats used
 * here have more application outside this class too
 *
 */

#ifndef EVENTTYPES_H_
#define EVENTTYPES_H_

#include "opencl_utils.h"


#define DEFAULT_TIME_COMPARE_THRESHOLD 10000

#include "timer.h"

//! Structure for user event information (we can't use OpenCL user
//! events because they don't support profiling information)
struct _cl_user_event {
	cl_ulong queued;
	cl_ulong submitted;
	cl_ulong start;
	cl_ulong end;
};
typedef _cl_user_event* cl_user_event;


//! Structure for holding information for each OpenCL event
struct _event_triple {
	cl_event event;
	const char* name;
	const char* type;
};
typedef _event_triple* event_triple;

//! Structure for holding information for each user event
struct _user_event_triple {
	cl_user_event event;
	const char* name;
	const char* type;
};
typedef _user_event_triple* user_event_triple;


#endif /* EVENTTYPES_H_ */
