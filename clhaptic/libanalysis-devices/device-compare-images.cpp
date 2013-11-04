#include "math.h"
#include <eventlist.h>
#include "device-compare-images.h"

#include <CL/cl.h>

//enum image_similarity;

/**
 * Call base constructor too.
 */
compare_images::compare_images():analysis_device()
{
	printf("Derived Class - Compare Images Device\n");
	THRESHOLD = 149.0f;
	//! Initialize the profiler and the rule database.
	v_profiler = new value_profiler[1];
	ruledb = new ad_rule_vec[1];
}

void compare_images::init_value_profiler()
{
	v_profiler->init(getCommandQueue(),getContext());
}


void compare_images::set_compare_threshold()
{
	printf("Enter Value to threshold\n");
	scanf("%f",&THRESHOLD);
	printf("Set Threshold to %f\n",THRESHOLD);
}
void compare_images::init_buffers(size_t mem_size)
{

    cl_int status;
    p_img = clCreateBuffer(getContext(),
						CL_MEM_READ_WRITE,
						mem_size, NULL, &status);
    ad_errChk(status, "Error allocating pinned memory", true);
    n_img = clCreateBuffer(getContext(),
						CL_MEM_READ_WRITE ,
						mem_size, NULL, &status);
	ad_errChk(status, "Error allocating pinned memory", true);

	opbuff.allocate_buffer(mem_size,getContext());
}

/**
 * Assign data to the analysis device's buffers
 * @param prev Previous image
 * @param next Next image
 * @param mem_size Data size
 */
void compare_images::assign_buffers_copy(float * prev, float * next, size_t mem_size)
{
	//! Uses the cl_map calls to map the pointers passed to the
	//! buffer objects
 	copyHostToAd(p_img,prev,mem_size);
	copyHostToAd(n_img,next,mem_size);

	//p_img = NULL;
	//n_img = NULL;
}


void compare_images::track_feature_count()
{
	v_profiler->test_rule(ruledb->get_rule(0));

}


float get_rand(int low, int high)
{
  srand ( time(NULL) );
	float isecret;
  /* generate secret number: */
  float r = (float)rand()/(float)RAND_MAX;

  isecret = r*10.0 + 140.0;
  return isecret;

}
bool compare_images::get_analysis_result()
{
	bool return_state;
	//! Read results from processing

	sync();

	float * data = (float *)mapBuffer(opbuff.buffer, 10*sizeof(float),CL_MAP_READ);

	//cl_int status = CL_SUCCESS;
	//float *data = (float *)malloc(opbuff.mem_size);
	//status = clEnqueueReadBuffer(queue,opbuff.buffer,TRUE,0,opbuff.mem_size,data,0,NULL,NULL);


 	//sync();
 	float diff_value = 0.0f;
	//for(int i=0;i < (kernel_vec.at(0)->globalws[0]); i++)
 	int i;
	for(i=0; i < 10; i++)
	{
	//	printf("Data is %f \n", data[i]);
 		diff_value = diff_value + data[i];
	}


//	diff_value = get_rand(0,150);
	if(fabs(diff_value) > THRESHOLD)
		return_state = ENABLED;
	else
		return_state = DISABLED;
	//printf("Diff is %f \n",diff_value);
	return return_state;
}

//! Configure the analysis kernel.
//! At this stage the kernel should be allocated and compiled
//! \param p_img Present Image
//! \param p_img Next Image
void compare_images::configure_analysis_kernel( int W, int H )
{
//	printf("Setting Arguments and Config Analysis Kernel\n");

	kernel_vec.at(0)->dim_globalws = 2;
	kernel_vec.at(0)->dim_localws = 2;
	kernel_vec.at(0)->localws[0] = 16;
	kernel_vec.at(0)->localws[1] = 16;

	//printf("New ws size %d\n",new_ws);
	//kernel_vec.at(0)->globalws[0] = W;
	//kernel_vec.at(0)->globalws[1] = H;

	kernel_vec.at(0)->globalws[0] = idivup(W,16);
	kernel_vec.at(0)->globalws[1] = idivup(H,16);


	kernel_vec.at(0)->localmemsize = (sizeof(float)*(kernel_vec.at(0)->localws[0])*(kernel_vec.at(0)->localws[1]));

 	ad_setKernelArg(getKernel(0), 0,sizeof(cl_mem),(void *)&p_img);
	ad_setKernelArg(getKernel(0), 1,sizeof(cl_mem),(void *)&n_img);
	ad_setKernelArg(getKernel(0), 2,sizeof(cl_mem),(void *)&(opbuff.buffer));
	ad_setKernelArg(getKernel(0), 3,kernel_vec.at(0)->localmemsize, NULL);
	ad_setKernelArg(getKernel(0), 4,sizeof(cl_int), (void *)&W);
	ad_setKernelArg(getKernel(0), 5,sizeof(cl_int), (void *)&H);


}

void compare_images::set_feature_count_threshold(int k,cl_mem d_count)
{
	ad_rule threshold_rule;
	threshold_rule.add(VALUE_MORE_THAN,d_count,float(k),sizeof(float));
	ruledb->add_rule(threshold_rule);
}



//! Input- Feature set
//! Output- Sorted set (sorted as per all Xcord followed by all Ycoord)
/*
int sort_features()
{


}

int orntn_check()
{
	cl_mem buff1;
	cl_mem buff2;
	cl_kernel ortn_compare;
	//! Get Feature Sets
	cl_setKernelArg()

	//! Get Feature Set2


}
*/

