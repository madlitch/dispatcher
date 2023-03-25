#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

#define MAX_CHAR 100

// allocate memory based on priority (realtime/non realtime) and size
int alloc_memory(resources *res, int size, int priority) {
    int max_memory = 0;
    int memory_count = size;
    int memory_index = 0;
    if (priority == 0) {
        max_memory = 1024;
        memory_index = 960;
        res->realtime_mem -= size;
    } else {
        max_memory = 960;
        res->available_memory -= size;
    }
    for (; memory_index < max_memory && memory_count > 0; memory_index++) {
        if (res->memory[memory_index] == 0) {
            res->memory[memory_index] = 1;
            memory_count--;
        }
    }
    return memory_index - size;
}

// free memory at specific index
void free_memory(resources *res, int index, int size) {
    int memory_index = index;
    while (memory_index < index + size) {
        res->memory[memory_index] = 0;
        memory_index++;
    }
    if (index >= 960) {
        res->realtime_mem += size;
    } else {
        res->available_memory += size;
    }
}

// loads the processes listed in the input file into the dispatch list array
int load_dispatch(char *dispatch_file, int dispatch_list[][9]) {
    FILE *file = fopen(dispatch_file, "r");
    int process_index = 0;
    int resource_index = 0;

    // checking if input file exits
    if (file == NULL) {
        printf("error: '%s' does not exist\n", dispatch_file);
    } else {
        // parse the dispatch list, tokenize, and save the int conversions in the dispatch list
        char row[MAX_CHAR];
        char *token;
        while (feof(file) != true) {
            fgets(row, MAX_CHAR, file);
            token = strtok(row, ",");
            while (token != NULL) {
                dispatch_list[process_index][resource_index] = atoi(token);
                token = strtok(NULL, ",");
                resource_index++;
            }
            process_index++;
            resource_index = 0;
        }
    }
    fclose(file);
    return process_index;
}

// add each process to the job dispatch queue, filled according to the arrival time of each process
// priority and real time queues are filled according to resource availability
void load_jobs(int time, int num_processes, int dispatch_list[][9], node_t *job_queue, node_t *realtime_queue,
               node_t *first_priority, node_t *second_priority, node_t *third_priority, resources *available_res) {
    int queued_jobs = 0;

    // if dispatcher time = process arrival time, push process into the job queue
    for (int i = 0; i < num_processes; i++) {
        if (time == dispatch_list[i][0]) {
            process *new_process = (process *) malloc(sizeof(process));
            new_process->arrival_time = dispatch_list[i][0];
            new_process->priority = dispatch_list[i][1];
            new_process->processor_time = dispatch_list[i][2];
            new_process->mbytes = dispatch_list[i][3];
            new_process->printers = dispatch_list[i][4];
            new_process->scanners = dispatch_list[i][5];
            new_process->modems = dispatch_list[i][6];
            new_process->cds = dispatch_list[i][7];
            new_process->memory_index = dispatch_list[i][8];

            push(job_queue, new_process);
            new_process = NULL;
        }
    }

    // if resources are available, pop the process from job queue and push into real time or priority
    // if resources aren't available, push it back into the job queue
    if (job_queue->next_node != NULL) {
        node_t *curr = job_queue->next_node;
        queued_jobs++;
        while (curr->next_node != NULL) {
            curr = curr->next_node;
            queued_jobs++;
        }
        curr = NULL;
        for (int i = 0; i < queued_jobs; i++) {
            process *proc = pop(job_queue);
            if (resources_available(proc, available_res)) {
                switch (proc->priority) {
                    case 0:
                        push(realtime_queue, proc);
                        break;
                    case 1:
                        push(first_priority, proc);
                        break;
                    case 2:
                        push(second_priority, proc);
                        break;
                    case 3:
                        push(third_priority, proc);
                        break;
                    default:
                        break;
                }
            } else {
                push(job_queue, proc);
            }
            proc = NULL;
        }
    }
}

// returns true if process resources are less than equal to available resources
bool resources_available(process *proc, resources *available_res) {
    int cds = available_res->cds - proc->cds;
    int modems = available_res->modems - proc->modems;
    int printers = available_res->printers - proc->printers;
    int scanners = available_res->scanners - proc->scanners;
    int memory = 0;

    if (proc->priority == 0) {
        memory = available_res->realtime_mem - proc->mbytes;
    } else {
        memory = available_res->available_memory - proc->mbytes;
    }

//    if (cds >= 0 && modems >= 0 && printers >= 0 && scanners >= 0 && memory >= 0) {
//        return true;
//    } else {
//        return false;
//    }

    return cds >= 0 && modems >= 0 && printers >= 0 && scanners >= 0 && memory >= 0;
}


// returns true if all queues are empty
bool queues_empty(node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority,
                  node_t *third_priority) {
    if (job_queue->next_node == NULL && realtime_queue->next_node == NULL && first_priority->next_node == NULL &&
        second_priority->next_node == NULL && third_priority->next_node == NULL) {
        return true;
    } else {
        return false;
    }
}
