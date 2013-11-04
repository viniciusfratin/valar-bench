#include <cmath>
#include <ctime>
#include <iostream>
#include "stdio.h"

#include "opencl_utils.h"

#define MAX_ERR_VAL 64

/**
 * A toy function that generates a random 0 or 1 sequence.
 * Used to test profiler stuff
 */
bool rand_zero_or_one()
{
	srand((unsigned)time(NULL));
	float val =((float) rand() / (RAND_MAX)) ;

	int k = round(val);
 	if(k == 0)
		return 0;
	else
	{
		if (k == 1)
			return 1;
		else
		{
			printf("A bug in analysis-devices-utils.cpp");
			exit(-1);
		}

	}

}


char * cl_ReadSrcFile(char * kernelPath)
{
    FILE *fp;
    cl_int status;

    char *source;
    long int size;

    printf("Kernel file: %s\n", kernelPath);

    fp = fopen(kernelPath, "rb");
    if(!fp) {
        printf("Could not open kernel file\n");
        exit(-1);
    }
    status = fseek(fp, 0, SEEK_END);
    if(status != 0) {
        printf("Error seeking to end of file\n");
        exit(-1);
    }
    size = ftell(fp);
    printf("size:**********************%d\n",size);
    if(size < 0) {
        printf("Error getting file position\n");
        exit(-1);
    }
    /*status = fseek(fp, 0, SEEK_SET);
    if(status != 0) {
    printf("Error seeking to start of file\n");
    exit(-1);
    }*/
    rewind(fp);

    source = (char *)malloc(size + 1);
    // fill with NULLs
    for (int i=0;i<size+1;i++) source[i]='\0';
    if(source == NULL) {
        printf("Error allocating space for the kernel source\n");
        exit(-1);
    }

    //fread(source, size, 1, fp);   // add error checking here
    fread(source,1,size,fp);
    source[size] = '\0';
    fclose(fp);

    return source;
}


//! OpenCl error code list
/*!
    An array of character strings used to give the error corresponding to the error code \n

    The error code is the index within this array
*/
const char *cl_err_msg[MAX_ERR_VAL] = {
    "CL_SUCCESS",                         // 0
    "CL_DEVICE_NOT_FOUND",                //-1
    "CL_DEVICE_NOT_AVAILABLE",            //-2
    "CL_COMPILER_NOT_AVAILABLE",          //-3
    "CL_MEM_OBJECT_ALLOCATION_FAILURE",   //-4
    "CL_OUT_OF_RESOURCES",                //-5
    "CL_OUT_OF_HOST_MEMORY",              //-6
    "CL_PROFILING_INFO_NOT_AVAILABLE",    //-7
    "CL_MEM_COPY_OVERLAP",                //-8
    "CL_IMAGE_FORMAT_MISMATCH",           //-9
    "CL_IMAGE_FORMAT_NOT_SUPPORTED",      //-10
    "CL_BUILD_PROGRAM_FAILURE",           //-11
    "CL_MAP_FAILURE",                     //-12
    "",                                   //-13
    "",                                   //-14
    "",                                   //-15
    "",                                   //-16
    "",                                   //-17
    "",                                   //-18
    "",                                   //-19
    "",                                   //-20
    "",                                   //-21
    "",                                   //-22
    "",                                   //-23
    "",                                   //-24
    "",                                   //-25
    "",                                   //-26
    "",                                   //-27
    "",                                   //-28
    "",                                   //-29
    "CL_INVALID_VALUE",                   //-30
    "CL_INVALID_DEVICE_TYPE",             //-31
    "CL_INVALID_PLATFORM",                //-32
    "CL_INVALID_DEVICE",                  //-33
    "CL_INVALID_CONTEXT",                 //-34
    "CL_INVALID_QUEUE_PROPERTIES",        //-35
    "CL_INVALID_COMMAND_QUEUE",           //-36
    "CL_INVALID_HOST_PTR",                //-37
    "CL_INVALID_MEM_OBJECT",              //-38
    "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR", //-39
    "CL_INVALID_IMAGE_SIZE",              //-40
    "CL_INVALID_SAMPLER",                 //-41
    "CL_INVALID_BINARY",                  //-42
    "CL_INVALID_BUILD_OPTIONS",           //-43
    "CL_INVALID_PROGRAM",                 //-44
    "CL_INVALID_PROGRAM_EXECUTABLE",      //-45
    "CL_INVALID_KERNEL_NAME",             //-46
    "CL_INVALID_KERNEL_DEFINITION",       //-47
    "CL_INVALID_KERNEL",                  //-48
    "CL_INVALID_ARG_INDEX",               //-49
    "CL_INVALID_ARG_VALUE",               //-50
    "CL_INVALID_ARG_SIZE",                //-51
    "CL_INVALID_KERNEL_ARGS",             //-52
    "CL_INVALID_WORK_DIMENSION ",         //-53
    "CL_INVALID_WORK_GROUP_SIZE",         //-54
    "CL_INVALID_WORK_ITEM_SIZE",          //-55
    "CL_INVALID_GLOBAL_OFFSET",           //-56
    "CL_INVALID_EVENT_WAIT_LIST",         //-57
    "CL_INVALID_EVENT",                   //-58
    "CL_INVALID_OPERATION",               //-59
    "CL_INVALID_GL_OBJECT",               //-60
    "CL_INVALID_BUFFER_SIZE",             //-61
    "CL_INVALID_MIP_LEVEL",               //-62
    "CL_INVALID_GLOBAL_WORK_SIZE"};       //-63


