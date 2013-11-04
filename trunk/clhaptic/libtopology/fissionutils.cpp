#include <CL/cl.h>
#include <CL/cl_ext.h>

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "fission_topology.h"
#include "fissionutils.h"

static clCreateSubDevicesEXT_fn pfn_clCreateSubDevicesEXT = NULL;

#define cl_errChk ad_errChk

#define INIT_CL_EXT_FCN_PTR(name) \
    if(!pfn_##name) { \
        pfn_##name = (name##_fn) clGetExtensionFunctionAddress(#name); \
        if(!pfn_##name) { \
            std::cout << "Cannot get pointer to ext. fcn. " #name << std::endl; \
            exit(1); \
        } \
    }


//! setup a queue of a certain type given a context
void setup_gpu_queue(cl_context  ip_ctx,
		fission_topology * topo,
		bool enable_profiling = TRUE)
{
	list_all_opencl_devices();
	topo->root_context = ip_ctx;
	//Look for the number of devices
	size_t numDevices;
	cl_int status = clGetContextInfo (
			topo->root_context,	CL_CONTEXT_DEVICES,
		 	0,NULL,&numDevices);
	ad_errChk(status, "error getting no of devices");
	printf("Context in setup root is %p \t NumDevices Memory %d\n",topo->root_context,numDevices);
	topo->devices = (cl_device_id *)malloc(numDevices);
	// Populate the device array
	status = clGetContextInfo (topo->root_context,
					CL_CONTEXT_DEVICES,
					numDevices,
					topo->devices,&numDevices);
	ad_errChk(status, "error populating device array");

	numDevices = numDevices/sizeof(cl_device_id);

	printf("Number of devices in Ctx: %d\n",numDevices);
	topo->numRootDevices = numDevices;
	topo->rootQueue = (cl_command_queue * )malloc(sizeof(cl_command_queue)*(topo->numRootDevices));
	//printf("Set subdevices to 0\n");
	topo->numSubDevices = 0;
	for(cl_uint i = 0 ;i < (topo->numRootDevices); i++)
	{

		//!Check if Device requested is a CPU or a GPU
		cl_device_type dtype;
		status = clGetDeviceInfo(topo->devices[i],
						CL_DEVICE_TYPE,
						sizeof(dtype),
						(void *)&dtype,
						NULL);

		if(cl_errChk(status,"Error in Getting Device Info\n")) exit(1);
		//! Choose the first GPU
		if(dtype == CL_DEVICE_TYPE_GPU)
		{
 			topo->gpu_queue_no = i;
			if(enable_profiling == TRUE)
			{
				topo->rootQueue[i] = clCreateCommandQueue(
								topo->root_context,
								topo->devices[i],
								CL_QUEUE_PROFILING_ENABLE, &status);
 			}
			else
				topo->rootQueue[i] = clCreateCommandQueue(
								topo->root_context,
								topo->devices[i],
								NULL, &status);
			if(cl_errChk(status,"Error Creating CQ\n")) exit(-1);
			return;
		}
	}
	printf("No GPU could be found");
	exit(-1);
}


