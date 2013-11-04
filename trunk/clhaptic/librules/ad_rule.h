#ifndef __ANALYSIS_RULE__
#define __ANALYSIS_RULE__

#include <CL/cl.h>
#include <vector>

#include "opencl_utils.h"
#include "rule_type.h"

//!
//! \brief Define a rule that is checked by an analysis-device object.
//! The OpenCL device specific things like cl_context and a
//! cl_command_queue are passed to the object.
//! Private functions added since high level code, may not know exact rule.
//! The functions apply_rule_exact() and so on are going to be usually unused
//! since the rule only stores information about what should be done.
//! We let the value profiler decide the best OpenCL method to do it.
class ad_rule
{
private:

	//! The type of test that needs to be done.
	rule_type type;
	//! OpenCL Buffer. The location of the non architectural event
	cl_mem target_buffer;
	float target_value;
	void * target_ptr;
	int buffer_offset;

	bool rule_result;

	//! Added for checking more than one value
	size_t mem_size;

	//! These will mostly not be used (See ad_rule )
	bool inline apply_rule_exact(cl_context , cl_command_queue );
	bool inline apply_rule_less_than(cl_context , cl_command_queue );
	bool inline apply_rule_more_than(cl_context , cl_command_queue );


public:

	char * stringify_rule_type(rule_type t);
	ad_rule();
	// Note that the inlining in this case wont work unless you
	// move the implementation of the function to the header because, gcc cannot see the code to inline it here.
	// For more details, google "inline function  used but never defined"
	rule_type get_type();
	float get_target_value();
	int get_target_offset();
	cl_mem get_target_buff();
	size_t get_target_mem_size();

	//! Stub for adding the "kernel" type rules
	void add(rule_type t);
	void add(rule_type, cl_mem, float val, int mem_size);
	//! Interface to call all the apply_ functions
	void print_rule_details();
	bool test_rule(cl_context ctx, cl_command_queue queue);
	void test_rule_kernel(cl_context ip_ctx, cl_command_queue ip_queue, cl_kernel ip_kernel,
			size_t * localws, size_t * globalws, int n_dims);
};


#endif
