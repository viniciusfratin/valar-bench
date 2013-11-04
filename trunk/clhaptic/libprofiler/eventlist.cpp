#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <CL/cl.h>
#include <time.h>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#define snprintf sprintf_s
#define strdup _strdup;
#else
#include <sys/time.h>
#endif

#include "eventlist.h"

//! Default Constructor added to ENABLE record_profiling flag by default
EventList::EventList()
{
	profiler_name = "Default Profiler";
    //! Done not to mess with Dana's interfaces
	record_profile = ENABLED;
	//! Initializing to 0 so that you atleast get only offseted data if the resetClocks() function isnt called
	gpu_timer_start = 0;
	cpu_timer_start = 0;
}
char * EventList::get_profiler_name()
{
	return profiler_name;
}

//! Constructor
EventList::EventList(cl_context context, cl_command_queue commandQueue,
                     cl_device_id device, bool free_events, char * name)
{
	if(name == NULL )
	{
		printf("No name for profiler \n");
		profiler_name = "Default Profiler";
	}
	else
	{
		profiler_name = (char *)malloc(strlen(name)*sizeof(char));
		strcpy(profiler_name,name);
		printf("Profiler name %s \n",profiler_name);
		//profiler_name = name;
	}
	printf("Initializing the OpenCL Profiler \n");
    // We need to record some OpenCL state to get the timer values later
    this->context = context;
    this->commandQueue = commandQueue;
    this->device = device;

    // Determines whether or not we will free events when this object
    // is deleted
    this->free_events = free_events;

    // Synchronize the CPU and GPU clocks and use the value as time-zero
    printf("reset clocks\n");
    this->resetClocks();

    //!EventList::record_profile ENABLED by default so as not to mess with Dana's interfaces
    record_profile = ENABLED;

    //! Random small window to check time difference
    config.time_threshold = DEFAULT_TIME_COMPARE_THRESHOLD;

    //! Change this parameter to DISABLED to shut out all  the PhaseControl stuff
    config.record_phases = ENABLED;
	config.verbose_messages = ENABLED;
	config.max_disabled_duration = 5;
	config.record_phases = ENABLED;
}

//! Enable Profiling flags.
//! Changes the record_profile to ENABLED or DISABLED
//! \param inputflag Required status of profiler
void EventList::setProfilingStatus(bool inputflag)
{
	record_profile = inputflag;
}

//! Return Profiling Status
bool EventList::getProfilingStatus()
{
 return record_profile;
}

//! Destructor
EventList::~EventList()
{
    // Free the events only if the user did not specify otherwise
    if(this->free_events) {
        // Release OpenCL events
        for(int i = 0; i < (int)(this->event_list.size()); i++) {
            clReleaseEvent(this->event_list[i]->event);
        }
        // Release user events
        for(int i = 0; i < (int)(this->user_event_list.size()); i++) {
            delete this->user_event_list[i]->event;
        }
    }

    // Free all strings
    for(int i = 0; i < (int)(this->event_list.size()); i++) {
        delete this->event_list[i]->name;
        delete this->event_list[i]->type;
    }
    for(int i = 0; i < (int)(this->user_event_list.size()); i++) {
        delete this->user_event_list[i]->name;
        delete this->user_event_list[i]->type;
    }
}