//! setup a queue of a certain type given a context
void setup_cpu_queue(cl_context  ip_ctx,
				fission_topology * topo,
				bool enable_profiling = TRUE)
{
	list_all_opencl_devices();
	topo->root_context = ip_ctx;
	//Look for the number of devices
	size_t mem_numDevices;
	size_t numDevices;
	cl_int status = clGetContextInfo (
			topo->root_context,	CL_CONTEXT_DEVICES,
		 	0,NULL,&mem_numDevices);
	ad_errChk(status, "error getting no of devices");
	printf("Context in setup root is %p \t NumDevices Memory %d\n",topo->root_context,mem_numDevices);
	topo->devices = (cl_device_id *)malloc(mem_numDevices);
	// Populate the device array
	status = clGetContextInfo (topo->root_context,
					CL_CONTEXT_DEVICES,
					mem_numDevices,topo->devices,&mem_numDevices);
	ad_errChk(status, "error populating device array");

	numDevices = mem_numDevices/sizeof(cl_device_id);

	printf("Number of devices in Ctx: %d\n",numDevices);
	topo->numRootDevices = numDevices;
	topo->rootQueue = (cl_command_queue * )malloc(sizeof(cl_command_queue)*(topo->numRootDevices));
	//printf("Set subdevices to 0\n");
	topo->numSubDevices = 0;
	int queue_index = 0 ;
	status = CL_SUCCESS;
	for(cl_uint i = 0 ;i < (topo->numRootDevices); i++)
	{
		printf("Itn No %d\n",i);

		//!Check if Device requested is a CPU or a GPU
		cl_device_type dtype;
		status = clGetDeviceInfo(topo->devices[i],
						CL_DEVICE_TYPE,
						sizeof(dtype),
						(void *)&dtype,
						NULL);

		if(cl_errChk(status,"Error in Getting Device Info\n")) exit(-1);
		//! Choose the first CPU
		printf("Searching for A CPU ");
		if(dtype == CL_DEVICE_TYPE_CPU)
		{
			topo->cpu_queue_no = queue_index;
			if(enable_profiling == TRUE)
			{
				topo->rootQueue[queue_index] = clCreateCommandQueue(
								topo->root_context,
								topo->devices[i],
								CL_QUEUE_PROFILING_ENABLE, &status);
 			}
			else
			{
				topo->rootQueue[queue_index] = clCreateCommandQueue(
								topo->root_context,
								topo->devices[i],
								CL_QUEUE_PROFILING_ENABLE, &status);
			}
			if(cl_errChk(status,"Error Creating CQ\n"))
				exit(-1);
			return;
		}
		else
		{
			if(dtype == CL_DEVICE_TYPE_GPU)
				printf("Found a GPU");
		}
	}
	printf("No CPU could be found");
	exit(-1);
}


void setup_multiple_root_queue(fission_topology * topo,
		bool enable_profiling)
{
 	cl_platform_id  * platforms ;
	cl_int status;

	int platform_touse;
	int device_touse;
	cl_uint numPlatforms;

	list_all_opencl_devices();
	printf("Enter platform No - Will create all queues for platform \n");
	scanf("%d %d",&platform_touse);
	printf("Using platform %d\t \n",platform_touse);

	cl_uint numDevices;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);
	platforms = (cl_platform_id *)malloc(sizeof(cl_device_id)*numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL,
							0, NULL, &numDevices);

	if(cl_errChk(status,"Error in Getting Device Count\n")) exit(1);

	printf("\n%d devices for %d Platform \n",numDevices, platform_touse);
	topo->numRootDevices = numDevices;
	topo->devices = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);

	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, numDevices,
					topo->devices, NULL);

	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);


	//!Check if Device requested is a CPU or a GPU
	cl_device_type dtype = CL_DEVICE_TYPE_ALL;
	//status = clGetDeviceInfo(topo->devices[device_touse],
	//				CL_DEVICE_TYPE,
	//				sizeof(dtype),
	//				(void *)&dtype,
	//				NULL);

	if(cl_errChk(status,"Error in Getting Device Info\n")) exit(1);
	if(dtype == CL_DEVICE_TYPE_GPU)
		printf("Creating GPU Context\n");
	else if (dtype == CL_DEVICE_TYPE_CPU)
		printf("Creating CPU Context\n");
	else if (dtype == CL_DEVICE_TYPE_ALL)
		printf("any device will do");
	else
		perror("This Context Type Not Supported\n");

	cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[platform_touse]), 0};

	//! Creating  root context - No subdevices needed here
	cl_context_properties *cprops = cps;
	topo->root_context = clCreateContextFromType(
					cprops, (cl_device_type)dtype,
					NULL, NULL, &status);
	if(cl_errChk(status, "creating Root Context"))
		exit(1);
	topo->rootQueue = (cl_command_queue * )malloc(sizeof(cl_command_queue)*topo->numRootDevices);
	topo->device_used = 0;
	printf("device to use %d\n",device_touse);
	for (int i = 0; i<topo->numRootDevices; i++)
	{
		status = clGetDeviceInfo(topo->devices[i],
						CL_DEVICE_TYPE,
						sizeof(dtype),
						(void *)&dtype,
						NULL);
		if (dtype == CL_DEVICE_TYPE_CPU)
			topo->cpu_queue_no = i;
		if (dtype == CL_DEVICE_TYPE_CPU)
			topo->gpu_queue_no = i;
		if(enable_profiling == TRUE)
			topo->rootQueue[i] = clCreateCommandQueue(
					topo->root_context,
					topo->devices[i],
					CL_QUEUE_PROFILING_ENABLE, &status);
		else
			topo->rootQueue[i] = clCreateCommandQueue(
					topo->root_context,
					topo->devices[i],
					CL_QUEUE_PROFILING_ENABLE, &status);
		if(cl_errChk(status,"clCreateCommandQueue failed."))exit(1);
	}
}

