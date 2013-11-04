#include "stdio.h"
#include "opencl_utils.h"
#include "ad_rule.h"
#include "rule_type.h"
#include "logger.h"

//! See documentation on why, inline has been removed

void ad_rule::test_rule_kernel(cl_context ip_ctx, cl_command_queue ip_queue, cl_kernel ip_kernel,
				size_t * localws, size_t * globalws, int n_dims)
{

	cl_int status  = CL_SUCCESS;
	status =  clEnqueueNDRangeKernel(ip_queue,ip_kernel,n_dims,localws,globalws,NULL,0,NULL,NULL);
	ad_errChk(status,"Error in monitoring kernel");

}


bool ad_rule::test_rule(cl_context ctx, cl_command_queue queue)
{
	printf("WARNING\nThis shouldnt be used  with the profilers\n");
	printf("Killing Program Here");
	exit(-1);
	if(type == VALUE_EXACT)
	{
		if(apply_rule_exact(ctx,queue) )
			return RULE_SUCCESS;
		else
			return RULE_FAILURE;
	}
	if(type == VALUE_LESS_THAN)
	{
		if(apply_rule_less_than(ctx,queue))
			return RULE_SUCCESS;
		else
			return RULE_FAILURE;
	}

	if(type == VALUE_MORE_THAN)
	{
		if(apply_rule_more_than(ctx,queue))
			return RULE_SUCCESS;
		else
			return RULE_FAILURE;
	}
	if(type == UNDEFINED_RULE_TYPE)
	{
		printf("rule not defined yet");
		exit(-1);
	}
	printf("SHOUldnt reach here\n");
	exit(-1);
	return RULE_FAILURE;
}


bool ad_rule::apply_rule_less_than(cl_context ctx, cl_command_queue queue)
{
	cl_int status;

	target_ptr = clEnqueueMapBuffer(queue,target_buffer,
							CL_TRUE, CL_MEM_READ_ONLY,
							0,mem_size,0,
							NULL,NULL,&status);
	ad_errChk(status,"error mapping buffer");

}

bool ad_rule::apply_rule_more_than(cl_context ctx, cl_command_queue queue)
{
	cl_int status;
	target_ptr = clEnqueueMapBuffer(queue,target_buffer,
							CL_TRUE, CL_MEM_READ_ONLY,
							0,mem_size,0,
							NULL,NULL,&status);
	ad_errChk(status,"error mapping buffer");

}


bool ad_rule::apply_rule_exact(cl_context ctx, cl_command_queue queue)
{
	cl_int status;

	target_ptr = clEnqueueMapBuffer(queue,target_buffer,
							CL_TRUE, CL_MEM_READ_ONLY,
							0,mem_size,0,
							NULL,NULL,&status);

	ad_errChk(status,"error mapping buffer");
}


ad_rule::ad_rule()
{
	type = UNDEFINED_RULE_TYPE;
}



//inline
rule_type ad_rule::get_type()
{
	return type;
}

//inline
cl_mem ad_rule::get_target_buff()
{
	return target_buffer;
}

int ad_rule::get_target_offset()
{
	return buffer_offset;
}
//inline
float ad_rule::get_target_value()
{
	return target_value;
}

//inline
size_t ad_rule::get_target_mem_size()
{
	return mem_size;
}

void ad_rule::add(rule_type ip_t, cl_mem ip_buff, float ip_value,int ip_mem_size)
{
	target_buffer= ip_buff ;
	type = ip_t;
	target_value = ip_value;
	mem_size = ip_mem_size;

}

void ad_rule::add(rule_type t)
{
	type = RULE_TYPE_KERNEL;
}


char * rule_type_msg[MAX_NUM_RULES] = {
	"VALUE_MORE_THAN",  	// 0
    "VALUE_LESS_THAN",		//-1
    "VALUE_EXACT",          //-2
    "RULE_TYPE_KERNEL"		//-3
};

char * ad_rule::stringify_rule_type(rule_type t)
{
	return rule_type_msg[t - RULE_TYPE_BASE_NO];
}

void ad_rule::print_rule_details()
{
	if(type == RULE_TYPE_KERNEL)
	{
		print_logging("Added a RULE \t Type");
		print_logging(stringify_rule_type(type));
		print_logging("No other details in RuleDB ");
	}
	else
	{
		printf("Added a RULE \t Type %s \t Target Value \t %f \n",stringify_rule_type(type),target_value);
		//print_logging("Added a RULE \t Type");
		//print_logging(stringify_rule_type(type));
		//print_logging("No other details in RuleDB ");

	}
}
