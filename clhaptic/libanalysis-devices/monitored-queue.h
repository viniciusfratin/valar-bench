#ifndef __MONITORED_QUEUE_
#define __MONITORED_QUEUE_

#include <CL/cl.h>
#include "kernel-object.h"
#include "analysis-devices.h"

class monitored_queue
{

private:
	//! Parameter defining ratio of BATCH vs DELTA queues

	//! Possible length of queue
	int batch_queue_len;
	//!
	int delta_queue_len;
public:
	cl_int init();
	//! Analysis device for the monitored_queue
	analysis_device * a_device;

	//! The compute device
	cl_device_id c_device;
	cl_command_queue queue;

	cl_int push_compute(kernel_object k);
	void assign_compute_device(cl_device_id ip_device);
	cl_command_queue compute_queue;

};

#endif // __MONITORED_QUEUE_
