/*
 * phasecontrol.cpp
 *
 *  Created on: 08-Sep-2011
 */

#include "stdlib.h"
#include <stdio.h>
#include "phasecontrol.h"
#include "eventlist.h"

//! Default Constructor
ProfilerConfig::ProfilerConfig()
{
	printf("Default ProfilerConfig Parameters\n");

	record_phases = ENABLED;
	verbose_messages = ENABLED;
	max_disabled_duration = 5;
	record_phases = ENABLED;

}

//! Default initialisation to 0
PhaseTags::PhaseTags()
{
	id = 0;
	start = 0;
	delta =0;
}

//! Constructor
PhaseTags::PhaseTags(int in_id, int in_start, int in_delta)
{
	in_id = id;
	in_start = start;
	in_delta = delta;
}
//! Default
PhaseControl::PhaseControl()
{
	user_event_count = 0;
	event_count = 0;
	event_phase_delta = 0;
	user_event_phase_delta = 0;
	phase_count = 0;
	user_event_phase_count = 0;
}

//! Constructor with configuration
PhaseControl::PhaseControl(ProfilerConfig * ip)
{
	//TODO
}

//! Read the PhaseControl::tags vector (With STL bound checking)
PhaseTags PhaseControl::read_tag(int i)
{
//	printf("Value of tags %d\n",i);
	return tags.at(i);
}

//! Read the PhaseControl::user_tags vector (With STL bound checking)
PhaseTags PhaseControl::read_user_event_tag(int i)
{
	return user_tags.at(i);
}

//! \param ip Return last tag in the vector.
//! Default argument is for the last tag, otherwise its the last - ip tag
PhaseTags PhaseControl::get_latest_phase_tag(int ip)
{
	return tags.at(phase_count -1);
}

//! The value returned is phase_count - 1 because the after the phase is added
//! in PhaseControl::mark_phase(), the counter is incremented. So phase_count
//! will always be ahead by one wrt to the location in the tags vector
int PhaseControl::get_latest_phase_no()
{
	//printf("Return the phase count %d  \n",phase_count-1);
	return (phase_count -1 );
}

void PhaseControl::update_tags()
{
	//! This function has been added to take possibly compact the vector
	//! in case we  decide to dynamically resize vectors
	//!
	printf("Not implemented");
	exit(-1);
}

int PhaseControl::getDisabledDurationCount()
{
	return profile_disabled_count;
}

//! Reset the count to 0.
void PhaseControl::resetDisabledDurationCount()
{
	profile_disabled_count = 0;
}

//! Increment disabled duration count
void PhaseControl::incDisabledDurationCount()
{
	//! A set function is not added since a user should not be able
	//! to assign a integer to this varaibel.
	//! This variable only gets incremented and then resets with
	//! PhaseControl::resetDisabledDurationCount()
	profile_disabled_count = profile_disabled_count +1;
}

//! Update state when a event is added
void PhaseControl::add_event()
{
	//! Called from EventList::add()
	event_count = event_count +1 ;
	event_phase_delta = event_phase_delta + 1;
}

//! Update state when a user event is added
void PhaseControl::add_user_event()
{
	//! Called from EventList::add()
	user_event_count = user_event_count +1 ;
	user_event_phase_delta = user_event_phase_delta + 1;
}

void PhaseControl::mark_phase(int p)
{
    PhaseTags t;
    t.id = p;
    t.start = event_count - event_phase_delta;
    t.delta = event_phase_delta;
    //printf("Phase Added %d\t %d\t %d\n", t.id, t.start, t.delta);
    tags.push_back(t);

    event_phase_delta = 0;
    phase_count = phase_count + 1;

}

void PhaseControl::mark_user_event_phase(int p)
{
    PhaseTags t;
    t.id = p;
    t.start = user_event_count - user_event_phase_delta;
    t.delta = user_event_phase_delta;
    printf("Phase Added %d\t %d\t %d\n", t.id, t.start, t.delta);
    user_tags.push_back(t);

    user_event_phase_delta = 0;
    user_event_phase_count = user_event_phase_count + 1;
}
