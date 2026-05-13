#include <stdlib.h>
#include "queue.h"

/*
 * queue.c  —  FIFO Queue implementation for Kahn's BFS topological sort.
 *
 * Internally a singly linked list: front pointer for O(1) dequeue,
 * rear pointer for O(1) enqueue. Both pointers are NULL when empty.
 */

void q_init(Queue *q) {
    q->front = NULL;
    q->rear  = NULL;
}

void q_enqueue(Queue *q, int taskID) {
    QNode *newNode = (QNode *)malloc(sizeof(QNode));
    if (newNode == NULL) return;

    newNode->taskID = taskID;
    newNode->next   = NULL;

    if (q->rear == NULL) {
        /* Queue was empty — both pointers point to the single node. */
        q->front = newNode;
        q->rear  = newNode;
    } else {
        q->rear->next = newNode;
        q->rear       = newNode;
    }
}

int q_dequeue(Queue *q) {
    QNode *oldFront;
    int    taskID;

    if (q->front == NULL) return -1;

    taskID   = q->front->taskID;
    oldFront = q->front;
    q->front = q->front->next;

    /* If the list is now empty, reset rear too. */
    if (q->front == NULL) q->rear = NULL;

    free(oldFront);
    return taskID;
}

int q_isEmpty(const Queue *q) {
    return q->front == NULL;
}

void q_free(Queue *q) {
    while (!q_isEmpty(q))
        q_dequeue(q);
}
