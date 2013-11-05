/*
 * FIR.c
 *
 *  Created on: Dec 1, 2011
 *      Author: ukidaveyash
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <CL/cl.h>
 
#include "opencl_utils.h"
#include "FIR_tapchange.h"
#include "analysis-devices.h"
#include "tap-change-device.h"
#include "eventlist.h"

//#include "fir.pb.h"

#ifdef GPUPROF
#include "inc/GPUPerfAPI.h"
#include <dlfcn.h>
#endif


#define MAX_SOURCE_SIZE 10000L
#define OUTPUT 0 						//Macro maintained to print output result
										// 1 - Print output results
										// 0 - Hide output results

#define CHECK_STATUS( status, message )   \
		if(status != CL_SUCCESS) \
		{ \
			printf( message); \
			printf( "\n" ); \
			return 1; \
		}




#ifdef GPUPROF
char* createFilenameWithTimestamp();
void WriteSession( gpa_uint32 currentWaitSessionID, char* filename );
#endif


cl_uint numTap = 0;
cl_uint numData = 0;		// Block size
cl_uint numTotalData = 0;
cl_uint numBlocks = 0;
cl_uint dispatchSize = 0;
cl_uint dispatchData = 0;
cl_float* input = NULL;
cl_float* output = NULL;
cl_float* coeff = NULL;
cl_float* temp_output = NULL;

cl_command_queue command_queue ;

cl_command_queue cl_getfircq()
{
	return command_queue;
}


void add_timing_delta(cl_uint timing_rate)
{
	usleep(timing_rate*1000);
}

int main(int argc , char** argv) {


	/** Define Custom Variables */
	int i,count;
	int local;

	if (argc < 6)
	{
		printf(" Usage : ./<path to binary> <numBlocks> <numData> <Dispatch_size> <Tap Change Iterations> <Tap Change Interval> \n");
		exit(0);
	}
	int ip_change_interval = 20;
	if (argc > 1)
	{
		numBlocks = atoi(argv[1]);
		numData = atoi(argv[2]);		
		dispatchSize = atoi(argv[3]);
		ip_change_interval = atoi(argv[5]);
	}
	if(dispatchSize > numBlocks)
	{
		printf("Warning: Dispatch size is greater than the number of blocks\n"
			"Using numBlocks as the dispatch size\n");
		dispatchSize = numBlocks;
	}
	tap_change_device * tcontrol = new tap_change_device[1];
    tcontrol->change_interval = ip_change_interval;
    
	/** Declare the Filter Properties */
	numTap = 4096;
	numTotalData = numData * numBlocks;
	dispatchData = numData * dispatchSize;
	local = dispatchData/(32*8);			// Variable maintained to make 8 warps per Local group
	printf("FIR Filter\n Data Samples : %d \n NumBlocks : %d \n Local Workgroups : %d\n", numData,numBlocks,local);

	/** Define variables here */
	input = (cl_float *) malloc( numTotalData* sizeof(cl_float) );
	output = (cl_float *) malloc( numTotalData* sizeof(cl_float) );
	coeff = (cl_float *) malloc( numTap* sizeof(cl_float) );
	temp_output = (cl_float *) malloc( (dispatchData+numTap-1) * sizeof(cl_float) );

	/** Initialize the input data */
	for( i=0;i<numTotalData;i++ )
	{
		input[i] = 8;
		output[i] = 99;
	}

	for( i=0;i<numTap;i++ )
		coeff[i] = 100.0/numTap;

	for( i=0; i<(dispatchData+numTap-1); i++ )
		temp_output[i] = 0.0;


	// Event Creation
	cl_event event;
	EventList* eventList;


	// Read the input file
	FILE *fip;
	i=0;
	fip = fopen("temp_in.dat","r");
	if (fip == NULL) printf("not good \n");
	while(i<numTotalData)
	{
		fscanf(fip,"%f",&input[i]);
		i++;
	}
	fclose(fip);


	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("FIR.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose( fp );

	// Get platform and device information
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	//ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1,
	//		&device_id, &ret_num_devices);

	printf("\n No of Platforms %d",ret_num_platforms );

	char *platformVendor;
	size_t platInfoSize;
	clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, 0, NULL,
			&platInfoSize);

	platformVendor = (char*)malloc(platInfoSize);

	clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, platInfoSize,
			platformVendor, NULL);
	printf("\tVendor: %s\n", platformVendor);
	free(platformVendor);

	printf("Get Number of Devices\n");
	cl_device_id * device_list;

	uint numDevices;
	cl_int status;
	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL,
                            0, NULL, &numDevices);
	CHECK_STATUS( status,"Error: (clGetDeviceIDs)\n");
	printf("num devices %d\n",numDevices);
	device_list = (cl_device_id * )malloc(sizeof(cl_device_id)*numDevices);
	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL,
	                            numDevices, device_list, NULL);

	for(int i=0;i<numDevices;i++)
		printf("Device Name %s\n",ad_getDeviceName(device_list[i]));

	// Create an OpenCL context
	//cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
	cl_context context = clCreateContext( NULL, numDevices, device_list, NULL, NULL, &ret);
	CHECK_STATUS( ret,"Error: (clCreateContext)\n");

	int device_to_use = 999;