// Add an OpenCL event to the OpenCL event list
void EventList::add(cl_event event, const char* name, const char* type)
{

	//! Only adds a event to the EventList::event_list members if EventList::record_profile is ENABLED
	if(record_profile == ENABLED)
	{
		//printf("add a event %d \n",foo-1);
		control.add_event();
		// Count of events (not thread safe)
		static unsigned short eventCtr = 0;  // Max of 5 characters

		// Triple for this event and its info
		event_triple triple = new _event_triple;

		// Determine the event name based on the 'name' parameter
		char* eventName;
		if(name == NULL) {
			eventName = new char[11];
			snprintf(eventName, 11, "Event%u", eventCtr);
			eventName[10] = '\0';
		}
		else {
			eventName = EventList::strCopy(name);
		}

		// Copy the event type if it was provided, otherwise generate one
		// automatically based on the command type
		char* eventType = NULL;
		if(type == NULL) {

			// No event type supplied

			cl_int status;
			cl_command_type cmdType;

			// Query OpenCL to get the command type
			status = clGetEventInfo(event, CL_EVENT_COMMAND_TYPE, sizeof(cl_command_type),
				&cmdType, NULL);
			if(status != CL_SUCCESS) {
				printf("Error getting event info\n");
				exit(-1);
			}

			// Create a string based on the command type
			switch(cmdType) {
				case CL_COMMAND_NDRANGE_KERNEL:
				case CL_COMMAND_TASK:
				case CL_COMMAND_NATIVE_KERNEL:
					eventType = EventList::strCopy("Kernel");
					break;
				case CL_COMMAND_READ_BUFFER:
				case CL_COMMAND_WRITE_BUFFER:
				case CL_COMMAND_COPY_BUFFER:
				case CL_COMMAND_READ_IMAGE:
				case CL_COMMAND_WRITE_IMAGE:
				case CL_COMMAND_COPY_IMAGE:
				case CL_COMMAND_COPY_BUFFER_TO_IMAGE:
				case CL_COMMAND_COPY_IMAGE_TO_BUFFER:
				case CL_COMMAND_MAP_BUFFER:
				case CL_COMMAND_MAP_IMAGE:
				case CL_COMMAND_UNMAP_MEM_OBJECT:
				case CL_COMMAND_READ_BUFFER_RECT:
				case CL_COMMAND_WRITE_BUFFER_RECT:
				case CL_COMMAND_COPY_BUFFER_RECT:
					eventType = EventList::strCopy("IO");
					break;
				case CL_COMMAND_MARKER:
					eventType = EventList::strCopy("Marker");
					break;
				case CL_COMMAND_ACQUIRE_GL_OBJECTS:
				case CL_COMMAND_RELEASE_GL_OBJECTS:
					eventType = EventList::strCopy("OpenGL");
					break;
				case CL_COMMAND_USER:
					eventType = EventList::strCopy("OpenCLUserEvent");
					break;
				default:
					eventType = EventList::strCopy("Unknown");
					break;
			}
		}
		else {
			// Copy the supplied event type
			eventType = EventList::strCopy(type);
		}

		// Populate the triple
		triple->event = event;
		triple->name = eventName;
		triple->type = eventType;

		// Add the event and data to the list
		this->event_list.push_back(triple);

		eventCtr++;
		//! Ideally this should be setup to return the event in this function
		//! because if we do nothing, to the cl_event passed to the kernel
		//! Also, that way we can return NULL for the else case
	}
	else
	{
		//if(config.verbose_messages == ENABLED)
		//	printf("Skipping Event Recording\n");
	}
}

// Add a user event to the user event list
void EventList::add(cl_user_event event, const char* name, const char* type) {

//! Enabled by constructor always, so it wont interfere
if(record_profile == ENABLED)
{
	control.add_user_event();

    // Count of user events (not thread safe)
    static unsigned short userEventCtr = 0;  // Max of 5 characters

    // Triple for this event and its info
    user_event_triple triple = new _user_event_triple;

    // Determine the event name based on the 'name' parameter
    char* eventName;
    if(name == NULL) {
        eventName = new char[15];
        snprintf(eventName, 15, "UserEvent%u", userEventCtr);
        eventName[14] = '\0';
    }
    else {
        eventName = EventList::strCopy(name);
    }

    // Copy the event type if it was provided, otherwise generate one
    // automatically based on the command type
    char* eventType = EventList::strCopy(type);

    // Populate the triple
    triple->event = event;
    triple->name = eventName;
    triple->type = eventType;

    // Add the event and data to the list
    this->user_event_list.push_back(triple);

    userEventCtr++;
}
else
	printf("Skipping this user event");

}

// Create a user event and return it
cl_user_event EventList::clCreateUserEvent() {

    // Create a new user event
    cl_user_event event = new _cl_user_event;
 
    return event;
}