void setup_root_queue(fission_topology * topo,
					bool enable_profiling )
{
 	cl_platform_id  * platforms ;
	cl_int status;

	int platform_touse;
	int device_touse;
	cl_uint numPlatforms;

	list_all_opencl_devices();
	printf("Enter platform and device\n");
	scanf("%d %d",&platform_touse, &device_touse);
	printf("Using platform %d\t and device %d\n",platform_touse,device_touse);

	cl_uint numDevices;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);
	platforms = (cl_platform_id *)malloc(sizeof(cl_device_id)*numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL,
							0, NULL, &numDevices);

	if(cl_errChk(status,"Error in Getting Device Count\n")) exit(1);

	printf("\n%d devices for %d Platform \n",numDevices, platform_touse);
	topo->numRootDevices = numDevices;
	topo->devices = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);

	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, numDevices,
					topo->devices, NULL);

	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);


	//!Check if Device requested is a CPU or a GPU
	cl_device_type dtype = CL_DEVICE_TYPE_ALL;
	//status = clGetDeviceInfo(topo->devices[device_touse],
	//				CL_DEVICE_TYPE,
	//				sizeof(dtype),
	//				(void *)&dtype,
	//				NULL);

	if(cl_errChk(status,"Error in Getting Device Info\n")) exit(1);
	if(dtype == CL_DEVICE_TYPE_GPU)
		printf("Creating GPU Context\n");
	else if (dtype == CL_DEVICE_TYPE_CPU)
		printf("Creating CPU Context\n");
	else if (dtype == CL_DEVICE_TYPE_ALL)
		printf("any device will do");
	else
		perror("This Context Type Not Supported\n");

	cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[platform_touse]), 0};

	//! Creating  root context - No subdevices needed here
	cl_context_properties *cprops = cps;
	topo->root_context = clCreateContextFromType(
					cprops, (cl_device_type)dtype,
					NULL, NULL, &status);
	if(cl_errChk(status, "creating Root Context"))
		exit(1);
	topo->rootQueue = (cl_command_queue * )malloc(sizeof(cl_command_queue)*1);
	topo->device_used = device_touse;
	printf("device to use %d\n",device_touse);
	if(enable_profiling == TRUE)
		topo->rootQueue[0] = clCreateCommandQueue(
						topo->root_context,
						topo->devices[device_touse],
						CL_QUEUE_PROFILING_ENABLE, &status);
	else
		topo->rootQueue[0] = clCreateCommandQueue(
						topo->root_context,
						topo->devices[device_touse],
						CL_QUEUE_PROFILING_ENABLE, &status);


	if(cl_errChk(status,"clCreateCommandQueue failed."))exit(1);

}



void setup_baseline(fission_topology * topo)
{
    printf("Baseline Config for Fission Experiments\n");
    cl_platform_id  * platforms ;
    cl_int status;

    int platform_touse;
    int device_touse;
    cl_uint numPlatforms;

    list_all_opencl_devices();
    printf("Enter platform and device\n");
    scanf("%d %d",&platform_touse, &device_touse);
    printf("Using platform %d\t and device %d\n",platform_touse,device_touse);

    cl_uint numDevices;


	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);
    platforms = (cl_platform_id *)malloc(sizeof(cl_device_id)*numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL,
                            0, NULL, &numDevices);

	if(cl_errChk(status,"Error in Getting Device Count\n")) exit(1);

    printf("\n%d devices for %d Platform \n",numDevices, platform_touse);

	topo->devices = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);

	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, numDevices,
					topo->devices, NULL);

	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);


	//!Check if Device requested is a CPU or a GPU
	cl_device_type dtype;
	status = clGetDeviceInfo(topo->devices[device_touse],
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

    //! Creating  root context - No subdevices needed here
	cl_context_properties *cprops = cps;
    topo->root_context = clCreateContextFromType(
					cprops, (cl_device_type)dtype,
					NULL, NULL, &status);
    if(cl_errChk(status, "creating Root Context")) exit(1);


    topo->numSubDevices = 1;


    topo->subDevices = (cl_device_id*)malloc( (topo->numSubDevices) * sizeof(cl_device_id));
    topo->subQueue= (cl_command_queue *)malloc( (topo->numSubDevices) * sizeof(cl_command_queue ));
    topo->subDevices[0] = topo->devices[device_touse];
    topo->subQueue[0] = clCreateCommandQueue(topo->root_context,
						topo->subDevices[0], CL_QUEUE_PROFILING_ENABLE, &status);


    if(cl_errChk(status,"clCreateCommandQueue for subdevices failed."))exit(1);

}




