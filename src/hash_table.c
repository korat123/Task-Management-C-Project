#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"

/*
 * hash_table.c  —  Hash Table module for categorising tasks by tag.
 *
 * Implementation: open-chaining (linked list per bucket).
 * Each HashNode holds a raw pointer to an existing Task — the Task's
 * lifetime is managed by the task linked list. clear_hash_table() must
 * be called before freeTaskList() on logout to avoid dangling pointers.
 *
 * Hash function: DJB2 (case-insensitive), chosen for good distribution
 * on short ASCII strings such as tag names.
 */

/* Global hash table array. Zero-initialised by the C runtime at startup;
 * reset to all-NULL by clear_hash_table() on every user logout. */
HashNode *hash_table[TABLE_SIZE];

void init_hash_table(void) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++)
        hash_table[i] = NULL;
}

/* DJB2 hash — case-insensitive so "Work" and "work" map to the same bucket. */
static unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + tolower(c);
    return hash % TABLE_SIZE;
}

void insert_tag(Task *task) {
    HashNode     *new_node;
    unsigned long index;

    if (strlen(task->tag) == 0) return;

    index    = hash_string(task->tag);
    new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL) return;

    new_node->task_ptr    = task;
    new_node->next        = hash_table[index];
    hash_table[index]     = new_node;
}

void remove_tag(int taskID) {
    int       i;
    HashNode *prev;
    HashNode *cur;
    HashNode *next;

    for (i = 0; i < TABLE_SIZE; i++) {
        prev = NULL;
        cur  = hash_table[i];
        while (cur != NULL) {
            next = cur->next;
            if (cur->task_ptr->taskID == taskID) {
                if (prev == NULL)
                    hash_table[i] = next;
                else
                    prev->next = next;
                free(cur);
            } else {
                prev = cur;
            }
            cur = next;
        }
    }
}

void clear_hash_table(void) {
    int       i;
    HashNode *cur;
    HashNode *next;

    for (i = 0; i < TABLE_SIZE; i++) {
        cur = hash_table[i];
        while (cur != NULL) {
            next = cur->next;
            free(cur);
            cur = next;
        }
        hash_table[i] = NULL;
    }
}

void search_by_tag(const char *search_tag) {
    unsigned long  index   = hash_string(search_tag);
    HashNode      *current = hash_table[index];
    int            found   = 0;

    printf("\n--- Tasks tagged with '%s' ---\n", search_tag);
    while (current != NULL) {
        /* strcasecmp for case-insensitive exact tag match within the bucket. */
        if (strcasecmp(current->task_ptr->tag, search_tag) == 0) {
            printf("  [ID: %d] %s\n",
                   current->task_ptr->taskID,
                   current->task_ptr->name);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
        printf("  No tasks found with tag: %s\n", search_tag);
}