// Set the status of a user event
cl_int EventList::clSetUserEventStatus(cl_user_event event, 
                                       cl_int execution_status) {

    // Record the current time for the execution status provided
    cl_time time = getCurrentTime();

    switch(execution_status) {
        case CL_QUEUED:
            event->start = time;
            break;
        case CL_SUBMITTED:
            event->queued = time;
            break;
        case CL_RUNNING:
            event->start = time;
            break;
        case CL_COMPLETE:
            event->end = time;
            break;
        default:
            perror("invalid user event status");
            exit(-1);
    }

    return 0;
}

int EventList::get_latest_phase_id()
{
	return control.get_latest_phase_no();

}

// Generate a filename based on the current time
char* EventList::createFilenameWithTimestamp() 
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

// Create a file and dump the event information
void EventList::dumpEvents(char* path, char * ip_filename)
{
    FILE* fp =  NULL;
    char * filename;
    // Construct a filename based on the current time
    if (ip_filename == NULL)
    	filename = this->createFilenameWithTimestamp();
    else
    {
    	filename =  (char *)malloc(strlen(ip_filename)*sizeof(char));
    	strcpy(filename, ip_filename);
    }

    // Create string to hold the path + filename
    char* fullpath = new char[strlen(path)+strlen(filename)+1];
 
    // Concat the path and filename into the destination string
    strcpy(fullpath, path);
    strcat(fullpath, filename);

    // Try to open the file for writing
    fp = fopen(fullpath, "w");
    if(fp == NULL) {
        printf("Error opening %s\n", fullpath);
        exit(-1);
    }

    // Write some information out about the environment
 
    cl_int status;
    size_t devInfoSize;
    char* devInfoStr = NULL;

    // Get the device name
    status = clGetDeviceInfo(this->device, CL_DEVICE_NAME, 0, NULL, 
        &devInfoSize);
    if(status != CL_SUCCESS) {
        printf("Device info failed\n");
        exit(-1);
    }
                
    devInfoStr = new char[devInfoSize];

    status = clGetDeviceInfo(this->device, CL_DEVICE_NAME, devInfoSize, 
        devInfoStr, NULL);
    if(status != CL_SUCCESS) {
        printf("Device info failed\n");
        exit(-1);
    }

    // Write the device name
    fprintf(fp, "Info;\t%s\n", devInfoStr);
    delete devInfoStr;
    devInfoStr = NULL;

    // Get the device driver version
    status = clGetDeviceInfo(this->device, CL_DRIVER_VERSION, 0, NULL, 
        &devInfoSize);	
    if(status != CL_SUCCESS) {
        printf("Device info failed\n");
        exit(-1);
    }

    devInfoStr = new char[devInfoSize];

    status = clGetDeviceInfo(this->device, CL_DRIVER_VERSION, devInfoSize, 			
        devInfoStr, NULL);		
    if(status != CL_SUCCESS) {
        printf("Device info failed\n");
        exit(-1);
    }

    // Write the device driver version
    fprintf(fp, "Info;\tDriver version %s\n", devInfoStr);
    delete devInfoStr;

    // Write the hostname
#ifndef _WIN32
    char hostname[50];
    if(gethostname(hostname, 50) != 0) {
        printf("Error getting hostname\n");
    }
    else {
        fprintf(fp, "Info;\tHost %s\n", hostname);
    }
#endif
printf("Timer start %lu \n",this->gpu_timer_start) ;
    // Write the events to file with each field separated by semicolons
    for(int i = 0; i < (int)this->event_list.size(); i++) {
//        fprintf(fp, "%s;\t%s;\t%f;\t%f;\t%f;\t%f\n",
	    fprintf(fp, "%s;\t%s;\t%llu;\t%llu;\t%llu;\t%llu\n",
            this->event_list[i]->type,
            this->event_list[i]->name, 
            long( getEventValue(this->event_list[i]->event,
                	CL_PROFILING_COMMAND_QUEUED) - this->gpu_timer_start ),
	    long( getEventValue(this->event_list[i]->event,
	                CL_PROFILING_COMMAND_SUBMIT) - this->gpu_timer_start ),
            long( getEventValue(this->event_list[i]->event,
        	        CL_PROFILING_COMMAND_START) - this->gpu_timer_start ),
            long( getEventValue(this->event_list[i]->event,
        	        CL_PROFILING_COMMAND_END) - this->gpu_timer_start )  
	);
/*
        printf("%s;\t%s;\t%lu;\t%lu;\t%lu;\t%lu\n", 
            this->event_list[i]->type,
            this->event_list[i]->name, 
            ulong(	getEventValue(this->event_list[i]->event, 
	                CL_PROFILING_COMMAND_QUEUED) - this->gpu_timer_start ),
            ulong(	getEventValue(this->event_list[i]->event, 
                	CL_PROFILING_COMMAND_SUBMIT) - this->gpu_timer_start ),
            ulong(	getEventValue(this->event_list[i]->event, 
	                CL_PROFILING_COMMAND_START) - this->gpu_timer_start ),
	    ulong(	getEventValue(this->event_list[i]->event, 
	                CL_PROFILING_COMMAND_END) - this->gpu_timer_start) );
*/
    }
    for(int i = 0; i < (int)this->user_event_list.size(); i++) {
        fprintf(fp, "%s;\t%s;\t%lu;\t%lu;\t%lu;\t%lu\n", 
            this->user_event_list[i]->type,
            this->user_event_list[i]->name, 
	    ulong(     getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_QUEUED) - this->cpu_timer_start ),
            ulong(	getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_SUBMIT) - this->cpu_timer_start ),
            ulong(	getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_START) - this->cpu_timer_start  ),
            ulong(	getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_END) - this->cpu_timer_start )
	);
    }
    //!Bound check
    if(event_list.size() > 0)
    {
    	long long start_t = getEventValue(this->event_list[0]->event, CL_PROFILING_COMMAND_QUEUED) - this->gpu_timer_start ;
    	long long end_t = getEventValue(this->event_list[event_list.size() - 1]->event,CL_PROFILING_COMMAND_END)	- this->gpu_timer_start;
		printf("Time first %llu \t last %llu \t Diff %f \n",start_t, end_t, float((end_t - start_t))/(1000.0f*1000.0f) );
    }
    else
    	printf("WARNING - No events Detected\n ");

    fclose(fp);

    delete filename;
    delete fullpath;
}