//! http://forums.amd.com/devforum/messageview.cfm?catid=390&threadid=136299
//! Reason for using function pointers
void setup_fission(fission_topology * topo)
{

    cl_platform_id  * platforms ;
    cl_int status;

    int platform_touse;
    int device_touse;
    cl_uint numPlatforms;

    list_all_opencl_devices();
    printf("Enter platform and device\n");
    scanf("%d %d",&platform_touse, &device_touse);
    printf("Using platform %d\t and device %d\n",platform_touse,device_touse);

    cl_uint numDevices = UNKNOWN;


	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);
    platforms = (cl_platform_id *)malloc(sizeof(cl_device_id)*numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL,
                            0, NULL, &numDevices);

	if(cl_errChk(status,"Error in Getting Device Count\n")) exit(1);

    printf("\n%d devices for %d Platform \n",numDevices, platform_touse);

	topo->devices = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);
	topo->numRootDevices =  numDevices;
	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, numDevices,
					topo->devices, NULL);

	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);


	//!Check if Device requested is a CPU or a GPU
	cl_device_type dtype;
	status = clGetDeviceInfo(topo->devices[device_touse],
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
    topo->root_context = clCreateContextFromType(
					cprops, (cl_device_type)dtype,
					NULL, NULL, &status);
    if(cl_errChk(status, "creating Root Context")) exit(1);

    bool extcheck = check_for_extensions("cl_ext_device_fission",device_touse, topo->devices);
    if(!extcheck)
    {   printf("Device Fission Not Supported");
        exit(1);
    }

    //    topo->numSubDevices  = 2;
    //cl_device_id * subDevices ;
    // Initialize required partition property

    cl_device_partition_property_ext partitionPrty[3] =
    {       CL_DEVICE_PARTITION_EQUALLY_EXT,
            1,
            CL_PROPERTIES_LIST_END_EXT
    };

    // Initialize clCreateSubDevicesEXT function pointer

    // Get number of sub-devices

    INIT_CL_EXT_FCN_PTR(clCreateSubDevicesEXT);
    topo->root_device = topo->devices[device_touse];

    status = pfn_clCreateSubDevicesEXT(topo->devices[device_touse],
                                        partitionPrty,
                                        0,
                                        NULL,
                                        &topo->numSubDevices);
    if(cl_errChk(status,"clCreateSubDevicesEXT failed.")) exit(1);

    printf("no of subdevices %d\n",topo->numSubDevices);

    topo->rootQueue = (cl_command_queue *)malloc(topo->numRootDevices*sizeof(cl_command_queue));

    topo->subDevices = (cl_device_id*)malloc( (topo->numSubDevices) * sizeof(cl_device_id));
    topo->subQueue= (cl_command_queue *)malloc( (topo->numSubDevices) * sizeof(cl_command_queue ));
    topo->profiling_status = (bool *)malloc(sizeof(bool)*topo->numSubDevices);
    for(int i=0;i < (topo->numSubDevices);i++)
    	topo->profiling_status[i] = ENABLED;

	if(NULL == (topo->subDevices))
        printf("Failed to allocate memory(subDevices)");

    status = pfn_clCreateSubDevicesEXT(topo->devices[device_touse],
                                        partitionPrty,
                                        topo->numSubDevices,
                                        topo->subDevices,
                                        NULL);
    if(cl_errChk(status, "clCreateSubDevicesEXT failed.")) exit(1);

    // Create context for sub-devices
    topo->subContext =
            clCreateContext(cps, topo->numSubDevices,
                         topo->subDevices, NULL, NULL, &status);


    if(cl_errChk(status,"clCreateContext for subdevices failed."))exit(1);
    printf("Creating contexts for subdevices\n");
    topo->rootQueue[0] = clCreateCommandQueue(topo->root_context,
					topo->root_device, CL_QUEUE_PROFILING_ENABLE, &status);


	for(unsigned int i=0;i<(topo->numSubDevices);i++)
    {

        if(topo->profiling_status[i] == ENABLED)
        {
        	printf("Init Sub-queue \t %d Profiling ENABLED\n",i);
        	topo->subQueue[i] = clCreateCommandQueue(topo->subContext,
						topo->subDevices[i], CL_QUEUE_PROFILING_ENABLE, &status);
        }
        else
        {
        	printf("Init Sub-queue \t %d Profiling DISABLED\n",i);
        	topo->subQueue[i] = clCreateCommandQueue(topo->subContext,
						topo->subDevices[i], NULL, &status);
        }
        printf("Value of subdevice %p \n", topo->subDevices[i]);

        cl_errChk(status,"clCreateCommandQueue for subdevices failed.");

    }


}


