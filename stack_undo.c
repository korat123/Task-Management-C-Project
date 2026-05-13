#include <stdio.h>
#include <stdlib.h>

#include "stack_undo.h"

/*
 * stack_undo.c  —  Implementation of the Undo Stack module.
 *
 * Standard LIFO linked-list stack. Push prepends to the head; pop removes
 * the head. The taskID stored is the ID of a task that was just marked
 * Done by markDone() — popping it feeds undoMarkDone() to revert.
 */

void stack_init(UndoStack *s) {
    if (s == NULL) return;
    s->top = NULL;
}

void stack_push(UndoStack *s, int taskID) {
    StackNode *node;

    if (s == NULL) return;

    node = (StackNode *)malloc(sizeof(StackNode));
    if (node == NULL) {
        printf("[ERROR] Out of memory while pushing task %d to undo stack.\n",
               taskID);
        return;
    }
    node->taskID = taskID;
    node->next   = s->top;
    s->top       = node;
}

int stack_pop(UndoStack *s) {
    StackNode *top;
    int        id;

    if (s == NULL || s->top == NULL) return -1;

    top    = s->top;
    id     = top->taskID;
    s->top = top->next;
    free(top);
    return id;
}

int stack_peek(const UndoStack *s) {
    if (s == NULL || s->top == NULL) return -1;
    return s->top->taskID;
}

int stack_isEmpty(const UndoStack *s) {
    if (s == NULL) return 1;
    return s->top == NULL;
}

void stack_free(UndoStack *s) {
    StackNode *cur;
    StackNode *next;

    if (s == NULL) return;

    cur = s->top;
    while (cur != NULL) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    s->top = NULL;
}
