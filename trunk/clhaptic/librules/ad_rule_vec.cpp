#include <CL/cl.h>
#include "ad_rule_vec.h"

ad_rule_vec::ad_rule_vec()
{
	verbose_logging = ENABLED;
}
void ad_rule_vec::add_rule(ad_rule ip)
{
	if(verbose_logging == ENABLED)
		ip.print_rule_details();

	rule_list.push_back(ip);
}


//! For now, all rules are always active.
//!	Return the number of active rules
int ad_rule_vec::active_rule_count()
{
	return rule_list.size();
}

//void ad_rule_vec::apply_rule(cl_context ctx, cl_command_queue queue, int i)
//{
//	rule_list.at(i).test_rule(ctx,queue);
//
//}

ad_rule ad_rule_vec::get_rule(int i)
{
	return rule_list.at(i);
}
