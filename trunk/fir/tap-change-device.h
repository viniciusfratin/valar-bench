#ifndef __TAP_CHANGE_DEVICE__
#define __TAP_CHANGE_DEVICE__

#include "analysis-devices.h"
#include "opencl_utils.h"
#include "ad_rule_vec.h"

class tap_change_device : public analysis_device
{

public:

	float threshold_value ;
	int change_interval;
	int num_taps;

	//! Amount of time taken by tap_change_device kernels
	int num_iterations;
	cl_mem coeff_buff;

	cl_mem input_buffer;

	value_profiler * v_profiler;
	ad_rule_vec * ruledb;

	tap_change_device();
	~tap_change_device();

	void add_phase(int ip_phase_id);
	void check_value();

	//! Set up the value checker to see if the threshold for a set of OP signals was crossed
	void init_value_checker(cl_command_queue ip_queue, cl_context ip_ctx, cl_device_id ip_device);
	void init_tap_change_device(cl_context ip_ctx, int , int);

	//! Function to set the value.
	void set_threshold(float value , cl_mem input_buffer, int offset );

	void configure_analysis_kernel();

	//! Add waiting requirements, overrides analysis_device::resolve_waiting
	void resolve_waiting();

};

#endif // __TAP_CHANGE_DEVICE__
