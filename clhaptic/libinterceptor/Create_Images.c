#define _GNU_SOURCE

#include <CL/cl_pm.h>

#include <stdio.h>
#include <dlfcn.h>

//typedef  cl_bitfield cl_mem_flags;

/**
    OpenCL calls interception
    export LD_PRELOAD='/usr/lib/libdl.so ./myOpenCL.so'
**/

#define NOT_IMPLEMENTED 9999

cl_mem *clCreateBuffer(cl_context context,
             	cl_mem_flags flags,
             	size_t size,
             	void *host_ptr,
             	cl_int *errcode_ret)
{

    *errcode_ret = NOT_IMPLEMENTED;
    printf("Calling Perhaad's clcreatebuffer\t Status \t %d\n",*errcode_ret);
    static cl_mem *(*realclCreateBuffer)(cl_context context,
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

    *error_code = NOT_IMPLEMENTED;
    printf("Calling Perhaad's clcreatesubbuffer\t Status \t %d\n",*errcode_ret);
    static cl_mem *(*realclCreateSubBuffer)(
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
    printf("Real clcreatesubbuffer Called \t Status \t %d\n",*errcode_ret);
    return x;
    //return INTERCEPTOR_NOT_IMPLEMENTED
}


//extern CL_API_ENTRY
cl_mem
//CL_API_CALL
clCreateImage2D(cl_context              /* context */,
                cl_mem_flags            /* flags */,
                const cl_image_format * /* image_format */,
                size_t                  /* image_width */,
                size_t                  /* image_height */,
                size_t                  /* image_row_pitch */,
                void *                  /* host_ptr */,
                cl_int *                /* errcode_ret */)
                //CL_API_SUFFIX__VERSION_1_0;
{

return INTERCEPTOR_NOT_IMPLEMENTED

}

extern CL_API_ENTRY
cl_mem
CL_API_CALL
clCreateImage3D(cl_context              /* context */,
                cl_mem_flags            /* flags */,
                const cl_image_format * /* image_format */,
                size_t                  /* image_width */,
                size_t                  /* image_height */,
                size_t                  /* image_depth */,
                size_t                  /* image_row_pitch */,
                size_t                  /* image_slice_pitch */,
                void *                  /* host_ptr */,
                cl_int *                /* errcode_ret */)
                //CL_API_SUFFIX__VERSION_1_0;
{
    return INTERCEPTOR_NOT_IMPLEMENTED
}

extern CL_API_ENTRY
cl_int
CL_API_CALL
clRetainMemObject(cl_mem /* memobj */)
                //CL_API_SUFFIX__VERSION_1_0;
{

    return INTERCEPTOR_NOT_IMPLEMENTED

}

extern CL_API_ENTRY
cl_int
CL_API_CALL
clReleaseMemObject(cl_mem /* memobj */)
                //CL_API_SUFFIX__VERSION_1_0;
{

    return INTERCEPTOR_NOT_IMPLEMENTED


}

extern CL_API_ENTRY cl_int CL_API_CALL
clGetSupportedImageFormats(cl_context           /* context */,
                           cl_mem_flags         /* flags */,
                           cl_mem_object_type   /* image_type */,
                           cl_uint              /* num_entries */,
                           cl_image_format *    /* image_formats */,
                           cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0;

extern CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(cl_mem           /* memobj */,
                   cl_mem_info      /* param_name */,
                   size_t           /* param_value_size */,
                   void *           /* param_value */,
                   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

extern CL_API_ENTRY cl_int CL_API_CALL
clGetImageInfo(cl_mem           /* image */,
               cl_image_info    /* param_name */,
               size_t           /* param_value_size */,
               void *           /* param_value */,
               size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

extern CL_API_ENTRY cl_int CL_API_CALL
clSetMemObjectDestructorCallback(  cl_mem /* memobj */,
                                    void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
                                    void * /*user_data */ )
                                    //CL_API_SUFFIX__VERSION_1_1;
{

}



