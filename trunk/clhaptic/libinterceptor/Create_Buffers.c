#define _GNU_SOURCE

#include "cl_pm.h"

#include "interceptor.h"

#include <stdio.h>
#include <dlfcn.h>

//typedef  cl_bitfield cl_mem_flags;

/**
    OpenCL calls interception
    export LD_PRELOAD='/usr/lib/libdl.so ./myOpenCL.so'
**/




cl_mem *clCreateBuffer(cl_context context,
             	cl_mem_flags flags,
             	size_t size,
             	void *host_ptr,
             	cl_int *errcode_ret)
{

    *errcode_ret = NOT_FOUND;

    printf("Calling Perhaad's clcreatebuffer\t Status \t %d\n",*errcode_ret);
    static cl_mem (*realclCreateBuffer)(cl_context context,
                     	cl_mem_flags flags,
 	                    size_t size,
                     	void *host_ptr,
                     	cl_int *errcode_ret);

    char * func_error;
    if (!realclCreateBuffer)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclCreateBuffer = dlsym(RTLD_NEXT, "clCreateBuffer");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }
    cl_mem x = realclCreateBuffer(context, flags, size, host_ptr, errcode_ret);
    //    return (realclCreateBuffer(context, flags, size, host_ptr, errcode_ret));
    printf("Real clcreatebuffer Called \t Status \t %d\n",*errcode_ret);
    return x;
}



//extern CL_API_ENTRY
/**

 \return  Buffer object

**/
cl_mem
//CL_API_CALL
*clCreateSubBuffer(cl_mem buf                         /* buffer */,
                  cl_mem_flags flags                  /* flags */,
                  cl_buffer_create_type buffer_type   /* buffer_create_type */,
                  const void * buffer_create_info     /* buffer_create_info */,
                  cl_int *    error_code              /* errcode_ret */)
                  //CL_API_SUFFIX__VERSION_1_1;
{

    *error_code = NOT_FOUND;
    printf("Calling Perhaad's clcreatesubbuffer\t Status \t %d\n",*error_code);
    static cl_mem (*realclCreateSubBuffer)(
                cl_mem buf                         /* buffer */,
                cl_mem_flags flags                  /* flags */,
                cl_buffer_create_type buffer_type   /* buffer_create_type */,
                const void * buffer_create_info     /* buffer_create_info */,
                cl_int *    error_code              /* errcode_ret */
                  );

    char * func_error;
    if (!realclCreateSubBuffer)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclCreateSubBuffer = dlsym(RTLD_NEXT, "clCreateSubBuffer");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }
    cl_mem x = realclCreateSubBuffer(buf, flags, buffer_type, buffer_create_info, error_code);
    printf("Real clcreatesubbuffer Called \t Status \t %d\n",*error_code);
    return x;
    //return INTERCEPTOR_NOT_IMPLEMENTED
}

//
//extern CL_API_ENTRY
//cl_int
//CL_API_CALL
//clRetainMemObject(cl_mem /* memobj */)
//                //CL_API_SUFFIX__VERSION_1_0;
//{
//
//    return INTERCEPTOR_NOT_IMPLEMENTED;
//
//}

//extern CL_API_ENTRY
cl_int
//CL_API_CALL
*clReleaseMemObject(cl_mem  memobj ) //CL_API_SUFFIX__VERSION_1_0;
{

    cl_int error_code = NOT_FOUND;
    printf("Calling Perhaad's realclReleaseMemObject \t Status \t %d\n",error_code);
    static cl_int (*realclReleaseMemObject)(
                cl_mem memobj          /* buffer */
                  );

    char * func_error;
    if (!realclReleaseMemObject)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclReleaseMemObject = dlsym(RTLD_NEXT, "clReleaseMemObject");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }
    error_code = realclReleaseMemObject(memobj);
    printf("Real realclReleaseMemObject Called \t Status \t %d\n",error_code);
    return error_code;
    //return INTERCEPTOR_NOT_IMPLEMENTED

}


//extern CL_API_ENTRY
cl_int
//CL_API_CALL
*clGetMemObjectInfo(cl_mem            memobj ,
                   cl_mem_info       param_name ,
                   size_t            param_value_size ,
                   void *            param_value ,
                   size_t *          param_value_size_ret ) //CL_API_SUFFIX__VERSION_1_0;
{
    cl_int error_code = NOT_FOUND;

    char * func_error;
    static cl_int
    (*realclGetMemObjectInfo)(
                cl_mem            memobj ,
                cl_mem_info       param_name ,
                size_t            param_value_size ,
                void *            param_value ,
                size_t *         param_value_size_ret   );

    if (!realclGetMemObjectInfo)
    {
        /* get address of ICD or OpenCL's  clCreateBuffer */
    	realclGetMemObjectInfo = dlsym(RTLD_NEXT, "clGetMemObjectInfo");

    	if( (func_error = dlerror()) != NULL)
    	{
	        fputs(func_error, stderr);
    	    exit(1);
	    }
    }
    error_code = realclGetMemObjectInfo(memobj , param_name , param_value_size ,
                                     param_value,param_value_size_ret );
    printf("Real realclGetMemObjectInfo Called \t Status \t %d\n",error_code);
    return error_code;

}



//extern CL_API_ENTRY cl_int CL_API_CALL
//clSetMemObjectDestructorCallback(  cl_mem /* memobj */,
//                            void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
//                            void * /*user_data */ )
                            //CL_API_SUFFIX__VERSION_1_1;
//{
//
//    return INTERCEPTOR_NOT_IMPLEMENTED ;
//}