// Grab the current time (in ns) using a system-specific timer
cl_time EventList::getCurrentTime() {

    // Current time in nanoseconds
    cl_time time;

#ifdef _WIN32
    // Use Windows performance counters to get the current time
    __int64 counter;
    __int64 freq;

    int status = QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    if(status == 0) {
        perror("QueryPerformanceCounter");
        exit(-1);
    }
    status = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    if(status == 0) {
        perror("QueryPerformanceFrequency");
        exit(-1);
    }

    // Return time in ns
    time = (cl_ulong)(counter*(1e9/freq));
#else
    // Use gettimeofday to get the current time
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    
    // Convert timeval into ns
    time = curTime.tv_sec * 1e9 + curTime.tv_usec * 1e6;
#endif

    return time;
}

// Retrieve the specified timer value from the event
cl_time EventList::getEventValue(cl_event event,
								cl_profiling_info param_name)
{
    
    cl_int status; 
    cl_ulong value;

    // Get the requested timer value from OpenCL
    status = clGetEventProfilingInfo(event, param_name, sizeof(cl_ulong),
                                     &value, NULL);

    if(status != CL_SUCCESS) {
        //printf("Profiling error (eventlist.cpp)\n");
        ad_errChk(status,"Profiling Error");
        exit(-1);
    }

    return value;
}

//! Retrieve the specified timer value from the event
cl_time EventList::getUserEventValue(cl_user_event event,
									cl_profiling_info param_name)
{
    cl_ulong value;

    // Return the requested timer value for a user event
    switch(param_name) {
        case CL_PROFILING_COMMAND_QUEUED:
            value = event->start;
            break;
        case CL_PROFILING_COMMAND_SUBMIT:
            value = event->queued;
            break;
        case CL_PROFILING_COMMAND_START:
            value = event->start;
            break;
        case CL_PROFILING_COMMAND_END:
            value = event->end;
            break;
        default:
            perror("Invalid user event param");
            exit(-1);
    }

    return (cl_time)value;
}

