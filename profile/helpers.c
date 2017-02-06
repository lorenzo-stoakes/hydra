#include <stdarg.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "profile.h"

/* Output to stderr then exit process with error code. */
void fatal(char *fmt, ...)
{
	va_list list;

	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);

	exit(EXIT_FAILURE);
}

/* Determine current thread's NUMA node, fatal if syscall fails. */
void get_node(unsigned int *node_ptr)
{
	if (syscall(SYS_getcpu, NULL, node_ptr, NULL) != 0)
		fatal("getcpu() failed.\n");
}
