#define _GNU_SOURCE

#include "cl_pm.h"

#include "interceptor.h"

#include <stdio.h>
#include <dlfcn.h>
//#include <vector>

//!Global State for profiler
//class eventmodule
//{
  //  public:
    //    std::vector<cl_event> profiler_state;

//};
//extern CL_API_ENTRY


#if 0
//extern CL_API_ENTRY
cl_int //CL_API_CALL
 clEnqueueNDRangeKernel(cl_command_queue  command_queue ,
                       cl_kernel         kernel ,
                       cl_uint           work_dim ,
                       const size_t *    global_work_offset ,
                       const size_t *    global_work_size ,
                       const size_t *    local_work_size ,
                       cl_uint           num_events_in_wait_list ,
                       const cl_event *  event_wait_list ,
                       cl_event *        event )
                       //CL_API_SUFFIX__VERSION_1_0;
{
    cl_int ecode = NOT_FOUND;

    static  cl_int
    (*realclEnqueueNDRangeKernel)
                    (    cl_command_queue  command_queue ,
                        cl_kernel         kernel ,
                        cl_uint           work_dim ,
                        const size_t *    global_work_offset ,
                        const size_t *    global_work_size ,
                        const size_t *    local_work_size ,
                        cl_uint           num_events_in_wait_list ,
                        const cl_event *  event_wait_list ,
                        cl_event *        event );

    char * func_error;
    if (!realclEnqueueNDRangeKernel)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclEnqueueNDRangeKernel =   dlsym(RTLD_NEXT, "clEnqueueNDRangeKernel");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }

    ecode  = realclEnqueueNDRangeKernel(command_queue, kernel ,
                       work_dim , global_work_offset , global_work_size ,local_work_size ,
                       num_events_in_wait_list, event_wait_list , event );


    printf("real realclEnqueueNDRangeKernel Called \t Status \t %d\n",ecode);
    return ecode;

}

#endif
//extern CL_API_ENTRY
 cl_int //CL_API_CALL
 clEnqueueNDRangeKernel(cl_command_queue  command_queue ,
                       cl_kernel         kernel ,
                       cl_uint           work_dim ,
                       const size_t *    global_work_offset ,
                       const size_t *    global_work_size ,
                       const size_t *    local_work_size ,
                       cl_uint           num_events_in_wait_list ,
                       const cl_event *  event_wait_list ,
                       cl_event *        event )
                       //CL_API_SUFFIX__VERSION_1_0;
{
    cl_int ecode = NOT_FOUND;
    printf("intercept\n");
    static  cl_int
    (*realclEnqueueNDRangeKernel)
                    (    cl_command_queue  command_queue ,
                        cl_kernel         kernel ,
                        cl_uint           work_dim ,
                        const size_t *    global_work_offset ,
                        const size_t *    global_work_size ,
                        const size_t *    local_work_size ,
                        cl_uint           num_events_in_wait_list ,
                        const cl_event *  event_wait_list ,
                        cl_event *        event );

    /*
    I dont have a clue why this works...Dana ??
    http://stackoverflow.com/questions/5208476/function-that-returns-value-from-dlsym
    */

    char * func_error;
    if (!realclEnqueueNDRangeKernel)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	*(void **)&realclEnqueueNDRangeKernel =   dlsym(RTLD_NEXT, "clEnqueueNDRangeKernel");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }

    ecode  = realclEnqueueNDRangeKernel(command_queue, kernel ,
                       work_dim , global_work_offset , global_work_size ,local_work_size ,
                       num_events_in_wait_list, event_wait_list , event );



    //int * ptr = (int *)eptr;
   // int * ptr ;
   // memcpy(ptr)


    //ecode = ptr[0];
    printf("real realclEnqueueNDRangeKernel Called \t Status \t %d\n",ecode);
    return ecode;

}
#if 0


cl_int
clEnqueueNDRangeKernel(cl_command_queue  command_queue ,
                       cl_kernel         kernel ,
                       cl_uint           work_dim ,
                       const size_t *    global_work_offset ,
                       const size_t *    global_work_size ,
                       const size_t *    local_work_size ,
                       cl_uint           num_events_in_wait_list ,
                       const cl_event *  event_wait_list ,
                       cl_event *        event ) //CL_API_SUFFIX__VERSION_1_0
{

    cl_int error_code = NOT_FOUND;

    printf("Calling Perhaad's clEnqueueNDRangeKernel\t Status \t %d\n",error_code );

    static  cl_int
    (*realclEnqueueNDRangeKernel)
                    (    cl_command_queue  command_queue ,
                        cl_kernel         kernel ,
                        cl_uint           work_dim ,
                        const size_t *    global_work_offset ,
                        const size_t *    global_work_size ,
                        const size_t *    local_work_size ,
                        cl_uint           num_events_in_wait_list ,
                        const cl_event *  event_wait_list ,
                        cl_event *        event );

    char * func_error;
    if (!realclEnqueueNDRangeKernel)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclEnqueueNDRangeKernel =  (dlsym(RTLD_NEXT, "clEnqueueNDRangeKernel"));

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }

    error_code = realclEnqueueNDRangeKernel(command_queue, kernel ,
                       work_dim , global_work_offset , global_work_size ,local_work_size ,
                       num_events_in_wait_list, event_wait_list , event );


    printf("real realclEnqueueNDRangeKernel Called \t Status \t %d\n",error_code );
    //return new_error_code;
    return error_code;
}






//extern CL_API_ENTRY
cl_int
//CL_API_CALL
*clEnqueueTask(cl_command_queue  /* command_queue */,
              cl_kernel         /* kernel */,
              cl_uint           /* num_events_in_wait_list */,
              const cl_event *  /* event_wait_list */,
              cl_event *        /* event */) //CL_API_SUFFIX__VERSION_1_0;
{
    return INTERCEPTOR_NOT_IMPLEMENTED;
}

//extern CL_API_ENTRY
cl_int
//CL_API_CALL
*clEnqueueNativeKernel(cl_command_queue  /* command_queue */,
					  void (*user_func)(void *),
                      void *            /* args */,
                      size_t            /* cb_args */,
                      cl_uint           /* num_mem_objects */,
                      const cl_mem *    /* mem_list */,
                      const void **     /* args_mem_loc */,
                      cl_uint           /* num_events_in_wait_list */,
                      const cl_event *  /* event_wait_list */,
                      cl_event *        /* event */) //CL_API_SUFFIX__VERSION_1_0;
{

    return INTERCEPTOR_NOT_IMPLEMENTED;
}

#endif
