
#include "monitored-queue.h"
#include "opencl_utils.h"
#include "kernel-object.h"

cl_int monitored_queue::push_compute(kernel_object compute)
{


	cl_uint status;
	cl_event e;
	status = clEnqueueNDRangeKernel(queue ,
			   compute->kernel, compute->dim_globalws,
			   NULL ,NULL,
			   compute->localws ,
			   compute->len_waitlist, compute->waitlist ,
			   &e );
	a_device->app_profiler->add(e);

	a_device->test_all_analysis_rule();
}


cl_int monitored_queue::init()
{
	//! THis is wrong
	a_device = new analysis_device;
}

void monitored_queue::assign_compute_device(cl_device_id ip_device)
{

}
