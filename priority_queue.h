#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

/*
 * priority_queue.h  —  Public interface for the Priority Queue module.
 *
 * The Priority Queue holds Task IDs for tasks that are currently "ready"
 * (in-degree = 0 and status = PENDING). Tasks are dequeued in ascending
 * priority-value order: High (1) before Medium (2) before Low (3).
 *
 * Implementation: a sorted singly linked list. pq_enqueue() inserts each
 * new node at the first position whose priority value is strictly greater
 * than the new one, which preserves FIFO ordering within a priority class.
 * O(n) insertion, O(1) dequeue — acceptable for project task counts.
 */

typedef struct PQNode {
    int            taskID;
    int            priority;   /* 1=High, 2=Medium, 3=Low (lower = more urgent) */
    struct PQNode *next;
} PQNode;

typedef struct {
    PQNode *head;              /* Front of queue (highest priority). */
} PriorityQueue;

/* Initialise an empty queue. */
void pq_init(PriorityQueue *pq);

/* Insert taskID at the correct sorted position. */
void pq_enqueue(PriorityQueue *pq, int taskID, int priority);

/* Remove and return the front task ID. Returns -1 if the queue is empty. */
int pq_dequeue(PriorityQueue *pq);

/* Return 1 if the queue has no nodes, 0 otherwise. */
int pq_isEmpty(const PriorityQueue *pq);

/* Free every remaining node and reset head to NULL. */
void pq_free(PriorityQueue *pq);

#endif /* PRIORITY_QUEUE_H */
