#ifndef __KERNEL_OBJECT_
#define __KERNEL_OBJECT_

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <CL/cl.h>


//! A kernel object containing all the necessary parameters to launch a kernel.
//! Dont add a cl_command_queue here because we dont know what device kernel gets thrown on
struct _kernel_object
{
	cl_kernel kernel;
	//! _kernel_object::localws and globalws hardwired to 3. Max dim of OpenCL kernels
	size_t dim_localws;
	size_t dim_globalws;
	//!_kernel_object::localws and _kernel_object::globalws are hard wired to 3. Max dim of OpenCL kernels
	size_t localws[3];
	size_t globalws[3];
	char * name;
	int n_args;

	size_t localmemsize;
	//! Waitlist for kernel. UNUSED for now
	cl_event * waitlist;

	//! Num of elements in waitlist UNUSED for now
	int len_waitlist;
    const size_t *    global_work_offset ;


};

//! Typedef in a similar manner as things like cl_mem
typedef _kernel_object * kernel_object;

kernel_object alloc_kernel_object();

void set_kernel_name(_kernel_object * k, char * name);

#endif //__KERNEL_OBJECT_
