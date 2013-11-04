#ifndef MULTIDEVICE_TOPOLOGY_H
#define MULTIDEVICE_TOPOLOGY_H

#include "opencl_utils.h"
#include "fissionutils.h"

//! This class is different from the fission_topology.
//! Created because all devices are equal unlike the fission case with subdevices and so on.
class multidevice_topology
{
  private:

    //! Round robin scheduling
    int last_assigned_queue;

    ext_scheduler_type fission_scheduler;
    bool * profiling_status;

  public:

    //! Profiling flags, made so that you can handle queues in different states


    //! Regular OpenCL Object Topology
    cl_device_id  * devices;
    cl_device_type * device_type;
    cl_context root_context;
    cl_command_queue * queue;


    //! Subprogram. One per subdevice
    cl_program * program;

    void cl_CompileProgramDevices(char * kernelPath,
                                    char * compileoptions, bool verbosebuild);

    //! Simple return - cpu or gpu queue interface
    cl_uint cpu_queue_no;
    cl_uint gpu_queue_no;
    cl_command_queue return_cpu_queue();
    cl_command_queue return_gpu_queue();

    queue_id schedule_kernel();
    //!TODO Command Queue Migration
};

#endif //MULTIDEVICE_TOPOLOGY_H
