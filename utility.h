#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdbool.h>
#include "queue.h"

// allocates a contiguous block of memory
extern int alloc_memory(resources *res, int size, int priority);

// frees a contiguous block of memory
extern void free_memory(resources *res, int index, int size);

// parses the dispatch list file and assigns to queue
extern int load_dispatch(char *dispatch_file, int dispatch_list[][9]);

// loads jobs into relevant queue
extern void load_jobs(int time, int num_processes, int dispatch_list[][9], node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority, resources *available_res);

// checks if resources are available for a process
extern bool resources_available(process *proc, resources *available_res);

// checks if queues are empty
extern bool queues_empty(node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority);

#endif /* UTILITY_H_ */