#ifdef COMPUTE_GPU
	device_to_use =  0;
#endif

#ifdef COMPUTE_CPU
	device_to_use =  1;
#endif

	device_id = device_list[device_to_use];


	// Create a command queue
	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
	CHECK_STATUS( ret,"clCreateCommandQueue\n");

	// Time of day calculation starts
	struct timeval start,end;
	gettimeofday(&start,NULL);



#ifdef GPUPROF
	// Create performance counter Init
	GPA_Initialize();
	GPA_OpenContext( command_queue );
	GPA_EnableAllCounters();
#endif

	// Create memory buffers on the device for each vector
	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(cl_float) * dispatchData, NULL, &ret);
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(cl_float) * dispatchData, NULL, &ret);
	cl_mem coeffBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
			sizeof(cl_float) * numTap, NULL, &ret);
	cl_mem temp_outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(cl_float) * (dispatchData+numTap-1), NULL, &ret);

	// Create EventList for Timestamps
	eventList = new EventList(context, command_queue, device_id,true);

	// Tap Change Device Control

	/*int num_iterations_tcontrol = 100000;*/
	int num_iterations_tcontrol = atoi(argv[3]);
	tcontrol->init_tap_change_device(context,numTap, num_iterations_tcontrol );

	//!Stuff for Value Checker
	tcontrol->init_value_checker(command_queue,context,device_id);
	tcontrol->set_threshold(1024.0f, temp_outputBuffer, 0);

	//! Stuff for TAP CHANGE
	tcontrol->init_app_profiler(eventList);
	tcontrol->build_analysis_kernel("tap-change-kernel.cl","tap_change_kernel",0);
	tcontrol->configure_analysis_kernel();


	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1,
			(const char **)&source_str, (const size_t *)&source_size, &ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	CHECK_STATUS( ret,"Error: Build Program\n");

	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "FIR", &ret);
	CHECK_STATUS( ret,"Error: Create kernel. (clCreateKernel)\n");


	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&outputBuffer);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&coeffBuffer);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&temp_outputBuffer);
	ret = clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&numTap);

	tcontrol->profiler->resetClocks();
	eventList->resetClocks();


	// Initialize Memory Buffer
	ret = clEnqueueWriteBuffer(command_queue,
			coeffBuffer,
			1,
			0,
			numTap * sizeof(cl_float),
			coeff,
			0,
			0,
			&event);

	eventList->add(event);

	ret = clEnqueueWriteBuffer(command_queue,
			temp_outputBuffer,
			1,
			0,
			(numTap) *sizeof(cl_float),
			temp_output,
			0,
			0,
			&event);

	eventList->add(event);
	ret = clFinish(command_queue);


	// Decide the local group formation
	size_t globalThreads[1]={dispatchData};
	size_t localThreads[local];
	for (i=0;i<local;i++)
		localThreads[i]=(dispatchData/local);
	cl_command_type cmdType;
	count = 0;
    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

	while( count < (numBlocks/dispatchSize))
	{

		/* fill in the temp_input buffer object */
		//for(int z=0 ; z<20; z++)
		//	printf("Input data %f\n", *(input + (count * dispatchData) + z) );
		//for (int u = 0; u < dispatchSize; u++)
		//{
		//	add_timing_delta(timing_rate);
		//	memcpy(staging_buffer,&input[count*dispatchSize],numData*sizeof(float));
		//}

		ret = clEnqueueWriteBuffer(command_queue,
				temp_outputBuffer,
				CL_FALSE,
				(numTap-1)*sizeof(cl_float),
				dispatchData * sizeof(cl_float),
				input + (count * dispatchData),
				0,
				0,
				&event);

		eventList->add(event);



#if GPUPROF


		//Session Creation
		static gpa_uint32 currentWaitSessionID = 1;
		gpa_uint32 sessionID;
		GPA_BeginSession( &sessionID );

		// Required number of passes can be custom
		gpa_uint32 numRequiredPasses = 1;


		GPA_GetPassCount( &numRequiredPasses );
		for ( gpa_uint32 i = 0; i < numRequiredPasses; i++ )
		{
			GPA_BeginPass();
			GPA_BeginSample( 0 );

#endif

			// Execute the OpenCL kernel on the list
			ret = clEnqueueNDRangeKernel(
					command_queue,
					kernel,
					1,
					NULL,
					globalThreads,
					localThreads,
					0,
					NULL,
					&event);

			//clFinish(command_queue);
			//tcontrol->check_value();
			tcontrol->add_phase(count);
			//printf("OPENCL BUFFER USED  - FIR CODE %p\n",coeffBuffer);

			//	tcontrol->inject_analysis(0);

			//	clFlush(command_queue);
			CHECK_STATUS( ret,"Error: Range kernel. (clCreateKernel)\n");
			//ret = clWaitForEvents(1, &event);
			//ret = clWaitForEvents(1, &event);

#if GPUPROF
			// End Profile session
			GPA_EndSample();
			GPA_EndPass();

			eventList->add(event);
		}
		GPA_EndSession();

		bool readyResult = true;
		if ( sessionID != currentWaitSessionID )
		{
			GPA_Status sessionStatus;
			sessionStatus = GPA_IsSessionReady( &readyResult,
					currentWaitSessionID );
			while ( sessionStatus == GPA_STATUS_ERROR_SESSION_NOT_FOUND )
			{
				// skipping a session which got overwritten
				currentWaitSessionID++;
				sessionStatus = GPA_IsSessionReady( &readyResult,
						currentWaitSessionID );
			}
		}
		if ( readyResult )
		{
			WriteSession( currentWaitSessionID,
					"Results1.log" );
			currentWaitSessionID++;
		}
		//Cleanup
		GPA_CloseContext();

#endif

		eventList->add(event);

		/* Get the output buffer */
		ret = clEnqueueReadBuffer(
				command_queue,
				outputBuffer,
				CL_FALSE,
				0,
				dispatchData * sizeof( cl_float ),
				output + count * dispatchData,
				0,
				NULL,
				&event );
		eventList->add(event);
		count ++;

		//clFinish(command_queue);
	}	//End Loop running FIR
	clFinish(command_queue);

	gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("Loop elapsedTime\t %f ms \t Time per Block %f ms\n",elapsedTime,elapsedTime/numBlocks);

