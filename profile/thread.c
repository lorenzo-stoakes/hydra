#include <sched.h>
#include <string.h>
#include <unistd.h>

#include "profile.h"

/* Set the thread to only have affinity to its core. */
static void set_affinity(struct thread *thread)
{
	int err;
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(thread->core, &cpuset);

	err = pthread_setaffinity_np(thread->id, sizeof(cpu_set_t), &cpuset);
	if (err != 0)
		fatal("%d: pthread_setaffinity_np() failed: %s\n", thread->core,
			strerror(err));
}

/* Populate remaining fields in the thread struct. */
static void populate(struct thread *thread)
{
	get_node(&thread->node);
}

/* Thread entrypoint. */
static void *do_thread(void *ptr)
{
	struct thread *thread = (struct thread *)ptr;

	/* Set affinity once thread created so we block on it. */
	set_affinity(thread);
	populate(thread);

	/* Now do some actual work. */
	thread->fn(thread);

	return NULL;
}

/* Create and execute new thread, storing details in *thread. */
static void create(int core, thread_func fn, struct thread *thread)
{
	int err;

	thread->core = core;
	thread->fn = fn;
	err = pthread_create(&thread->id, NULL, do_thread, thread);
	if (err != 0)
		fatal("%d: pthread_create() failed: %s\n", thread->core,
			strerror(err));
}

/* Creates a set of threads for each system core. */
struct thread_set *create_thread_set(thread_func fn)
{
	int i;
	int nr_cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
	struct thread_set *ret = malloc(sizeof(struct thread_set));

	ret->nr_cores = nr_cores;
	ret->threads = calloc(nr_cores, sizeof(struct thread));

	for (i = 0; i < nr_cores; i++)
		create(i, fn, &ret->threads[i]);

	return ret;
}

/* Joins all threads in thread set. */
void join_thread_set(struct thread_set *set)
{
	int i, err;

	for (i = 0; i < set->nr_cores; i++) {
		struct thread *thread = &set->threads[i];

		if ((err = pthread_join(thread->id, NULL)) != 0)
			fprintf(stderr, "%d: pthread_join() failed: %s\n",
				thread->core, strerror(err));
	}
}

/* Output thread details to stderr. */
void print_thread(struct thread *thread)
{
	fprintf(stderr, "%02d: node %d\n", thread->core, thread->node);
}
