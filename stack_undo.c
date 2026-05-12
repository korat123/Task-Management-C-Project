#include "stack_undo.h"

/*
 * stack_undo.c  —  Implementation of the Undo Stack module.
 *
 * Phase 4 TODO: Implement all functions declared in stack_undo.h.
 *
 * Implementation checklist:
 *   [ ] stack_push(stack, taskID)
 *         - Allocate a new StackNode and set its taskID field.
 *         - Point the new node's next at the current stack->top.
 *         - Update stack->top to the new node.
 *
 *   [ ] stack_pop(stack)
 *         - Return -1 (invalid ID) if stack_isEmpty() is true.
 *         - Save the taskID from stack->top.
 *         - Advance stack->top to stack->top->next.
 *         - Free the old top node.
 *         - Return the saved taskID.
 *
 *   [ ] stack_peek(stack)
 *         - Return stack->top->taskID without modifying the stack.
 *         - Return -1 if the stack is empty.
 *
 *   [ ] stack_isEmpty(stack)
 *         - Return 1 if stack->top == NULL, 0 otherwise.
 *
 *   [ ] stack_free(stack)
 *         - Pop all remaining nodes to release heap memory.
 *         - Called on user logout or program exit to prevent memory leaks.
 *
 * Integration point (Phase 4):
 *   markDone() in task_graph.c calls stack_push() after changing a task's
 *   status to DONE. The Undo option in mainMenu() calls stack_pop() to get
 *   the last completed task ID, then reverts it and refreshes the graph.
 */
