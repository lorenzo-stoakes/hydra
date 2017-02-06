#include "profile.h"

/*
 * v1 concept:
 *
 * 1. Allocate NR_CPU * N * 2MiB buffers for threads in each core.
 * 2. Fault in.
 * 3. Wait until THP...
 * 4. Rotate buffers, so they're off-node.
 * 5. Wait for migration...
 * 6. Report stats and repeat...
 */

/* Run in each thread. */
static void do_thread(struct thread *thread)
{
	/* Do something... */
}

/* Execute the profiler. */
void do_profile(void)
{
	int i;
	struct thread_set *set = create_thread_set(do_thread);

	join_thread_set(set);
	for (i = 0; i < set->nr_cores; i++) {
		struct thread *thread = &set->threads[i];

		print_thread(thread);
	}
}