//! Return a delta from the start of the timers
cl_time EventList::getEventDelta(cl_event input_event,
								cl_profiling_info param_name )
{
    cl_time diff;
    diff = getEventValue(input_event,param_name) - this->gpu_timer_start;
    return diff;
}

//! Return a delta from the start of the timers
cl_time EventList::getUserEventDelta(cl_user_event input_event,
								cl_profiling_info param_name )
{
    cl_time diff;
    diff = getUserEventValue(input_event,param_name) - this->cpu_timer_start;
    return diff;
}

// Print the event information to the screen
void EventList::printEvents() {

    for(int i = 0; i < (int)this->event_list.size(); i++) {
        printf("%s (%s)\
               \n\tQueued:     %lu\
               \n\tSubmitted:  %lu\
               \n\tStarted:    %lu\
               \n\tCompleted:  %lu\n", 
            this->event_list[i]->name, 
            this->event_list[i]->type,
            ulong( getEventValue(this->event_list[i]->event, 
                CL_PROFILING_COMMAND_QUEUED) - this->gpu_timer_start ),
            ulong( getEventValue(this->event_list[i]->event, 
                CL_PROFILING_COMMAND_SUBMIT) - this->gpu_timer_start ),
            ulong( getEventValue(this->event_list[i]->event, 
                CL_PROFILING_COMMAND_START) - this->gpu_timer_start ),
            ulong( getEventValue(this->event_list[i]->event, 
                CL_PROFILING_COMMAND_END) - this->gpu_timer_start )  );
    }
    for(int i = 0; i < (int)this->user_event_list.size(); i++) {
        printf("%s (%s)\
               \n\tQueued:     %lu\
               \n\tSubmitted:  %lu\
               \n\tStarted:    %lu\
               \n\tCompleted:  %lu\n", 
            this->user_event_list[i]->name, 
            this->user_event_list[i]->type,
            ulong( getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_QUEUED) - this->cpu_timer_start ),
            ulong( getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_SUBMIT) - this->cpu_timer_start ),
            ulong( getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_START) - this->cpu_timer_start  ),
            ulong( getUserEventValue(this->user_event_list[i]->event, 
                CL_PROFILING_COMMAND_END) - this->cpu_timer_start) 
	) ;
    }
}

// Allocate space and perform a string copy
char* EventList::strCopy(const char* origStr) {

    size_t strLen = strlen(origStr)+1;

    char* newStr = new char[strLen];

    memcpy(newStr, origStr, strLen);

    return newStr;
}

// Set the start times of the GPU and CPU clocks
void EventList::resetClocks() 
{
    cl_event event;
    cl_int status;
    
    // Create a buffer to perform a transfer (needed to get the GPU
    // clock)
    cl_mem buf = clCreateBuffer(this->context, 0, 1, NULL, &status);
    if(status != CL_SUCCESS) {
        printf("EventList: Failed to create buffer\n");
        exit(-1);
    }

    char tmp;
    // Enqueue an command to get the GPU timer
    status = clEnqueueReadBuffer(commandQueue, buf, CL_TRUE, 0, 1, &tmp, 0, 
        NULL, &event);

    // Set the CPU timer initial value
    this->cpu_timer_start = EventList::getCurrentTime();

    // Make sure that the read buffer happened correctly (we're checking here to
    // avoid extra overhead between CPU and GPU clock synchronization)
    if(status != CL_SUCCESS) {
        printf("EventList: Read buffer failed (%d)\n", status);
        exit(-1);
    }

    // Set the GPU timer intial value
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, 
        sizeof(cl_ulong), &this->gpu_timer_start, NULL);
    if(status != CL_SUCCESS) {
        printf("EventList: clGetEventProfilingInfo failed (%d)\n", status);
        exit(-1);
    }

    // Free the memory object
    clReleaseMemObject(buf);
}
