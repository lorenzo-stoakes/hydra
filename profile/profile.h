#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct thread;
typedef void (*thread_func)(struct thread *);

struct thread {
	pthread_t id;
	unsigned int core, node;

	thread_func fn;
};

struct thread_set {
	int nr_cores;
	struct thread *threads;
};

/* helpers.c */
void fatal(char *fmt, ...);
void get_node(unsigned int *node_ptr);

/* profile.c */
void do_profile(void);

/* thread.c */
struct thread_set *create_thread_set(thread_func fn);
void join_thread_set(struct thread_set *set);
void print_thread(struct thread *thread);
