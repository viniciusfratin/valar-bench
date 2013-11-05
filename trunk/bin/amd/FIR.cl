/*!
 * Calculate a FIR filter
 * one work group, and the number of work items is the number of output points
 */

#ifndef __OPENCL_VERSION__
#define __constant
#define __kernel
#define __global
#define get_global_id
#define get_global_size
#define barrier( CLK_GLOBAL_MEM_FENCE );


#endif

__kernel void FIR( __global       float * output,
			  __constant     float * coeff,       	/* numFilter sets of coeff, each is numTap long */
			  __global       float * temp_input,    /* numFilter sets of temp data, each
													is (numData+numTap-1) long,
													the first (numTap-1) data is actually the history data */
                              uint numTap){
#if 1
    uint tid = get_global_id(0);
    uint numData = get_global_size(0);
    uint xid = tid + numTap - 1;

    float sum = 0;
    uint i=0;

    for( i=0; i<numTap; i++ )
    {
        sum = sum + coeff[i] * temp_input[tid + i];
     
    }
    output[tid] = sum;

    barrier( CLK_GLOBAL_MEM_FENCE );

    /* fill the history buffer */
    if( tid >= numData - numTap + 1 )
        temp_input[ tid - ( numData - numTap + 1 ) ] = temp_input[xid];

    barrier( CLK_GLOBAL_MEM_FENCE );
#endif
}
