#include "priority_queue.h"

/*
 * priority_queue.c  —  Implementation of the Priority Queue module.
 *
 * Phase 3 TODO: Implement all functions declared in priority_queue.h.
 *
 * Implementation checklist:
 *   [ ] pq_enqueue(queue, taskID, priority)
 *         - Allocate a new PQNode and set its taskID and priority fields.
 *         - Walk the linked list from head to find the first node whose
 *           priority value is greater than the new node's (lower urgency).
 *         - Insert the new node before that position to keep the list sorted.
 *         - If no such node exists, append to the tail.
 *
 *   [ ] pq_dequeue(queue)
 *         - Check pq_isEmpty() and return -1 (invalid ID) if empty.
 *         - Save the taskID from head, advance head to head->next, free the
 *           old node, and return the saved taskID.
 *
 *   [ ] pq_isEmpty(queue)
 *         - Return 1 if queue->head == NULL, 0 otherwise.
 *
 *   [ ] pq_free(queue)
 *         - Walk the list from head to tail, freeing each node.
 *         - Set queue->head to NULL after the loop.
 *
 * Integration point (Phase 3):
 *   topologicalSort() in task_graph.c will call pq_enqueue() for every task
 *   whose in-degree reaches 0, then mainMenu() will call pq_dequeue() to
 *   populate Dashboard Zone 1 in priority order.
 */