#if OUTPUT
	printf("\n The Output:\n");
	i = 0;
	while( i<numTotalData )
	{
		printf( "%f ", output[i] );

		i++;
	}
#endif
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(inputBuffer);
	ret = clReleaseMemObject(outputBuffer);
	ret = clReleaseMemObject(coeffBuffer);
	ret = clReleaseMemObject(temp_outputBuffer);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	ad_errChk(ret,"cleaning up");
	free(input);
	free(output);
	free(coeff);
	free(temp_output);


	
	
#ifdef GPUPROF
	GPA_Destroy();
#endif

	
	//eventList->printEvents();
	eventList->dumpEvents("eventDumps");
	tcontrol->sync();
	tcontrol->profiler->dumpEvents("eventDumps_ad");
	delete eventList;

	// Time of day calculation end

	//gettimeofday(&end,NULL);


	//printf("\nInfo,%d,%d,%f\n",atoi(argv[2]),atoi(argv[3]),(double)((end.tv_sec*1000000 + end.tv_usec)-(start.tv_sec*1000000 + start.tv_usec))/1000.0);
	fflush(NULL);


	return 0;
}

#ifdef GPUPROF
// Perfoemance API write function
void WriteSession( gpa_uint32 currentWaitSessionID,  char* filename )
{
	static bool doneHeadings = false;
	gpa_uint32 count;
	GPA_GetEnabledCount( &count );
	FILE* f;
	if ( !doneHeadings )
	{
		const char* name;
		f = fopen( filename, "w+" );
		assert( f );
		fprintf( f, "Identifier, " );
		for ( gpa_uint32 counter = 0 ; counter < count ; counter++ )
		{
			gpa_uint32 enabledCounterIndex;
			GPA_GetEnabledIndex( counter, &enabledCounterIndex );
			GPA_GetCounterName( enabledCounterIndex, &name );
			fprintf( f, "%s, ", name );
		}
		fprintf( f, "\n" );
		fclose( f );
		doneHeadings = true;
	}
	f = fopen( filename, "a+" );
	assert( f );
	gpa_uint32 sampleCount;
	GPA_GetSampleCount( currentWaitSessionID, &sampleCount );
	for ( gpa_uint32 sample = 0 ; sample < sampleCount ; sample++ )
	{
		fprintf( f, "session: %d; sample: %d, ", currentWaitSessionID,
				sample );
		for ( gpa_uint32 counter = 0 ; counter < count ; counter++ )
		{
			gpa_uint32 enabledCounterIndex;
			GPA_GetEnabledIndex( counter, &enabledCounterIndex );
			GPA_Type type;
			GPA_GetCounterDataType( enabledCounterIndex, &type );
			if ( type == GPA_TYPE_UINT32 )
			{
				gpa_uint32 value;
				GPA_GetSampleUInt32( currentWaitSessionID,
						sample, enabledCounterIndex, &value );
				fprintf( f, "%u,", value );
			}
			else if ( type == GPA_TYPE_UINT64 )
			{
				gpa_uint64 value;
				GPA_GetSampleUInt64( currentWaitSessionID,
						sample, enabledCounterIndex, &value );
				fprintf( f, "%I64u,", value );
			}
			else if ( type == GPA_TYPE_FLOAT32 )
			{
				gpa_float32 value;
				GPA_GetSampleFloat32( currentWaitSessionID,
						sample, enabledCounterIndex, &value );
				fprintf( f, "%f,", value );
			}
			else if ( type == GPA_TYPE_FLOAT64 )
			{
				gpa_float64 value;
				GPA_GetSampleFloat64( currentWaitSessionID,
						sample, enabledCounterIndex, &value );
				fprintf( f, "%f,", value );
			}
			else
			{
				assert(false);
			}
		}
		fprintf( f, "\n" );
	}
	fclose( f );
}
#endif

