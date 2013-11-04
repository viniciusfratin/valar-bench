/*
 * eventlist-phasecontrol.cpp
 *
 */
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "eventlist.h"
#include "phasecontrol.h"

//! Interface between phase_data and EventList.
//! \param u A unique identifier for each phase, simply a int for now
void EventList::markPhase(int u)
{
	//! The markPhase function will not record anything if EventList::record_profile is DISABLED
	if(config.record_phases == ENABLED)
	{
		if(record_profile == ENABLED)
			control.mark_phase(u);
		samplePhaseChange();
	}
}

void EventList::simple_enqueue_phase(int u)
{
	control.mark_phase(u);
}

//! User API to compare phase changes.
//! This is done instead of just showing EventList::analysePhaseChange
//! so that we dont have to expose the profiling checking code to the user
void EventList::samplePhaseChange()
{
	//Just Check for variation

	//! If profiling is ENABLED - go to EventList::analysePhaseChange()
	if(record_profile == ENABLED)
	{
		//This conditional's added since we need atleast 2 phases to compare :)
		if(control.get_latest_phase_no() >= 1 )
			analysePhaseChange();
	}
	else
	{
		control.incDisabledDurationCount();
		// !If record_profiling is DISABLED -
		//! Enable it again after ProfileConfig::max_disabled_duration
		if(control.getDisabledDurationCount() >= config.max_disabled_duration)
		{
			setProfilingStatus(ENABLED);
			control.resetDisabledDurationCount();
		}
	}
}

void EventList::analysePhaseChange()
{
	//! Get Current Phase and Get Previous Phase and compare the events in each one
	if(record_profile == DISABLED)
	{
		if(config.verbose_messages == ENABLED)
		{
			printf("\n------------------------\n");
			printf("Events are not recorded - so we cannot analyse phase");
			printf("\n------------------------\n");
		}
		return;
	}

	//	printf("Latest 2 Phase Nos are %d\t %d\n",control.get_latest_phase_no(), control.get_latest_phase_no()-1);

	PhaseTags p0 = control.read_tag(control.get_latest_phase_no() 	);
	PhaseTags p1 = control.read_tag(control.get_latest_phase_no() -1 );

	printf("Latest 2 Phase Id Params \t %d \t %d\n",p0.id, p1.id);
	if (p0.delta != p1.delta)
	{
		printf("Profiler Name %s\t",profiler_name);
		printf("Dont know how to compare unequal length phases - Skipping compare\n");
		return ;
	}
	// This is a wrong Lambda, Ignore this for now. Will try and add a better verification statement here
	//	if(p0.id != ( control.get_latest_phase_no()  ) || p1.id != (control.get_latest_phase_no() -1))
	//	{
	//		printf("Something is wrong\n");
	//		exit(-1);
	//	}

	int compare_len = p0.delta;
	int i0_start = p0.start;
	int i1_start = p1.start;
	//Compare and Decide on a return type
	bool loop_aborted = FALSE;
	int i;
	for( i = 0; i < compare_len; i++ )
	{
 		cl_event event0 = event_list[i+i0_start]->event;
		cl_event event1 = event_list[i+i1_start]->event;

		cl_time t0 = getEventDelta(event0,CL_PROFILING_COMMAND_END) -
					 getEventDelta(event0,CL_PROFILING_COMMAND_START);

		cl_time t1 = getEventDelta(event1,CL_PROFILING_COMMAND_END) -
					 getEventDelta(event1,CL_PROFILING_COMMAND_START);
		printf("%d\t T0 \t %ld \t \t T1 \t %ld \n",i,t0,t1);
		if( fabs(t0 - t1) > config.time_threshold)
		{
			if(config.verbose_messages == ENABLED)
			{
				printf("\nLOG: %s ------------------------\n",profiler_name);
				printf("LOG: Too much variation between same events in different phases\n");
				printf("LOG: Exit Comparison and Carry On\n");
				printf("------------------------\n");
			}
 			break;
		}
	}
	printf("leaving phase compare loop\n");
	// profiling disabled only if all the phase differences are smaller than threshold
	if(i == (compare_len ))
	{
		printf("Disabling Profiling\n");
		setProfilingStatus(DISABLED);
	}
}


cl_time EventList::getPhaseEventDelta(int phase_id,
							cl_event input_event,
							cl_profiling_info param_name)
{
	printf(" To add");
	exit(-1);
}

cl_time EventList::getPhaseUserEventDelta(int phase_id,
							cl_user_event input_event,
							cl_profiling_info param_name)
{
	printf(" To add");
	exit(-1);
}