//! Allocate a buffer on device pinning the host memory at host_ptr
/*!
    \param mem_size Size of memory in bytes
    \return Returns a cl_mem object that points to pinned memory on the host
*/
cl_mem ad_allocBufferPinned(size_t mem_size,cl_context ctx )
{
    cl_mem mem;
    cl_int status;

   	mem = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                         mem_size, NULL, &status);


    ad_errChk(status, "Error allocating pinned memory", true);

    return mem;
}


//! OpenCl Error checker
/*!
Checks for error code as per cl_int returned by OpenCl
\param status Error value as cl_int
\param msg User provided error message
\return True if Error Seen, False if no error
*/
int ad_errChk(const cl_int status, const char * msg, bool exitOnErr)
{

    if(status != CL_SUCCESS) {
        printf("OpenCL Error: %d %s %s\n", status, cl_err_msg[-status], msg);

        if(exitOnErr) {
            exit(-1);
        }

        return true;
    }
    return false;
}

bool check_for_extensions(char * ext_name, int deviceId, cl_device_id * device_list)
{
    char* deviceExtensions = NULL;;
    size_t extStringSize = 0;
    cl_int status;

    // Get device extensions
    status = clGetDeviceInfo(device_list[deviceId],  CL_DEVICE_EXTENSIONS,
                                0, deviceExtensions,  &extStringSize);
    ad_errChk(status,"Getting Device Extension Length");
    deviceExtensions = new char[extStringSize];

    if(NULL == deviceExtensions)
        printf("Failed to allocate memory(deviceExtensions)");

    status = clGetDeviceInfo(device_list[deviceId], CL_DEVICE_EXTENSIONS,
                            extStringSize, deviceExtensions, NULL);

    if(ad_errChk(status, "clGetDeviceInfo failed.(CL_DEVICE_EXTENSIONS)")) exit(1);

    // Check if byte-addressable store is supported
    if(!strstr(deviceExtensions, ext_name))
    {
         printf("Device does not support %s extension!",ext_name);
         delete deviceExtensions;
         return 0;
    }
    else
    {
        printf("%s Supported\n",ext_name);
        delete deviceExtensions;
        return 1;
    }
}


uint get_num_devices(cl_platform_id ip_platform)
{
	uint numDevices;
	cl_int status;
	status = clGetDeviceIDs(ip_platform, CL_DEVICE_TYPE_ALL,
                            0, NULL, &numDevices);

	if(ad_errChk(status,"Error in Getting Device Count\n"))
		exit(1);

    //print_logging("\n%d devices for platform \n",(numDevices));
	if(ad_errChk(status,"Error in Getting Devices\n"))
		exit(1);
	return numDevices ;
}


