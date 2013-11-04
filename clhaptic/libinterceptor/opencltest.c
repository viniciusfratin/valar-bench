#include <CL/cl.h>
#include "stdio.h"
#include <iostream>


cl_context clGPUContext;



//! Globally visible OpenCL cmd queue
cl_command_queue clCommandQueue;

cl_device_id device;

#define TRUE 1

#define FALSE 0

int cl_errChk(const cl_int status, const char * msg)
{

	if(status != CL_SUCCESS) {
		printf("OpenCL Error: %d\t %s \n", status, msg);
		return TRUE;
	}
	return FALSE;
}


cl_context cl_init_context()
{
	cl_int status;
	// Used to iterate through the platforms and devices, respectively
	cl_uint numPlatforms;
	cl_uint numDevices;

	// These will hold the platform and device we select (can potentially be
	// multiple, but we're just doing one for now)
	// cl_platform_id platform = NULL;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);

	// Print some information about the available platforms
	cl_platform_id *platforms = NULL;
	cl_device_id * devices = NULL;
	if (numPlatforms > 0)
	{
		// get all the platforms
		platforms = (cl_platform_id*)malloc(numPlatforms *
			sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);

		// Traverse the platforms array
		printf("Checking For OpenCl Compatible Devices\n");
		int device_index = 0;
		for(unsigned int i = 0; i < numPlatforms ; i++)
		{
			char pbuf[100];
			printf("Platform %d:\t", i);
			status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,
				sizeof(pbuf), pbuf, NULL);
			printf("Vendor: %s\n", pbuf);

			//unsigned int numDevices;

			status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
			if(cl_errChk(status, "checking for devices"))
				exit(1);
			if(numDevices == 0) {
				printf("There are no devices for Platform %d\n",i);
				exit(0);
			}
			else
			{
				printf("\tNo of devices for Platform %d is %u\n",i, numDevices);
				//! Allocate an array of devices of size "numDevices"
				devices = (cl_device_id*)malloc(sizeof(cl_device_id)*numDevices);
				//! Populate Arrray with devices
				status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices,
					devices, NULL);
				if(cl_errChk(status, "getting device IDs")) {
					exit(1);
				}
			}
			for( unsigned int j = 0; j < numDevices; j++)
			{
				char dbuf[100];
				char deviceStr[100];
				printf("\tDevice: %d\t", j);
				status = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(dbuf),
					deviceStr, NULL);
				cl_errChk(status, "Getting Device Info\n");
				printf("Vendor: %s", deviceStr);
				status = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(dbuf),
					dbuf, NULL);
				printf("\n\t\tName: %s\n", dbuf);
			}
		}
	}
	else
	{
		// If no platforms are available, we're sunk!
		printf("No OpenCL platforms found\n");
		exit(0);
	}

	int platform_touse, device_touse;
	printf("Enter Platform and Device No (Seperated by Space) \n");
	scanf("%d %d", &platform_touse, &device_touse);
	printf("Using Platform %d \t Device No %d \n",platform_touse, device_touse);

	//! Recheck how many devices does our chosen platform have
	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

	if(device_touse > numDevices)
	{
		printf("Invalid Device Number\n");
		exit(1);
	}
	else
		printf("No of devices is %d\n",numDevices);

	//! Populate devices array with all the visible devices of our chosen platform
	devices = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);
	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, numDevices,
					devices, NULL);
	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);


	//!Check if Device requested is a CPU or a GPU
	cl_device_type dtype;
	device = devices[device_touse];
	status = clGetDeviceInfo(devices[device_touse],
					CL_DEVICE_TYPE,
					sizeof(dtype),
					(void *)&dtype,
					NULL);
	if(cl_errChk(status,"Error in Getting Device Info\n")) exit(1);
	if(dtype == CL_DEVICE_TYPE_GPU) printf("Creating GPU Context\n");
	else if (dtype == CL_DEVICE_TYPE_CPU) printf("Creating CPU Context\n");
	else perror("This Context Type Not Supported\n");

	cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[platform_touse]), 0};

	cl_context_properties *cprops = cps;
	clGPUContext = clCreateContextFromType(
					cprops, (cl_device_type)dtype,
					NULL, NULL, &status);
	if(cl_errChk(status, "creating Context")) {
		exit(1);
	}

#define PROFILING

#ifdef PROFILING

	clCommandQueue = clCreateCommandQueue(clGPUContext,
						devices[device_touse], CL_QUEUE_PROFILING_ENABLE, &status);

#else

	clCommandQueue = clCreateCommandQueue(clGPUContext,
						devices[device_touse], NULL, &status);

#endif // PROFILING

	if(cl_errChk(status, "creating command queue")) {
		exit(1);
	}
	return clGPUContext;
}




int main()
{
    int N = 100;
    cl_init_context();

    cl_int status = CL_SUCCESS;

    clCreateBuffer(clGPUContext,
                    CL_MEM_READ_WRITE, N*sizeof(float),
                    NULL, &status);
    cl_kernel k;
    size_t localws[1]={1};
    size_t globalws[1]={16};
    printf("enq kernel\n");
    status = clEnqueueNDRangeKernel(clCommandQueue,k,1,0,globalws,localws,0,NULL,NULL);
    printf("Status is %d \n",status);

}