void multi_device_single_context(fission_topology * topo)
{

    printf("Two CQs on 2 Devices");
    cl_platform_id  * platforms ;
    cl_int status;

    int platform_touse;
    int device_touse;
    cl_uint numPlatforms;

    list_all_opencl_devices();
    platform_touse = 0;
    device_touse = 0;
    //printf("Enter platform and device\n");
    //scanf("%d %d",&platform_touse, &device_touse);
    //printf("Using platform %d\t and device %d\n",platform_touse,device_touse);

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);
    platforms = (cl_platform_id *)malloc(sizeof(cl_device_id)*numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL,
                            0, NULL, &(topo->numRootDevices));

	if(cl_errChk(status,"Error in Getting Device Count\n")) exit(1);

    printf("\n%d devices for %d Platform \n",topo->numRootDevices, platform_touse);

	topo->devices = (cl_device_id *)malloc(sizeof(cl_device_id)*(topo->numRootDevices));
    topo->root_device_type  = (cl_device_type *)malloc( (topo->numRootDevices) * sizeof(cl_device_type));
    topo->rootQueue  = (cl_command_queue *) malloc( (topo->numRootDevices) * sizeof(cl_command_queue));

	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, topo->numRootDevices,
					topo->devices, NULL);


	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);

    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[platform_touse]), 0};

	cl_context_properties *cprops = cps;
    topo->root_context = clCreateContextFromType(
					cprops, CL_DEVICE_TYPE_ALL,
					NULL, NULL, &status);
    if(cl_errChk(status, "creating Root Context")) exit(1);

    for(unsigned int i = 0; i< topo->numRootDevices ; i++)
    {

        //!Check if Device requested is a CPU or a GPU

        status = clGetDeviceInfo(topo->devices[i],
                        CL_DEVICE_TYPE,
                        sizeof(cl_device_type),
                        (void *)&(topo->root_device_type[i]),
                        NULL);

        if(cl_errChk(status,"Error in Getting Device Info\n")) exit(1);
        if(topo->root_device_type[i] == CL_DEVICE_TYPE_GPU)
        {
            topo->gpu_queue_no = i;
            printf("Creating GPU Queue for %d Device \n",i);
        }
        else if (topo->root_device_type[i] == CL_DEVICE_TYPE_CPU)
        {
            topo->cpu_queue_no = i;
            printf("Creating CPU Queue for %d Device \n",i);
        }
        else
            perror("This Context Type Not Supported\n");


#ifdef PROFILING

        topo->rootQueue[i] = clCreateCommandQueue(topo->root_context,
						topo->devices[i], CL_QUEUE_PROFILING_ENABLE, &status);

#else

        topo->rootQueue[i] = clCreateCommandQueue(topo->root_context,
						topo->devices[i], NULL, &status);

#endif // PROFILING

        //printf("Value of subdevice %p \n", topo->subDevices[i]);

        cl_errChk(status,"clCreateCommandQueue for subdevices failed.");

    }

}


