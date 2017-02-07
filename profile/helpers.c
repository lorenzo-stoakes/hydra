#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "profile.h"

#define BUF_SIZE 64

/* Read file until EOF or newline. */
static void eat_line(FILE *file)
{
	char chr;

	while ((chr = fgetc(file)) != EOF)
		if (chr == '\n')
			return;
}

/* Output to stderr then exit process with error code. */
void fatal(char *fmt, ...)
{
	va_list list;

	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);

	exit(EXIT_FAILURE);
}

/* Get number of anonymous huge pages containing addr. */
int get_anon_huge_kb(unsigned long addr)
{
	unsigned long from, to;
	int ret = 0;
	FILE *file = fopen("/proc/self/smaps", "r");

	if (file == NULL)
		return -errno;

	while (fscanf(file, "%lx-%lx%*[^\n]", &from, &to) == 2) {
		int amount;
		char type[BUF_SIZE], unit[BUF_SIZE];
		bool skip = addr < from || addr >= to;

		while (fscanf(file, "%s %d %s", type, &amount, unit) == 3)
			if (strcmp(type, "VmFlags:") == 0)
				break;
			else if (skip)
				continue;
			else if (strcmp(type, "AnonHugePages:") == 0) {
				ret = amount;
				goto exit;
			}

		eat_line(file);
	}
	ret = EOF;
exit:
	if (ferror(file))
		ret = -errno;
	fclose(file);

	return ret;
}

/* Determine the total number of cores in the system. */
int get_nr_cores(void)
{
	return (int)sysconf(_SC_NPROCESSORS_ONLN);
}

/* Determine current thread's NUMA node, fatal if syscall fails. */
void get_node(unsigned int *node_ptr)
{
	if (syscall(SYS_getcpu, NULL, node_ptr, NULL) != 0)
		fatal("getcpu() failed.\n");
}
