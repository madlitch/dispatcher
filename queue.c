#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// pushes a node onto a queue
// the queue is accessed from the tail, so we can just insert the process
void push(node_t *tail, process *proc) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));
    new_node->proc = proc;
    new_node->next_node = tail->next_node;
    tail->next_node = new_node;
}

// returns the lengths of the queue by recursively traversing it until the end
int length(node_t *tail, int len) {
    if (tail->next_node != NULL) {
        return length(tail->next_node, len + 1);
    } else {
        return len;
    }
}

// pops a process from the queue
// since the queue is accessed through the tail instead of the head we need to iterate to the front of the
// queue to pop the first element that was pushed (FIFO)
process *pop(node_t *tail) {
    process *return_process;
    node_t *current_node, *previous_node;
    if (tail->next_node == NULL) {
        return NULL;
    } else {
        current_node = tail->next_node;
        previous_node = tail;
        while (current_node->next_node != NULL) {
            previous_node = current_node;
            current_node = current_node->next_node;
        }
    }
    previous_node->next_node = NULL;
    return_process = current_node->proc;
    return return_process;
}
