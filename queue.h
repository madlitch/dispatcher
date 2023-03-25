#ifndef QUEUE_H_
#define QUEUE_H_

#define MEMORY 1024
#define REALTIME_MEMORY 64
#define RESOURCE_CDS 2
#define RESOURCE_SCANNERS 2
#define RESOURCE_PRINTERS 2
#define RESOURCE_MODEMS 2

// resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
    int printers;
    int cds;
    int scanners;
    int modems;
    int available_memory;
    int realtime_mem;
    int memory[MEMORY];
} resources;

// process structure that contains all the data and resources needed from the dispatch list
typedef struct {
    int arrival_time;
    int priority;
    int processor_time;
    int mbytes;
    int printers;
    int scanners;
    int modems;
    int cds;
    int memory_index;
} process;

// node data structure for queue
typedef struct node {
    process *proc;
    struct node *next_node;
} node_t;

// push onto queue
extern void push(node_t *tail, process *proc);

// return length of queue
extern int length(node_t *tail, int len);

// pop from queue
extern process *pop(node_t *tail);

#endif /* QUEUE_H_ */