#include <CL/cl.h>
#include "FIR_tapchange.h"

#include "analysis-devices.h"
#include "tap-change-device.h"
#include "logger.h"

void tap_change_device::resolve_waiting()
{
	print_warning("Waiting on Stuff to Finish");
	clFinish(cl_getfircq());
}

tap_change_device::tap_change_device()
{
	change_interval = UNKNOWN;
	v_profiler = new value_profiler[1];
	ruledb = new ad_rule_vec[1];

}

void tap_change_device::set_threshold(float ip_value , cl_mem ip_buffer, int offset)
{
	threshold_value = ip_value;
	input_buffer = ip_buffer;

	ad_rule value_monitor_rule;
	value_monitor_rule.add(VALUE_MORE_THAN,input_buffer,float(threshold_value), 64*sizeof(float));
	ruledb->add_rule(value_monitor_rule);

}

tap_change_device::~tap_change_device()
{
	delete v_profiler;
	delete ruledb;
}

void tap_change_device::init_value_checker(cl_command_queue ip_queue, cl_context ip_ctx, cl_device_id ip_device)
{
	v_profiler->init(ip_queue,ip_ctx,ip_device);

//	v_profiler->set_kernel("sum-op-kernel.cl","sum_op_kernel");
//	ad_rule value_monitor_kernel_rule;
//	value_monitor_kernel_rule.add(RULE_TYPE_KERNEL);
//	value_monitor_kernel_rule.print_rule_details();
//	ruledb->add_rule(value_monitor_kernel_rule);

}


void tap_change_device::check_value()
{

	bool result = v_profiler->test_rule(ruledb->get_rule(0));
	print_logging("Result of Rule");
	print_logging(v_profiler->stringify_rule_result(result));

}

void tap_change_device::add_phase(int ip_phase_id)
{
	app_profiler->simple_enqueue_phase(ip_phase_id);
	if( (app_profiler->get_latest_phase_id()) % change_interval == 0)
	{
		print_logging("\t\t\tEnable Device\n");
		set_device_state(ENABLED);
	}
	else
	{
		print_logging("\t\t\tDisable Device\n");
		set_device_state(DISABLED);
	}
}

//! Setup device
void tap_change_device::init_tap_change_device(cl_context ip_ctx, int ip_n_taps, int ip_n_iterations)
{
	num_taps = ip_n_taps;
	num_iterations = ip_n_iterations;
	//change_interval = 10;
	//! Need to pass on the OpenCL Context from the application

	#ifdef ANALYSIS_GPU
	configure_analysis_device_gpu(ip_ctx);
	#endif

	#ifdef ANALYSIS_CPU
	configure_analysis_device_cpu(ip_ctx);
	#endif

	cl_int status;

	coeff_buff = clCreateBuffer(ip_ctx,CL_MEM_READ_WRITE,num_taps*sizeof(float),NULL,&status);
	ad_errChk(status,"error declaring buffers");

}

//! Configure the analysis kernel.
//! At this stage the kernel should be allocated and compiled
void tap_change_device::configure_analysis_kernel()
{
	print_logging("Setting Arguments and Config Analysis Kernel");

	kernel_vec.at(0)->dim_globalws = 1;
	kernel_vec.at(0)->dim_localws = 1;
	kernel_vec.at(0)->localws[0] = 64;
	kernel_vec.at(0)->globalws[0] = num_taps;

	//kernel_vec.at(0)->localmemsize = (sizeof(float)*(kernel_vec.at(0)->localws[0])*(kernel_vec.at(0)->localws[1]));

 	ad_setKernelArg(getKernel(0), 0,sizeof(cl_mem),(void *)&coeff_buff);
	ad_setKernelArg(getKernel(0), 1,sizeof(cl_int), (void *)&num_iterations);
	ad_setKernelArg(getKernel(0), 2,sizeof(cl_int), (void *)&num_taps);

}
