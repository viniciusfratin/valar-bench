#include <CL/cl.h>
#include "stdio.h"
#include "stdlib.h"

#include "opencl_utils.h"

#include "fission_topology.h"

/**
 * Used to return device used
 */
cl_device_id fission_topology::get_device_used(int ip )
{
	if (ip == 0)
	{
		printf("\nDevice Used = %d", device_used);
		return devices[device_used];
	}
	else
		return devices[ip];
}

void fission_topology::add_problem_subqueue()
{

}

fission_topology::fission_topology()
{
    last_assigned_queue = 0;
    fission_scheduler = ROUND_ROBIN;
    numSubDevices = 0;
    n_devices_requested = 0;
}


queue_id fission_topology::schedule_kernel()
{
    if(fission_scheduler == ROUND_ROBIN)
    {
        if(last_assigned_queue >= (numSubDevices - 1))
        {
            printf("Loopback ---- returning queue 0\n");
            last_assigned_queue = 0;
            return 0;
        }
        else
        {
            last_assigned_queue = last_assigned_queue+1;
            printf("Now returning queue %d\n",last_assigned_queue  );
            return (last_assigned_queue);
        }
    }
    else
    {
        printf("no other schedulers available for now....Dying\n");
        exit(1);
    }

}

cl_command_queue fission_topology::return_subqueue(int i)
{
	return subQueue[i];

}
/**
    \short Return a commamd queue to the calling application
**/
cl_command_queue fission_topology::return_subqueue()
{
    if(fission_scheduler == ROUND_ROBIN)
    {
        if(last_assigned_queue >= (numSubDevices-1))
        {
            printf("Loopback ---- returning queue 0\n");
            last_assigned_queue = 0;
            return subQueue[0];
        }
        else
        {
            last_assigned_queue = last_assigned_queue+1;
            printf("Now returning queue %d\n",last_assigned_queue  );
            return subQueue[last_assigned_queue];
        }
    }
    else
    {
        printf("no other schedulers available for now....Dying\n");
        exit(1);
    }

}


cl_command_queue fission_topology :: return_cpu_queue()
{
    /**
    Warning: If the number of queues is more than two then the
    load on the system is unbalanced becuase always the same cpu or
    the same GPU queue will be returned and if you have greater than 2 queues
    some queue will be totally idle
    **/
    if(numRootDevices > 2)
        printf("Performance may not be great\t See Source Code\n");
    return rootQueue[cpu_queue_no];

}


cl_command_queue fission_topology :: return_gpu_queue()
{
    if(numRootDevices > 2)
        printf("Performance may not be great\t See Source Code\n");
    return rootQueue[gpu_queue_no];
}

