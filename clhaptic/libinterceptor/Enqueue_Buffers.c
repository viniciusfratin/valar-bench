#define _GNU_SOURCE

#include "cl_pm.h"

#include <stdio.h>
#include <dlfcn.h>

//typedef  cl_bitfield cl_mem_flags;

/**
    OpenCL calls interception
    export LD_PRELOAD='/usr/lib/libdl.so ./myOpenCL.so'
**/

#include "interceptor.h"


/** Enqueued Commands APIs **/
//extern CL_API_ENTRY
cl_int //CL_API_CALL
 clEnqueueReadBuffer(cl_command_queue  command_queue ,
                    cl_mem               buffer ,
                    cl_bool              blocking_read ,
                    size_t               offset ,
                    size_t               cb ,
                    void *               ptr ,
                    cl_uint              num_events_in_wait_list ,
                    const cl_event *     event_wait_list ,
                    cl_event *           event )  //CL_API_SUFFIX__VERSION_1_0;

{

    cl_int error_code = NOT_FOUND;
    printf("Calling Perhaad's clEnqueueReadBuffer\t Status \t %d\n",error_code );
    static  cl_int (*realclEnqueueReadBuffer)
                    (cl_command_queue  command_queue ,
                    cl_mem               buffer ,
                    cl_bool              blocking_read ,
                    size_t               offset ,
                    size_t               cb ,
                    void *               ptr ,
                    cl_uint              num_events_in_wait_list ,
                    const cl_event *     event_wait_list ,
                    cl_event *           event ) ;

    char * func_error;
    if (!realclEnqueueReadBuffer)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclEnqueueReadBuffer = dlsym(RTLD_NEXT, "clEnqueueReadBuffer");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }

    error_code = realclEnqueueReadBuffer(command_queue ,
                    buffer , blocking_read ,offset , cb , ptr ,
                    num_events_in_wait_list, event_wait_list ,event ) ;


    printf("real clEnqueueReadBuffer Called \t Status \t %d\n",error_code );
    return error_code ;

}


//extern CL_API_ENTRY
cl_int //CL_API_CALL
clEnqueueWriteBuffer(cl_command_queue   command_queue ,
                     cl_mem             buffer ,
                     cl_bool            blocking_write ,
                     size_t             offset ,
                     size_t             cb ,
                     const void *       ptr ,
                     cl_uint            num_events_in_wait_list ,
                     const cl_event *   event_wait_list ,
                     cl_event *         event ) //CL_API_SUFFIX__VERSION_1_0;
{

    cl_int error_code = NOT_FOUND;
    printf("Calling Perhaad's clEnqueueWriteBuffer\t Status \t %d\n",error_code );
    static  cl_int (*realclEnqueueWriteBuffer)
                    (cl_command_queue  command_queue ,
                    cl_mem               buffer ,
                    cl_bool              blocking_write ,
                    size_t               offset ,
                    size_t               cb ,
                    const void *               ptr ,
                    cl_uint              num_events_in_wait_list ,
                    const cl_event *     event_wait_list ,
                    cl_event *           event ) ;

    char * func_error;
    if (!realclEnqueueWriteBuffer)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclEnqueueWriteBuffer = dlsym(RTLD_NEXT, "clEnqueueWriteBuffer");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }

    error_code = realclEnqueueWriteBuffer(command_queue ,
                    buffer , blocking_write ,offset , cb , ptr ,
                    num_events_in_wait_list, event_wait_list ,event ) ;


    printf("real realclEnqueueWriteBuffer Called \t Status \t %d\n",error_code );
    return error_code ;

}
