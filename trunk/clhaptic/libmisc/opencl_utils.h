#ifndef __OPENCL_UTILS_H_
#define __OPENCL_UTILS_H_

#include "string.h"
#include "stdlib.h"
#include "math.h"
#include <CL/cl.h>

#include "cassert"

//void assert(int expn)

//! Macro to enable / disable profiling
#define ENABLED 1

//! Macro to enable / disable profiling
#define DISABLED 0


//! other macros
#define TRUE 1

//! other macros
#define FALSE 0

#define EXITERROR 1

#define DONTEXITERROR 0

#define UNKNOWNSIZE -999

#define UNKNOWN -999

void * ad_malloc(size_t);

/*
 *
 * \return Pointer to a array allocated with the name of Device
 */
char * ad_getDeviceName(cl_device_id dev);

void ad_sync(cl_command_queue commandQueue);

bool rand_zero_or_one();

cl_mem ad_allocBufferPinned(size_t mem_size,cl_context ctx );

int ad_errChk(const cl_int status, const char * msg, bool exitOnErr = 0);

bool check_for_extensions(char * ext_name, int deviceId, cl_device_id * device_list);

void ad_setKernelArg(cl_kernel kernel, unsigned int index, size_t size,
                     void* data);

void list_all_opencl_devices();                     

char * cl_ReadSrcFile(char * kernelPath);

cl_program cl_CompileProgram(char * kernelPath,
							char * compileoptions, bool verbosebuild,
							cl_context ip_ctx, cl_device_id ip_device);

cl_program cl_CompileProgram_from_array(char * source,
							char * compileoptions, bool verbosebuild,
							cl_context ip_ctx,
							cl_device_id ip_device);

unsigned int idivup(unsigned int , unsigned int);

// Generate a filename based on the current time
char* createFilenameWithTimestamp() ;

//! Generate filename based on the current time and prefix string
char* createFilenameWithTimestamp(char * prefix_str);

uint get_num_devices(cl_platform_id ip_platform);

#endif // _OPENCL_UTILS_H_
