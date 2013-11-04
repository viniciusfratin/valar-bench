#ifndef __ANALYSIS_RULE_VEC_
#define __ANALYSIS_RULE_VEC_

#include <CL/cl.h>
#include "ad_rule.h"

class ad_rule_vec
{
private:
	bool verbose_logging;
	std::vector<ad_rule> rule_list;
public:
	ad_rule_vec();
	//void apply_rule(cl_context , cl_command_queue, int i);
	void add_rule(ad_rule input);
	int active_rule_count();
	ad_rule get_rule(int i);
};


#endif
