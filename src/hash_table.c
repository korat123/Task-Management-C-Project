#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"

// The Hash Table Array
HashNode* hash_table[TABLE_SIZE];

// Initialize table with NULLs
void init_hash_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hash_table[i] = NULL;
    }
}

// DJB2 Hash Algorithm for strings
unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;
    // Case-insensitive hashing
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + tolower(c); // hash * 33 + c
    }
    return hash % TABLE_SIZE;
}

// Insert a Task pointer into the Hash Table
void insert_tag(Task* task) {
    if (strlen(task->tag) == 0) return;

    unsigned long index = hash_string(task->tag);

    // Create new HashNode
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->task_ptr = task;

    // Insert at head of the chain (O(1) insertion)
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

// Search and print all tasks with a matching tag (O(1) average time)
void search_by_tag(const char* search_tag) {
    unsigned long index = hash_string(search_tag);
    HashNode* current = hash_table[index];
    int found = 0;

    printf("\n--- Tasks tagged with '%s' ---\n", search_tag);
    while (current != NULL) {
        if (strcasecmp(current->task_ptr->tag, search_tag) == 0) {
            printf("  [ID: %d] %s\n",
                   current->task_ptr->taskID,
                   current->task_ptr->name);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No tasks found with tag: %s\n", search_tag);
    }
}