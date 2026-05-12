#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

/*
 * priority_queue.h  —  Public interface for the Priority Queue module.
 *
 * The Priority Queue holds Task IDs for tasks that are currently "ready"
 * (in-degree = 0 after topological processing). Tasks are dequeued in
 * descending urgency order: High (1) before Medium (2) before Low (3).
 *
 * Implementation strategy (Phase 3):
 *   A sorted singly linked list where pq_enqueue() inserts each new node
 *   into its correct position based on the priority value. This gives
 *   O(n) insertion and O(1) dequeue — acceptable for the task counts
 *   expected in this project.
 *
 * ── Phase 3 TODO: Define the following types ────────────────────────────
 *
 *   PQNode  (struct)
 *     int           taskID    — ID of the ready task.
 *     int           priority  — Urgency level (1=High, 2=Medium, 3=Low).
 *     struct PQNode *next     — Pointer to the next node in the queue.
 *
 *   PriorityQueue  (struct)
 *     PQNode *head  — Pointer to the highest-priority node (front of queue).
 *
 * ── Phase 3 TODO: Declare the following function prototypes ─────────────
 *
 *   pq_enqueue()  — Insert a task ID at the correct sorted position.
 *   pq_dequeue()  — Remove and return the task ID at the front (highest priority).
 *   pq_isEmpty()  — Return 1 if the queue contains no nodes, 0 otherwise.
 *   pq_free()     — Release all heap memory held by the queue nodes.
 */

#endif /* PRIORITY_QUEUE_H */
