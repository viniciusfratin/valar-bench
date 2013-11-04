
#ifndef _EVENTLIST_
#define _EVENTLIST_

#include <CL/cl.h>

#include <iostream>
#include <vector>

#include <sstream>
#include <string>

#include "phasecontrol.h"
#include "eventtypes.h"
#include "logger.h"


//! The main Event Handling Class. Part of libprofile in HAPTIC
//! The main profiling state is in event_list and user_event_list
//! The PhaseControl member object EventList::control manages the sampling granularity
//! The ProfilerConfig member object is used to configure the profiler
class EventList
{	

public:

    //! Constructor
	EventList();

    //! Constructor
    EventList(cl_context context, cl_command_queue commandQueue,
                         cl_device_id device, bool free_events=true,
                         char * name = NULL);

    //! Destructor
    ~EventList();

    //! Add an OpenCL event to the event list
    void add(cl_event event, const char* name=NULL, 
        const char* type=NULL);

    //! Add a user event to the event list
    void add(cl_user_event event, const char* name=NULL, 
        const char* type=NULL);

    // Command to create user event (designed similar to OpenCL API)
    static cl_user_event clCreateUserEvent();

    //! Command to set user event status (records time when status is set)
    static cl_int clSetUserEventStatus(cl_user_event, cl_int execution_status);

    //! Writes event information to file
    void dumpEvents(char* path, char * ip_filename = NULL);

    char * get_profiler_name();

    //! Prints the events to the screen
    void printEvents();

    //! Queries the GPU and CPU clocks at (roughly) the same time
    void resetClocks();

    //! Enable Profiling flags - changes the record_profile to ENABLED or DISABLED
    void setProfilingStatus(bool inputflag);

    //! Return Profiling Status
    bool getProfilingStatus();

    //! Adds a marker in PhaseControl tags vector.
    //! This function is only useful for the case of recording timer stamps
    void markPhase(int u);
    void simple_enqueue_phase(int u);

    int get_latest_phase_id();

private:

    //! Interface to PhaseControl object, calls EventList::analysePhaseChange()
    void samplePhaseChange();


	//! A name to keep track
	char * profiler_name;

    //! Interface to the OpenCL application
    void analysePhaseChange();

    //! Enable or Disable profiling (see typdef in eventtypes.h)
    bool record_profile ;

    //! OpenCL command queue for the application
    cl_command_queue commandQueue;

    //! OpenCL context for the application
    cl_context context;

    //! Timer offset for the CPU
    cl_ulong cpu_timer_start;

    //! Creates a filename for the event dump based on the current time
    char* createFilenameWithTimestamp();

    //! OpenCL device for the application
    cl_device_id device;

    //! Boolean for whether we should free the events for the user
    bool free_events;

    //! Function to get the current system (CPU) time
    static cl_time getCurrentTime();

    //! Function to get a profiling value from an OpenCL event
    cl_time getEventValue(cl_event event, cl_profiling_info param_name);

    //! Function to get a profiling value from a user event
    cl_time getUserEventValue(cl_user_event event,
								cl_profiling_info param_name);

    //! Function to get a profiling value from a user event
	cl_time getEventDelta(cl_event input_event,
								cl_profiling_info param_name );

    cl_time getUserEventDelta(cl_user_event input_event,
								cl_profiling_info param_name );

    //! Get the delta within the phase window
	cl_time getPhaseEventDelta(int phase_id, cl_event input_event,
								cl_profiling_info param_name);

    //! Get the delta within the phase window
	cl_time getPhaseUserEventDelta(int phase_id, cl_user_event input_event,
								cl_profiling_info param_name);

    //! Timer offset for the GPU
    cl_ulong gpu_timer_start;

    //! Performs as safe (hopefully quick) string copy
    static char* strCopy(const char* origStr);
    
    //! List of user events
    std::vector<user_event_triple> user_event_list;

    //! List of OpenCL events
    std::vector<event_triple> event_list;

    //! Phase data Used to maintain list markers for application phase experiments
    PhaseControl control;

    //!Configuration Structure
    ProfilerConfig config;
};

#endif 
