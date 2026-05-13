#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"

/*
 * priority_queue.c  —  Implementation of the Priority Queue module.
 *
 * Strategy: sorted singly linked list. The strict-greater-than comparison
 * inside pq_enqueue() means equal-priority items keep insertion order,
 * which matches how the dashboard expects to display ready tasks.
 */

void pq_init(PriorityQueue *pq) {
    if (pq == NULL) return;
    pq->head = NULL;
}

void pq_enqueue(PriorityQueue *pq, int taskID, int priority) {
    PQNode *node;
    PQNode *cur;
    PQNode *prev;

    if (pq == NULL) return;

    node = (PQNode *)malloc(sizeof(PQNode));
    if (node == NULL) {
        printf("[ERROR] Out of memory while enqueueing task %d.\n", taskID);
        return;
    }
    node->taskID   = taskID;
    node->priority = priority;
    node->next     = NULL;

    /* Walk to the first node whose priority value is strictly greater
     * (i.e. less urgent) than the new node. Insert before that node. */
    prev = NULL;
    cur  = pq->head;
    while (cur != NULL && cur->priority <= priority) {
        prev = cur;
        cur  = cur->next;
    }

    if (prev == NULL) {
        node->next = pq->head;
        pq->head   = node;
    } else {
        node->next = cur;
        prev->next = node;
    }
}

int pq_dequeue(PriorityQueue *pq) {
    PQNode *front;
    int     id;

    if (pq == NULL || pq->head == NULL) return -1;

    front    = pq->head;
    id       = front->taskID;
    pq->head = front->next;
    free(front);
    return id;
}

int pq_isEmpty(const PriorityQueue *pq) {
    if (pq == NULL) return 1;
    return pq->head == NULL;
}

void pq_free(PriorityQueue *pq) {
    PQNode *cur;
    PQNode *next;

    if (pq == NULL) return;

    cur = pq->head;
    while (cur != NULL) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    pq->head = NULL;
}