//! http://forums.amd.com/devforum/messageview.cfm?catid=390&threadid=136299
//! Reason for using function pointers
void setup_fission_pipeline(fission_topology * topo)
{
    printf("USING SURF Pipeline oclprof\n");


    cl_platform_id  * platforms ;


    cl_int status;

    int platform_touse;
    int device_touse;
    cl_uint numPlatforms;

    list_all_opencl_devices();
    //printf("Enter platform and device\n");
    //scanf("%d %d",&platform_touse, &device_touse);
    platform_touse = 0;
    device_touse = 0;
    printf("Using platform %d\t and device %d\n",platform_touse,device_touse);

    cl_uint numDevices;


	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);
    platforms = (cl_platform_id *)malloc(sizeof(cl_device_id)*numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	status = clGetDeviceIDs(platforms[platform_touse], CL_DEVICE_TYPE_ALL,
                            0, NULL, &numDevices);

	if(cl_errChk(status,"Error in Getting Device Count\n")) exit(1);

    printf("\n%d devices for %d Platform \n",numDevices, platform_touse);

	topo->devices = (cl_device_id *)malloc(sizeof(cl_device_id)*numDevices);

	status = clGetDeviceIDs(platforms[platform_touse],
					CL_DEVICE_TYPE_ALL, numDevices,
					topo->devices, NULL);

	if(cl_errChk(status,"Error in Getting Devices\n")) exit(1);


	//!Check if Device requested is a CPU or a GPU
	cl_device_type dtype;
	status = clGetDeviceInfo(topo->devices[device_touse],
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
    topo->root_context = clCreateContextFromType(
					cprops, (cl_device_type)dtype,
					NULL, NULL, &status);
    if(cl_errChk(status, "creating Root Context")) exit(1);

    bool extcheck = check_for_extensions("cl_ext_device_fission",device_touse, topo->devices);
    if(!extcheck)
    {   printf("Device Fission Not Supported");
        exit(1);
    }

//    topo->numSubDevices  = 2;
    //cl_device_id * subDevices ;
    // Initialize required partition property
    cl_device_partition_property_ext partitionPrty[3] =
    {       CL_DEVICE_PARTITION_EQUALLY_EXT,
            1,
            CL_PROPERTIES_LIST_END_EXT
    };

    // Initialize clCreateSubDevicesEXT function pointer

    // Get number of sub-devices

    INIT_CL_EXT_FCN_PTR(clCreateSubDevicesEXT);
    printf("no of subdevices %d\n",topo->numSubDevices);
    topo->root_device = topo->devices[device_touse];

    status = pfn_clCreateSubDevicesEXT(topo->devices[device_touse],
                                        partitionPrty,
                                        0,
                                        NULL,
                                        &topo->numSubDevices);
    if(cl_errChk(status,"clCreateSubDevicesEXT failed.")) exit(1);

    printf("no of subdevices %d\n",topo->numSubDevices);


    topo->subDevices = (cl_device_id*)malloc( (topo->numSubDevices) * sizeof(cl_device_id));
    topo->subQueue= (cl_command_queue *)malloc( (topo->numSubDevices) * sizeof(cl_command_queue ));

    if(NULL == (topo->subDevices))
        printf("Failed to allocate memory(subDevices)");

    status = pfn_clCreateSubDevicesEXT(topo->devices[device_touse],
                                        partitionPrty,
                                        topo->numSubDevices,
                                        topo->subDevices,
                                        NULL);
    if(cl_errChk(status, "clCreateSubDevicesEXT failed.")) exit(1);

    // Create context for sub-devices
    topo->subContext =
            clCreateContext(cps, topo->numSubDevices,
                         topo->subDevices, NULL, NULL, &status);


    if(cl_errChk(status,"clCreateContext for subdevices failed."))exit(1);
    printf("Creating contexts for subdevices\n");

    for(unsigned int i=0;i<(topo->numSubDevices);i++)
    {
        printf("Init Sub-queue \t %d\n",i);

#ifdef PROFILING

        topo->subQueue[i] = clCreateCommandQueue(topo->subContext,
						topo->subDevices[i], CL_QUEUE_PROFILING_ENABLE, &status);

#else

        topo->subQueue[i] = clCreateCommandQueue(topo->subContext,
						topo->subDevices[i], CL_QUEUE_PROFILING_ENABLE, &status);

#endif // PROFILING

        printf("Value of subdevice %p \n", topo->subDevices[i]);

        cl_errChk(status,"clCreateCommandQueue for subdevices failed.");

    }


}
