#ifndef STACK_UNDO_H
#define STACK_UNDO_H

/*
 * stack_undo.h  —  Public interface for the Undo Stack module.
 *
 * The Undo Stack records the IDs of tasks that have been marked as Done,
 * enabling the user to reverse the most recent completion. It follows the
 * standard LIFO (Last In, First Out) discipline.
 *
 * Workflow:
 *   markDone(taskID)  →  stack_push(taskID)
 *   User presses Undo →  taskID = stack_pop()
 *                     →  revert task status to PENDING
 *                     →  restore graph edges and recalculate in-degrees
 *
 * Scope note:
 *   The project specification requires Undo for only the last action, but
 *   implementing a full stack costs nothing extra and makes the module
 *   reusable if multi-level Undo is ever needed.
 *
 * Implementation strategy (Phase 4):
 *   A dynamically allocated singly linked list so the stack can grow
 *   without a fixed size limit. Each node is malloc'd on push and free'd
 *   on pop.
 *
 * ── Phase 4 TODO: Define the following types ────────────────────────────
 *
 *   StackNode  (struct)
 *     int               taskID  — The ID of the completed task that was stored.
 *     struct StackNode *next    — Pointer toward the bottom of the stack.
 *
 *   UndoStack  (struct)
 *     StackNode *top  — Pointer to the most recently pushed node.
 *
 * ── Phase 4 TODO: Declare the following function prototypes ─────────────
 *
 *   stack_push()     — Allocate a new node and push a task ID onto the stack.
 *   stack_pop()      — Remove the top node and return its task ID.
 *   stack_peek()     — Return the top task ID without removing the node.
 *   stack_isEmpty()  — Return 1 if the stack has no nodes, 0 otherwise.
 *   stack_free()     — Release all remaining nodes (called on logout/exit).
 */

#endif /* STACK_UNDO_H */
