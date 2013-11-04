#include "stdio.h"
#include "stdlib.h"
#include <CL/cl.h>
#include "logger.h"
#include "opencl_utils.h"
#include "value-profiler.h"

/**
 * Important design question that needs to be answered first
 * Where should the rule database be located ?
 */
void value_profiler::check_value_on_host(ad_rule rule)
{
	cl_int status;
	int mem_size;
	void * ptr = clEnqueueMapBuffer(access_queue,
						rule.get_target_buff(),
						CL_TRUE,NULL,
						rule.get_target_offset(),mem_size,
						0,NULL,NULL,&status);
	ad_errChk(status,"Snoop value on host", TRUE);
	rule.test_rule(ctx,access_queue);


}

void value_profiler::check_value_on_device(ad_rule rule)
{
	cl_mem dest;
	cl_int status;
	printf("Not Implemented ");
	exit(-1);
	//ad_setKernelArg(test_kernel,0,sizeof(cl_mem),(void *)&(rule.get_target_buff()));
	//! Action to be done
	//ad_setKernelArg(test_kernel,1,sizeof(cl_int),(void *)&(rule.get_target_buff()));
	clEnqueueTask(access_queue,test_kernel,0,NULL,NULL);

}
void value_profiler::record_result_on_host()
{

}
void value_profiler::record_result_on_device()
{

}

void value_profiler::set_buffer(cl_mem ip_buff, int ip_offset)
{
	result_buffer r;
	r.buffer = ip_buff;
	r.offset = ip_offset;
	locn.push_back(r);

}

inline bool value_profiler::apply_rule_exact(ad_rule ip)
{

	if(profiler_state == DISABLED)
	{
		print_warning("Profiler Disabled");
		return RULE_FAILURE;
	}
	else
	{
		float * target_ptr;
		cl_int status;

		target_ptr = (float *)clEnqueueMapBuffer(access_queue ,ip.get_target_buff(),
								CL_TRUE, CL_MAP_READ,
								0,ip.get_target_mem_size(),0,
								NULL,NULL,&status);
		//! Verify if this is needd
		clFinish(access_queue);
		ad_errChk(status,"error mapping buffer in value profiler");
		if(float(target_ptr[0]) == ip.get_target_value())
			return RULE_SUCCESS;
		else
			return RULE_FAILURE;
	}
}
/**
 * Rule to check if the data on the device is less than the target value
 */
inline bool value_profiler::apply_rule_less_than(ad_rule ip)
{

	if(profiler_state == DISABLED)
	{
		printf("Profiler Disabled");
		return RULE_FAILURE;
	}
	else
	{
		float * target_ptr;
		cl_int status;
		target_ptr = (float *)clEnqueueMapBuffer(access_queue,ip.get_target_buff(),
								CL_TRUE, CL_MAP_READ,
								0,ip.get_target_mem_size(),0,
								NULL,NULL,&status);
		ad_errChk(status,"error mapping buffer in value profiler");
		//! Verify if this is needd
		clFinish(access_queue);

		if(float(target_ptr[0]) <= ip.get_target_value())
			return RULE_SUCCESS;
		else
			return RULE_FAILURE;

	}
}


char * rule_result_msg[2] = {
	"RULE_FAILURE",  	// 0
    "RULE_SUCCESS",		//-1
};


char * value_profiler::stringify_rule_result(bool rule_status)
{
	return rule_result_msg[int (rule_status) ];

}
/**
 * Rule to check if the data on the device is more than the target value
 */
inline bool value_profiler::apply_rule_more_than(ad_rule ip)
{
	if(profiler_state == DISABLED)
	{
		print_warning("Profiler Disabled");
		return RULE_FAILURE;
	}
	else
	{
		float * target_ptr;
		cl_int status;
		printf("OPENCL BUFFER USED HAP CODE %p\n",ip.get_target_buff());
		target_ptr = (float *)clEnqueueMapBuffer(access_queue,ip.get_target_buff(),
								CL_TRUE, CL_MAP_READ,
								0,ip.get_target_mem_size(),0,
								NULL,NULL,&status);
		ad_errChk(status,"error mapping buffer in value profiler");
		status = clFinish(access_queue);
		ad_errChk(status,"error mapping buffer in value profiler");

		//printf("Value seen from memory %f and Target is %f \n", target_ptr[0],ip.get_target_value());
		//for(int i =0 ; i< 64;i++)
		//	printf("Value seen from memory mapping is %f \t Mapping %d \n",target_ptr[i],ip.get_target_mem_size());
		//getchar();

		if(float(target_ptr[0]) > ip.get_target_value())
			return RULE_SUCCESS;
		else
			return RULE_FAILURE;
	}
}


value_profiler::~value_profiler()
{

	free(wait_list);

}

value_profiler::value_profiler()
{
	print_logging("Default Constructor - Value profiler\n");
	//! Disable value_profiler by default since it is a high overhead operation
	//profiler_state = DISABLED;
	profiler_state = ENABLED;
	len_wait_list = 0;
	wait_list = (cl_event * )malloc(sizeof(cl_event)*MAX_LEN_WAITLIST);

}

void value_profiler::add_to_wait_list(int k, cl_event * ip_event)
{
	for(int i = 0; i<k; i++)
	{


	}

}

void value_profiler::set_kernel(char * filename, char * kernelname)
{
	cl_int status = CL_SUCCESS;
	cl_program test_program ;
	test_program = cl_CompileProgram(filename,NULL,TRUE,ctx,access_device);

 	kernel_object k = alloc_kernel_object();
 	k->dim_globalws = 2;
 	k->dim_localws = 2;
 	k->name = kernelname;
 	set_kernel_name(k,kernelname);
 	k->kernel = clCreateKernel(test_program,kernelname,&status);
	ad_errChk(status,"Creating Checking task",EXITERROR);

 	test_kernel_vec.push_back(k);



}

void value_profiler::init(cl_command_queue ip_queue, cl_context ip_ctx, cl_device_id ip_device )
{
	access_queue = ip_queue;
	ctx = ip_ctx;

	if(ip_device != NULL)
	{
		access_device = ip_device;
	}
	else
	{
		print_logging("Unknown Device passed to value profiler\n Cannot compile kernel");
		exit(-1);

	}
}

bool value_profiler::test_rule(ad_rule ip_rule)
{
	if(ip_rule.get_type() == RULE_TYPE_KERNEL)
	{
		print_warning("NOT Supported");
		exit(-1);
	}
	else
	{
		if(profiler_state == ENABLED)
		{
			if(ip_rule.get_type() == VALUE_EXACT)
			{
				if(apply_rule_exact(ip_rule ) )
					return RULE_SUCCESS;
				else
					return RULE_FAILURE;
			}
			if(ip_rule.get_type()  == VALUE_LESS_THAN)
			{
				if(apply_rule_less_than(ip_rule ))
					return RULE_SUCCESS;
				else
					return RULE_FAILURE;
			}
			if(ip_rule.get_type() == VALUE_MORE_THAN)
			{
				if(apply_rule_more_than(ip_rule ))
					return RULE_SUCCESS;
				else
					return RULE_FAILURE;
			}
			if(ip_rule.get_type() == UNDEFINED_RULE_TYPE)
			{
				print_logging("rule not defined yet, Cannot test");
				exit(-1);
			}
		}
		else
		{
			print_warning("Value profiler not enabled");
		}
	}
	print_logging("Should not get here");
	exit(-1);
	// Added this so the compiler doesnt moan
	return RULE_FAILURE;
}
