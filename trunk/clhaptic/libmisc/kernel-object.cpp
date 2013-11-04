#include "stdio.h"
#include <string.h>

#include "kernel-object.h"


kernel_object alloc_kernel_object()
{
	return new _kernel_object;
}

void set_kernel_name(_kernel_object * k, char * name)
{
	k->name = (char *)malloc(sizeof(char)*strlen(name));
	strcpy(k->name,name);
}
