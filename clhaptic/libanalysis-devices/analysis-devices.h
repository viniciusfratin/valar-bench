#ifndef __ANALYSIS_DEVICE_
#define __ANALYSIS_DEVICE_

#include <CL/cl.h>
#include "analysis-devices-utils.h"
#include "kernel-object.h"
#include "fissionutils.h"
#include "ad_rule_vec.h"
#include "eventlist.h"
#include "value-profiler.h"

#define ANALYSIS_APPLIED 0010

#define ANALYSIS_NOT_APPLIED 0011

#define FISSION_SUBDEVICE 0020

#define ROOT_DEVICE 0021

typedef cl_uint system_state;


typedef cl_uint device_type;

/**
 * A general analysis device base class.
 * Different analysis devices can be derived from this
 * class with their own kernels and so on. \n
 * This class includes all the architectural state needed to define a device \n
 * Derived classes only need to define the kernels and the buffers.
**/
class analysis_device
{

private:

	char device_name[128];
	//! Whether the analysis_device is ENABLED or DISABLED
	bool device_state;

	//! Used OpenCL context. This should be the same context as the
	cl_context context;
	//! Device ID of the analysis device
	cl_device_id device;
	cl_program analysis_program;


	//! Analysis Device is a Fission Topology
	fission_topology * topo;
	//! Kernel declaration made protected.
	//! Since specific inherited case needs to set args

	int n_analysis_kernels;

	system_state application_state;

	device_type opencl_arch;

protected:

	//! Read an OpenCL buffer and return pointer to host
	void * read_global_mem_counter(cl_mem buff);

	std::vector<kernel_object> kernel_vec;

	//! Derived classes need to configure these
	cl_event * analysis_waitlist;
	cl_uint len_analysis_waitlist;

	//! Add python script's parameters
	int frequency;

public:

	//! This could map to any OpenCL device or subdevice
	cl_command_queue queue;


	//!Adding the resolve waiting
	virtual void resolve_waiting();

	//! A rule database
	ad_rule_vec * ruledb;

	void test_all_analysis_rule();

	void set_device_state(bool);
	bool get_device_state();

	//! Constructor
	analysis_device();

	void * mapBuffer(cl_mem mem, size_t mem_size, cl_mem_flags flags);

	void configure_analysis_subdevice_cpu(char * device_name = NULL);

	//! Set up Fission, create root context and subqueues
	void configure_analysis_rootdevice();

	void configure_analysis_device_cpu(cl_context );
	void configure_analysis_device_gpu(cl_context );



	void copyHostToAd( cl_mem buff, void * mem, size_t mem_size);
	//! Return the kernel object
	cl_kernel getKernel(int k);

	virtual ~analysis_device();

	//! Profiler for analysis device.
	//! Handles timing and the insertion of kernels
	EventList * profiler;

	//! Profiler for application
	//! Handles timing and the insertion of kernels
	EventList * app_profiler;

	//!Removed this declaration
	//result_buffer result;

	void alloc_kernel_mem(int k);

	//! Compile kernels for device
	void build_analysis_kernel(char * filename, char * kernel_name, int pos);
	//! Will be called by function defined in the derived class
	void inject_analysis(int kernel_to_inject = UNKNOWN);

	char * generate_kernel_path(char * filename);
	cl_context getContext();
	cl_command_queue getCommandQueue();
	//! Create a new profiler object
	void init_app_profiler(EventList * profiler_locn = NULL);

	void sync();

 };

#endif //__ANALYSIS_DEVICE_
