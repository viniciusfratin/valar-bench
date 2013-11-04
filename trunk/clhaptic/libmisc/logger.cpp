#include "stdio.h"
#include "string.h"
#include <iostream>
#include <vector>

#include "logger.h"

logger * warning_object = NULL;

logger * logging_object = NULL;

logger::logger()
{
	filename = createFilenameWithTimestamp();

}

void logger::add(char * s)
{
   printf("%s\n",s);
}

void shutdown_logging()
{
	//! TODO Flush the data in the logs
	//! Close Files
	//! Delete Objects
}

void initialize_logging()
{
	warning_object = new logger[1];
	logging_object = new logger[1];
}
// TODO Add File writing options for warnings
//! This function is used to print warnings and will mostly be disabled when
//! I finally get this working
void print_warning(char * input_string)
{

#ifdef ENABLE_WARNINGS

	printf("WARNING  - \t");
	printf("%s \n",input_string);

#endif
}


//TODO Add File writing options for LOGS
void print_logging(char * input_string)
{

#ifdef ENABLE_LOGGING

	printf("LOG  - \t");
	printf("%s \n",input_string);

#endif
}

void print_logging(float k)
{

#ifdef ENABLE_LOGGING

	printf("LOG  - \t");
	printf("%f \n",k);

#endif

}

//TODO Add File writing options for LOGS
void print_logging(bool hard_wired, char * input_string)
{
	if(hard_wired == ENABLED)
	{
		printf("LOG  - \t");
		printf("%s \n",input_string);
	}

}
