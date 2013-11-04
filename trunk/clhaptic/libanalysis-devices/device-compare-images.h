
#include <CL/cl.h>
#include "stdio.h"

#include "analysis-devices-utils.h"

#include "fissionutils.h"
#include "analysis-devices.h"
#include "ad_rule_vec.h"

#ifndef __COMPARE_DEVICE_
#define __COMPARE_DEVICE_



class compare_images : public analysis_device
{

private:

	float THRESHOLD;
	result_buffer opbuff;
	cl_mem p_img;
	cl_mem n_img;

	char * device_name;

	int feature_count;
	 //application_state;

public:
	//! Value profiler.
	value_profiler * v_profiler;


	compare_images();

	bool get_analysis_result();

	//! This function handles initial set up only
	void initialize_analysis_kernels();
	void init_buffers(size_t mem_size);
	void assign_buffers_copy(float * prev, float * next,size_t mem_size);

	//! Kernel Configuration function.
	//! Should be called before the analysis_device::inject_analysis() function
	void configure_analysis_kernel( int W, int H );

	void track_feature_count();

	//! No of features to act on.
	void set_feature_count_threshold(int k,cl_mem d_count);
	//! Not used.
	//! Has been disabled for now since the user cannot control when the
	//! analysis is launched exactly
	void launch_compare(
			cl_mem present_image, cl_mem next_image,
			int W, int H );

	void set_compare_threshold();
	void init_value_profiler();
};

#endif // __COMPARE_DEVICE_
