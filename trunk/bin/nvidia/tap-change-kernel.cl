
#ifndef __OPENCL_VERSION__

#define __kernel
#define __global
#define get_global_id

#endif


__kernel void tap_change_kernel(
		__global float * input_tap,
		int n_iterations, int n_tap)
{

	float old_tap = input_tap[get_global_id(0)];
	float new_tap = input_tap[get_global_id(0)]/200.0f;
	for(int m =0 ; m < n_iterations; m++)
		for(int k=0; k < n_iterations; k++)
		{
			new_tap = new_tap + asin((float)m)*old_tap*( 1.0f / (100.0f*100.0f*100.0f) );
			new_tap = new_tap + asin((float)k)*old_tap*( 1.0f / (100.0f*100.0f*100.0f) );
		}
	input_tap[get_global_id(0)] = new_tap;
}
		
