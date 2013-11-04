#ifndef __LOGGER__
#define __LOGGER__

#include "opencl_utils.h"
#include "stdio.h"

/*
#define ENABLE_WARNINGS

//#define ENABLE_LOGGING
*/

#define LOG_TYPE_WARNING 0x999

#define LOG_TYPE_RECORD 0x998

class logger
{
private:
	char * filename;
	int log_type;
public:
	logger();
	void add(char * );

};

void initialize_logging();
void shutdown_logging();

void print_warning(char * input);


void print_logging(char * input);
void print_logging(float k);
void print_logging(bool hard_wired, char * input_string);

#endif // __LOGGER__
