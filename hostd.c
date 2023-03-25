#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"
#include "utility.h"

#define NUM_RESOURCE 9
#define MAX_PROCESS 1000

node_t *job_queue, *realtime_queue, *first_priority, *second_priority, *third_priority;
resources *available_resources;

int main(int argc, char *argv[]) {
    char dispatch_file[256];
    int num_processes = 0;
    int time = 0;
    int dispatch_list[MAX_PROCESS][NUM_RESOURCE] = {0};

    // initializing all the queues
    job_queue = (node_t *) malloc(sizeof(node_t));
    job_queue->next_node = NULL;

    realtime_queue = (node_t *) malloc(sizeof(node_t));
    realtime_queue->next_node = NULL;

    first_priority = (node_t *) malloc(sizeof(node_t));
    first_priority->next_node = NULL;

    second_priority = (node_t *) malloc(sizeof(node_t));
    second_priority->next_node = NULL;

    third_priority = (node_t *) malloc(sizeof(node_t));
    third_priority->next_node = NULL;


    // initializing the resources
    available_resources = (resources *) malloc(sizeof(resources));
    available_resources->cds = RESOURCE_CDS;
    available_resources->scanners = RESOURCE_SCANNERS;
    available_resources->printers = RESOURCE_PRINTERS;
    available_resources->modems = RESOURCE_MODEMS;
    available_resources->available_memory = MEMORY - REALTIME_MEMORY;
    available_resources->realtime_mem = REALTIME_MEMORY;

    // initializing memory to 0
    for (int i = 0; i < MEMORY; i++) {
        available_resources->memory[i] = 0;
    }

    // checking if input file was specified
    if (argc != 2) {
        printf("no input arg - using default (dispatchlist)\n");
        strcpy(dispatch_file, "dispatchlist");
    } else {
        strcpy(dispatch_file, argv[1]);
    }

    // load the processes from dispatchlist into the job queue
    num_processes = load_dispatch(dispatch_file, dispatch_list);

    printf("starting dispatcher...\n");
    int proc_id = 1;
    int mem_index = 0;
    pid_t pid;

    while (true) {
        // iterate through each item in the job dispatch list, add each process to the appropriate queues
        load_jobs(time, num_processes, dispatch_list, job_queue, realtime_queue, first_priority, second_priority,
                  third_priority, available_resources);

        printf("dispatching process %d/%d from", proc_id, num_processes);

        process *proc;
        if (realtime_queue->next_node != NULL) {
            proc = pop(realtime_queue);
            printf(" realtime ");
        } else if (first_priority->next_node != NULL) {
            proc = pop(first_priority);
            printf(" first_priority ");
        } else if (second_priority->next_node != NULL) {
            proc = pop(second_priority);
            printf(" second_priority ");
        } else {
            proc = pop(third_priority);
            printf(" third_priority ");
        }

        mem_index = alloc_memory(available_resources, proc->mbytes, proc->priority);

        printf("queue with arrival time %d and priority %d\n", proc->arrival_time, proc->priority);
        printf("allocating %d mbytes, %d printers, %d scanners, %d modems, %d cds\n", proc->mbytes, proc->printers,
               proc->scanners, proc->modems, proc->cds);

        // run mock process
        pid = fork();
        if (pid == 0) {
            execl("./process", NULL, NULL);
            perror(": ");
        } else if (pid != 0) {
            printf(" ---- executing process %d ---- \n", pid);
            sleep(3);
            kill(pid, SIGINT);
            waitpid(pid, 0, 0);
        }

        free_memory(available_resources, mem_index, proc->mbytes);
        printf("process %d terminated\n", pid);
        printf("freeing resources\n");

        time++;
        proc_id++;
        if (queues_empty(job_queue, realtime_queue, first_priority, second_priority, third_priority)) {
            break;
        }
    }

    printf("no processes remaining");
    return EXIT_SUCCESS;
}