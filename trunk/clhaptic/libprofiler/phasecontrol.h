/**
 * \file phasecontrol.h
 *
 *  Created on: 08-Sep-2011
 */

#ifndef PHASECONTROL_H_
#define PHASECONTROL_H_

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <CL/cl.h>

#include "eventtypes.h"

//! A configuration typedef for clarity
typedef int config_param;

//! To set up EventList and PhaseControl.
//! Default Parameters used are					\n
//! ProfilerConfig::record_phases = ENABLED; 	\n
//! ProfilerConfig::verbose_messages = ENABLED;	\n
//! ProfilerConfig::max_disabled_duration = 5;	\n
//! Simple example is in EventList::EventList()
class ProfilerConfig
{
public:
	//! Default initialisation of verbose_messages and max_disabled_duration
	ProfilerConfig();
	//! Depends on EventList::record_profile TODO
	bool record_phases;

	//! Depends on EventList::record_profile TODO
	config_param discard_same_phases;
	//! For future work
	//! To add to data output considering the sampled nature
	config_param trace_format;
	//! For comparison between phases
	config_param comparison_method;

	//! The time difference between any two measurements.
	//! Used as a threshold for event_triple objects
	cl_time time_threshold;
	//! Verbose messages printedd
	bool verbose_messages ;
	//! No of phases that the profiler remains disabled for
	int max_disabled_duration ;
};


//! Forward declaration
class EventList;


//! Each phase entry consists of a PhaseTag which shows
//! where in the event vector it was used and recorded
class PhaseTags
{
public:
	int id;
	int start;
	int delta;
	PhaseTags();
	PhaseTags(int in_id, int in_start, int in_delta);
};

class PhaseControl
{
private:
	//! A vector of PhaseTags
	std::vector<PhaseTags> tags;

	//! A %vector of PhaseTags for user events
	std::vector<PhaseTags> user_tags;
	//! Total numbers of events
	int user_event_count ;
	//! Total number of user events
	int event_count;
	//!  Current phase length event_phase_delta;
	int event_phase_delta;
	//!  Current phase length event_phase_delta;
	int user_event_phase_delta;

	//! No of phases recorded
	int phase_count;
	//! No of phases recorded
	int user_event_phase_count;

	//! Count of number of phases that profiling has been disabled for
	int profile_disabled_count;
public:
	//! Initialise counters to 0
	PhaseControl();
	//! Constructor to configure profiler
	PhaseControl(ProfilerConfig * ip);

	// 	void compare_phases(int i0, int i1);
	//	void compare_phases(int id_0, int id_1, std::vector<event_triple > foo);
	//	void compare_phases_user_event(int id_0, int id_1, std::vector<user_event_triple > foo);
	//	void mark_and_compare_phase(int candidate);

	//! Remove phases.
	//! Related to PhaseControl::update_tags TODO
	void clear_phase(int candidate);

	//! Function added to update internal state of PhaseControl
	//! Called from EventList::add()
	void add_event();

	//! Function added to update internal state of PhaseControl
	void add_user_event();

	//! Add a element to the tags vector
	void mark_phase(int p);
	//! Add a element to the user_tags vector
	void mark_user_event_phase(int p);

	//! Update the tag parameters.
	void update_tags();

	//! Set the number of disabled iterations to 0.
	//! This is called when we set the EventList::record_profiling.
	void resetDisabledDurationCount();
	//! Number of phases that the profiler has been disabled for
	int getDisabledDurationCount();
	//! Increment the no of disabled iterations.
	//! More info about design of this interface in the declaration
	void incDisabledDurationCount();

	//! Return tags for the EventList class
	PhaseTags read_tag(int i);
	//! Return tags for the EventList class user_event
	PhaseTags read_user_event_tag(int i);

	//! Return the tag
	PhaseTags get_latest_phase_tag(int ip = 0);
	//int get_latest_phase_no(int ip = 0);

	int get_latest_phase_no();
};

#endif /* PHASECONTROL_H_ */
