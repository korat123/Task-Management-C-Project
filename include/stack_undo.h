#ifndef STACK_UNDO_H
#define STACK_UNDO_H

/*
 * stack_undo.h  —  Public interface for the Undo Stack module.
 *
 * The Undo Stack records the IDs of tasks that have been marked Done in
 * the current session. The user can revert the most recent completion by
 * popping the top of the stack and feeding the ID back into undoMarkDone().
 *
 * Workflow:
 *   markDone(taskID)  →  stack_push(taskID)
 *   User presses Undo →  taskID = stack_pop(undoStack)
 *                     →  undoMarkDone(taskID)  (restores PENDING + in-degrees)
 *
 * Implementation:
 *   Singly linked list with the most recent push at the top. Heap nodes
 *   are allocated per push and freed per pop / on stack_free().
 *
 * Persistence:
 *   The stack is in-memory only — it does NOT survive a logout. Each
 *   session starts with an empty undo history.
 */

typedef struct StackNode {
    int               taskID;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} UndoStack;

/* Initialise an empty stack. */
void stack_init(UndoStack *s);

/* Push a task ID onto the top of the stack. */
void stack_push(UndoStack *s, int taskID);

/* Pop and return the top task ID. Returns -1 if the stack is empty. */
int  stack_pop(UndoStack *s);

/* Return the top task ID without removing it. Returns -1 if empty. */
int  stack_peek(const UndoStack *s);

/* Return 1 if the stack has no nodes, 0 otherwise. */
int  stack_isEmpty(const UndoStack *s);

/* Free every remaining node (called on logout). */
void stack_free(UndoStack *s);

#endif /* STACK_UNDO_H */
