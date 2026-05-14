#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "task_graph.h"

#define TABLE_SIZE 97 // A prime number to reduce collisions

// Hash Node points to an existing task and handles collisions via linked list
typedef struct HashNode {
    Task* task_ptr;
    struct HashNode* next;
} HashNode;

void init_hash_table();
void insert_tag(Task* task);
void search_by_tag(const char* tag);

#endif