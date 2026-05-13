#ifndef QUEUE_H
#define QUEUE_H

/*
 * queue.h  —  Public interface for the FIFO Queue module.
 *
 * This Queue stores integer task IDs and is used by Kahn's BFS
 * topological sort algorithm in task_graph.c. It is a classic
 * FIFO (First In, First Out) queue backed by a singly linked list:
 *   - enqueue appends to the rear  — O(1)
 *   - dequeue removes from the front — O(1)
 *
 * Why a linked-list queue instead of an array-based queue?
 *   An array queue requires a fixed capacity or ring-buffer management.
 *   A linked-list queue grows dynamically and keeps the implementation
 *   simple — the only cost is one malloc/free per operation, which is
 *   negligible for the task counts this project uses.
 */

/* ── Data Structures ─────────────────────────────────────────────────────── */

typedef struct QNode {
    int          taskID;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
} Queue;

/* ── Function Prototypes ─────────────────────────────────────────────────── */

/* Zero-initialise a Queue before first use. */
void q_init(Queue *q);

/* Append a task ID to the rear of the queue. */
void q_enqueue(Queue *q, int taskID);

/* Remove and return the task ID at the front; returns -1 if empty. */
int  q_dequeue(Queue *q);

/* Return 1 if the queue contains no nodes, 0 otherwise. */
int  q_isEmpty(const Queue *q);

/* Release all remaining nodes (call when done with the queue). */
void q_free(Queue *q);

#endif /* QUEUE_H */