/*!

\brief Compile Opencl source file into a cl_program.
The cl_program will be made into a kernel in Kernel_Precompile

Convert source code file into cl_program

\param kernelPath  Filename of OpenCl code
\param compileoptions Compilation options
\param verbosebuild Switch to enable verbose Output
*/
void fission_topology::cl_CompileProgramRootDevices(char * kernelPath,
                                        char * compileoptions, bool verbosebuild)
{


    printf("\nOpencl compiler driver - Root Device\n");

    cl_int status;
    FILE *fp;
    char *source;
    long int size;

    printf("Compiler Function: Kernel file is: %s\n", kernelPath);

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
    //printf("source:%s",source);
    //printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    printf("num root devices %d \n", numRootDevices);
    root_program = (cl_program * )malloc(sizeof(cl_program)*numRootDevices);
    if(root_program == NULL)
    	printf("malloc error");
    for(int i=0; i< numRootDevices; i++)
    {
    	printf("compiling root program\n");

        root_program[i] = clCreateProgramWithSource(root_context, 1,
            (const char **)&source, NULL, &status);

        status = clBuildProgram(root_program[i], 0, NULL,NULL, NULL, NULL);
        if(ad_errChk(status, "creating program")) {
            printf("status of build %d\n",status);
            break;
            //Get out of the loop to print log and die
        }

    }

    free(source);
    fclose(fp);
    if(ad_errChk(status, "building program") )
    {
        cl_build_status build_status;
        clGetProgramBuildInfo( root_program[0], devices[0], CL_PROGRAM_BUILD_STATUS,
            sizeof(cl_build_status), &build_status, NULL);

        if(build_status == CL_SUCCESS )
        {
            printf("Built program properly");
            return;
        }

        //char *build_log;
        size_t ret_val_size;
        //printf("Device: %p",topo->devices[0]);
        clGetProgramBuildInfo(root_program[0], devices[0], CL_PROGRAM_BUILD_LOG, 0,
            NULL, &ret_val_size);
        printf("Build log size %d\n",ret_val_size);
        char *build_log = (char *) malloc(ret_val_size+1);
        if(build_log == NULL){ printf("Couldnt Allocate Build Log of Size %d \n",ret_val_size); exit(1);}

        clGetProgramBuildInfo(root_program[0], devices[0], CL_PROGRAM_BUILD_LOG,
            ret_val_size+1, build_log, NULL);

        // to be careful, terminate with \0
        // there's no information in the reference whether the string is 0
        // terminated or not
        build_log[ret_val_size] = '\0';

        printf("Build log:\n %s...\n", build_log);
        if(build_status != CL_SUCCESS)
        {
            exit(1);
        }
    }
    printf("Leaving RootDevice Compiler Driver\n");
}

/**
 * Compile root_program for the k subDevice.
 * Assumes root_program is correctly compiled
  */
void fission_topology::cl_CompileProgramSubDevice(char * kernelPath,
							char * compileoptions, bool verbosebuild)
{

    printf("\nOpencl compiler driver - Subdevices\n");

    cl_int status = CL_SUCCESS;
    char *source = cl_ReadSrcFile(kernelPath);

    //printf("source:%s",source);
    //printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    sub_program= (cl_program * )malloc(sizeof(cl_program)*numSubDevices);
    if(root_program == NULL)
    	printf("malloc error");
    printf("No of subdevices %d\n",numSubDevices);
    for(int i=0; i< numSubDevices; i++)
    {
    	printf("compiling Subprogram\n");

        sub_program[i] = clCreateProgramWithSource(subContext, 1,
            (const char **)&source, NULL, &status);

        status = clBuildProgram(sub_program[i], 0, NULL,NULL, NULL, NULL);
        if(ad_errChk(status, "creating program")) {
            printf("status of build %d\n",status);
            break;
            //Get out of the loop to print log and die
        }

    }

    free(source);
    if(ad_errChk(status, "building program") )
    {

        cl_build_status build_status;

        clGetProgramBuildInfo( sub_program[0], subDevices[0], CL_PROGRAM_BUILD_STATUS,
            sizeof(cl_build_status), &build_status, NULL);

        if(build_status == CL_SUCCESS )
        {
            printf("Built program properly");
            return;
        }

        //char *build_log;
        size_t ret_val_size;
        //printf("Device: %p",topo->devices[0]);
        clGetProgramBuildInfo(sub_program[0], subDevices[0], CL_PROGRAM_BUILD_LOG, 0,
            NULL, &ret_val_size);
        printf("Build log size %d\n",ret_val_size);
        char *build_log = (char *) malloc(ret_val_size+1);
        if(build_log == NULL){ printf("Couldnt Allocate Build Log of Size %d \n",ret_val_size); exit(1);}

        clGetProgramBuildInfo(sub_program[0], subDevices[0], CL_PROGRAM_BUILD_LOG,
            ret_val_size+1, build_log, NULL);

        //printf("After build log call\n");
        // to be careful, terminate with \0
        // there's no information in the reference whether the string is 0
        // terminated or not
        build_log[ret_val_size] = '\0';

        printf("Build log:\n %s...\n", build_log);
        if(build_status != CL_SUCCESS) exit(1);

    }
    printf("Leaving SubDevice Compiler Driver\n");

}