void list_all_opencl_devices()
{
	cl_int status;
	// Used to iterate through the platforms and devices, respectively
	cl_uint numPlatforms;
	cl_uint numDevices;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("Number of platforms detected:%d\n", numPlatforms);

	// Print some information about the available platforms
	cl_platform_id * platforms = NULL;
	cl_device_id * devices = NULL;
	if (numPlatforms > 0)
	{
		// get all the platforms
		platforms = (cl_platform_id*)malloc(numPlatforms *
			sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);

		// Traverse the platforms array

		for(unsigned int i = 0; i < numPlatforms ; i++)
		{
			char pbuf[100];
			printf("Platform %d:\t", i);
			status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,
				sizeof(pbuf), pbuf, NULL);
			printf("Vendor: %s\n", pbuf);

			//unsigned int numDevices;

			status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
			if(ad_errChk(status, "checking for devices"))
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
				if(ad_errChk(status, "getting device IDs")) {
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
				ad_errChk(status, "Getting Device Info\n");
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
}



//-------------------------------------------------------
//          Synchronization functions
//-------------------------------------------------------

/*!
    Wait till all pending commands in queue are finished
*/
void ad_sync(cl_command_queue commandQueue)
{
    clFinish(commandQueue);
}



//! \return the next highest value such that x/y = 0
// This function that takes a positive integer 'value' and returns
// the nearest multiple of 'multiple' (used for padding columns)
unsigned int idivup(unsigned int value, unsigned int multiple) {

   unsigned int remainder = value % multiple;

   // Make the value a multiple of multiple
   if(remainder != 0) {
      value += (multiple-remainder);
   }

   return value;
}

//! A wrapper for malloc that checks the return value
void* ad_malloc(size_t size) {

    void* ptr = NULL;
    ptr = malloc(size);
    if(ptr == NULL) {
        perror("malloc");
        exit(-1);
    }

    return ptr;
}


char* ad_getDeviceName(cl_device_id dev)
{
    cl_int status;
    size_t devInfoSize;
    char* devInfoStr = NULL;


    // Print the name
    status = clGetDeviceInfo(dev, CL_DEVICE_NAME, 0,
        NULL, &devInfoSize);
    ad_errChk(status, "Getting device name", true);

    devInfoStr = (char*)ad_malloc(devInfoSize);

    status = clGetDeviceInfo(dev, CL_DEVICE_NAME, devInfoSize,
        devInfoStr, NULL);
    ad_errChk(status, "Getting device name", true);

    //This will be a memory leak if not freed by caller
    return(devInfoStr);
}


//! Set an argument for a OpenCL kernel
/*!

\param kernel The kernel for which the argument is being set
\param index The argument index
\param size The size of the argument
\param data A pointer to the argument
*/
void ad_setKernelArg(cl_kernel kernel, unsigned int index, size_t size,
                     void* data)
{
    cl_int status;
    status = clSetKernelArg(kernel, index, size, data);
    if(status != CL_SUCCESS)
    	printf("Error in Arg No %d\t",index);
    ad_errChk(status, "Setting kernel arg", true);
}


char* createFilenameWithTimestamp(char * prefix_str)
{
    int maxStringLen = 100;
    char* timeStr = new char[maxStringLen];

    time_t rawtime;
    struct tm* timeStruct;

    time(&rawtime);
    timeStruct = localtime(&rawtime);

    strftime(timeStr, maxStringLen, "/Events_%Y_%m_%d_%H_%M_%S.eventlog", timeStruct);

    return timeStr;
}


char* createFilenameWithTimestamp()
{
    int maxStringLen = 100;
    char* timeStr = new char[maxStringLen];

    time_t rawtime;
    struct tm* timeStruct;

    time(&rawtime);
    timeStruct = localtime(&rawtime);

    strftime(timeStr, maxStringLen, "/Events_%Y_%m_%d_%H_%M_%S.eventlog", timeStruct);

    return timeStr;
}

//! A general compiler driver that returns a program
cl_program cl_CompileProgram_from_array(char * source,
							char * compileoptions, bool verbosebuild,
							cl_context ip_ctx,
							cl_device_id ip_device)
{
	cl_program op_prog;

    printf("\nOpencl compiler driver - General\n");

    cl_int status = CL_SUCCESS;
    //char *source = cl_ReadSrcFile(kernelPath);

    //printf("source:%s",source);
    //printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    //op_prog = (cl_program * )malloc(sizeof(cl_program)*1);

    op_prog = clCreateProgramWithSource(ip_ctx, 1,
            (const char **)&source, NULL, &status);

	status = clBuildProgram(op_prog, 0, NULL,NULL, NULL, NULL);
	if(ad_errChk(status, "creating program")) {
            printf("status of build %d\n",status);

	}

    free(source);
    if(ad_errChk(status, "building program") )
    {

        cl_build_status build_status;

        clGetProgramBuildInfo( op_prog, ip_device, CL_PROGRAM_BUILD_STATUS,
            sizeof(cl_build_status), &build_status, NULL);

        if(build_status == CL_SUCCESS )
        {
            printf("Built program properly");
            return op_prog;
        }

        //char *build_log;
        size_t ret_val_size;
        //printf("Device: %p",topo->devices[0]);
        clGetProgramBuildInfo(op_prog, ip_device, CL_PROGRAM_BUILD_LOG, 0,
            NULL, &ret_val_size);
        printf("Build log size %d\n",ret_val_size);
        char *build_log = (char *) malloc(ret_val_size+1);
        if(build_log == NULL){ printf("Couldnt Allocate Build Log of Size %d \n",ret_val_size); exit(1);}

        clGetProgramBuildInfo(op_prog, ip_device, CL_PROGRAM_BUILD_LOG,
            ret_val_size+1, build_log, NULL);

        //printf("After build log call\n");
        // to be careful, terminate with \0
        // there's no information in the reference whether the string is 0
        // terminated or not
        build_log[ret_val_size] = '\0';

        printf("Build log:\n %s...\n", build_log);
        if(build_status != CL_SUCCESS)
        	exit(1);

    }
    return op_prog;
}


//! A general compiler driver that returns a program
cl_program cl_CompileProgram(char * kernelPath,
							char * compileoptions, bool verbosebuild,
							cl_context ip_ctx,
							cl_device_id ip_device)
{
	cl_program op_prog;

    printf("\nOpencl compiler driver - General\n");

    cl_int status = CL_SUCCESS;
    char *source = cl_ReadSrcFile(kernelPath);

    //printf("source:%s",source);
    //printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    //op_prog = (cl_program * )malloc(sizeof(cl_program)*1);

    op_prog = clCreateProgramWithSource(ip_ctx, 1,
            (const char **)&source, NULL, &status);
	if(ad_errChk(status, "creating program from source")) {
            printf("status of build %d\n",status);

	}

	status = clBuildProgram(op_prog, 1, &ip_device,NULL, NULL, NULL);
	if(ad_errChk(status, "clBuildProgram call")) {
            printf("status of build %d\n",status);

	}

    free(source);
    if(ad_errChk(status, "building program") )
    {

        cl_build_status build_status;

        clGetProgramBuildInfo( op_prog, ip_device, CL_PROGRAM_BUILD_STATUS,
            sizeof(cl_build_status), &build_status, NULL);

        if(build_status == CL_SUCCESS )
        {
            printf("Built program properly");
            return op_prog;
        }

        //char *build_log;
        size_t ret_val_size=0;
        //printf("Device: %p",topo->devices[0]);
        clGetProgramBuildInfo(op_prog, ip_device, CL_PROGRAM_BUILD_LOG, 0,
            NULL, &ret_val_size);
        printf("Build log size %d\n",ret_val_size);
        char *build_log = (char *) malloc(ret_val_size+1);
        if(build_log == NULL){ printf("Couldnt Allocate Build Log of Size %d \n",ret_val_size); exit(1);}

        clGetProgramBuildInfo(op_prog, ip_device, CL_PROGRAM_BUILD_LOG,
            ret_val_size+1, build_log, NULL);

        //printf("After build log call\n");
        // to be careful, terminate with \0
        // there's no information in the reference whether the string is 0
        // terminated or not
        build_log[ret_val_size] = '\0';

        printf("Build log:\n %s...\n", build_log);
        if(build_status != CL_SUCCESS)
        	exit(1);

    }
    return op_prog;
}
