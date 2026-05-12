#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_graph.h"

/*
 * task_graph.c  —  Implementation of the Task & Dependency Graph module.
 *
 * File format for "<username>_tasks.txt":
 *   TASK|<id>|<priority>|<status>|<name>
 *   DEP|<fromID>|<toID>
 *   NEXTID|<n>
 *
 * Key design decisions:
 *   - Task nodes are appended to the tail of the list to preserve insertion
 *     order, which gives more intuitive display ordering.
 *   - addDependency() adds the edge first, then runs DFS. On a cycle, the
 *     edge is removed. This avoids duplicating the graph state.
 *   - File parsing uses sscanf instead of strtok to handle names with spaces.
 *   - loadTasksFromFile() bypasses addDependency() (no cycle check) because
 *     the data was already validated when it was saved.
 */


/* ─────────────────────────────────────────────────────────────────────────
 * Utility
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * priorityToString  —  Maps a TaskPriority enum value to a display string.
 * Parameters  : priority — the enum value to convert.
 * Return Value: A constant string literal; never NULL.
 */
const char *priorityToString(TaskPriority priority) {
    switch (priority) {
        case PRIORITY_HIGH:   return "High";
        case PRIORITY_MEDIUM: return "Medium";
        case PRIORITY_LOW:    return "Low";
        default:              return "Unknown";
    }
}

/*
 * statusToString  —  Maps a TaskStatus enum value to a display string.
 * Parameters  : status — the enum value to convert.
 * Return Value: A constant string literal; never NULL.
 */
const char *statusToString(TaskStatus status) {
    switch (status) {
        case STATUS_PENDING: return "Pending";
        case STATUS_DONE:    return "Done";
        default:             return "Unknown";
    }
}


/* ─────────────────────────────────────────────────────────────────────────
 * Graph Lifecycle
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * initGraph  —  Zero-initialises a TaskGraph before first use.
 *
 * Purpose:
 *   Sets all adjacency-list pointers to NULL, all in-degree counters to 0,
 *   and the bookkeeping fields to their starting values. Must be called once
 *   before any other graph operation.
 *
 * Parameters  : graph — pointer to the TaskGraph to initialise.
 * Return Value: None.
 */
void initGraph(TaskGraph *graph) {
    int i;
    for (i = 0; i <= MAX_TASKS; i++) {
        graph->adjList[i]  = NULL;
        graph->inDegree[i] = 0;
    }
    graph->taskCount = 0;
    graph->nextID    = 1;
}

/*
 * freeGraph  —  Releases all heap memory held by the graph and resets it.
 *
 * Purpose:
 *   Walks every adjacency list and frees each AdjNode. Also resets all
 *   fields so the graph can be reused (e.g., after a user logs out and
 *   another logs in).
 *
 * Parameters  : graph — pointer to the TaskGraph to free.
 * Return Value: None.
 */
void freeGraph(TaskGraph *graph) {
    int      i;
    AdjNode *current;
    AdjNode *nextNode;

    for (i = 0; i <= MAX_TASKS; i++) {
        current = graph->adjList[i];
        while (current != NULL) {
            nextNode = current->next;
            free(current);
            current = nextNode;
        }
        graph->adjList[i]  = NULL;
        graph->inDegree[i] = 0;
    }
    graph->taskCount = 0;
    graph->nextID    = 1;
}

/*
 * freeTaskList  —  Releases all Task nodes in the linked list.
 *
 * Purpose:
 *   Walks the list from head to tail, freeing each node. Sets *head to
 *   NULL so the caller's pointer is safe to reuse.
 *
 * Parameters  : head — pointer to the list head pointer.
 * Return Value: None.
 */
void freeTaskList(Task **head) {
    Task *current = *head;
    Task *nextTask;

    while (current != NULL) {
        nextTask = current->next;
        free(current);
        current = nextTask;
    }
    *head = NULL;
}


/* ─────────────────────────────────────────────────────────────────────────
 * Task Operations
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * addTask  —  Creates a new Task, appends it to the list, and registers it.
 *
 * Purpose:
 *   Allocates a new Task node with an auto-assigned ID, appends it to the
 *   tail of the linked list (to preserve insertion order), and increments
 *   the graph's task counter and ID generator.
 *
 * Parameters:
 *   head     — pointer to the list head pointer (may be updated if list is empty).
 *   graph    — the workspace graph (taskCount and nextID are updated).
 *   name     — null-terminated task name string.
 *   priority — the urgency level for this task.
 *
 * Return Value:
 *   The new task's auto-assigned ID (>= 1) on success.
 *  -1 if the MAX_TASKS cap has been reached or malloc fails.
 */
int addTask(Task **head, TaskGraph *graph,
            const char *name, TaskPriority priority) {
    Task *newTask;
    Task *tail;

    if (graph->taskCount >= MAX_TASKS) return -1;

    newTask = (Task *)malloc(sizeof(Task));
    if (newTask == NULL) return -1;

    newTask->taskID   = graph->nextID++;
    strncpy(newTask->name, name, MAX_TASK_NAME - 1);
    newTask->name[MAX_TASK_NAME - 1] = '\0';
    newTask->priority = priority;
    newTask->status   = STATUS_PENDING;
    newTask->next     = NULL;

    /* Append to the tail so the list stays in insertion (creation) order. */
    if (*head == NULL) {
        *head = newTask;
    } else {
        tail = *head;
        while (tail->next != NULL)
            tail = tail->next;
        tail->next = newTask;
    }

    graph->taskCount++;
    return newTask->taskID;
}

/*
 * findTaskByID  —  Linear search for a task by its unique ID.
 *
 * Purpose:
 *   Walks the task list and returns a pointer to the first node whose
 *   taskID matches. Used by addDependency, the dashboard, and the UI.
 *
 * Parameters:
 *   head   — head of the task linked list.
 *   taskID — the ID to search for.
 *
 * Return Value: Pointer to the matching Task node, or NULL if not found.
 */
Task *findTaskByID(Task *head, int taskID) {
    Task *current = head;

    while (current != NULL) {
        if (current->taskID == taskID)
            return current;
        current = current->next;
    }
    return NULL;
}


/* ─────────────────────────────────────────────────────────────────────────
 * Cycle Detection (DFS)
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * dfsVisit  —  Recursive DFS helper for cycle detection.
 *
 * Purpose:
 *   Explores the graph from `nodeID` using depth-first search. Maintains
 *   two boolean arrays:
 *     visited[]  — marks nodes that have been fully explored.
 *     inStack[]  — marks nodes on the current DFS recursion path.
 *   A back-edge is detected when a neighbour is already in the current
 *   path (inStack[neighbour] == 1), which means a cycle exists.
 *
 * Parameters:
 *   graph   — the graph to search.
 *   nodeID  — the current node being explored.
 *   visited — array of size MAX_TASKS+1; 1 if node has been visited.
 *   inStack — array of size MAX_TASKS+1; 1 if node is on the DFS path.
 *
 * Return Value: 1 if a cycle is detected, 0 otherwise.
 */
static int dfsVisit(TaskGraph *graph, int nodeID,
                    int *visited, int *inStack) {
    AdjNode *neighbor;
    int      neighborID;

    visited[nodeID] = 1;
    inStack[nodeID] = 1;

    neighbor = graph->adjList[nodeID];
    while (neighbor != NULL) {
        neighborID = neighbor->taskID;

        if (!visited[neighborID]) {
            /* Recurse into unvisited neighbour. */
            if (dfsVisit(graph, neighborID, visited, inStack))
                return 1;
        } else if (inStack[neighborID]) {
            /* Back-edge found — cycle confirmed. */
            return 1;
        }
        neighbor = neighbor->next;
    }

    inStack[nodeID] = 0; /* Remove node from the current path on backtrack. */
    return 0;
}

/*
 * detectCycle  —  Full DFS-based cycle detector over the entire graph.
 *
 * Purpose:
 *   Launches dfsVisit() from every unvisited task node. Because the graph
 *   may be a forest (multiple disconnected components), we must start DFS
 *   from each component's root.
 *
 * Parameters:
 *   graph — the graph to check.
 *   head  — the task list (used to iterate over all valid task IDs).
 *
 * Return Value: 1 if a cycle exists anywhere in the graph, 0 if it is a DAG.
 */
int detectCycle(TaskGraph *graph, Task *head) {
    int  visited[MAX_TASKS + 1];
    int  inStack[MAX_TASKS + 1];
    int  i;
    Task *current;

    for (i = 0; i <= MAX_TASKS; i++) {
        visited[i] = 0;
        inStack[i] = 0;
    }

    current = head;
    while (current != NULL) {
        if (!visited[current->taskID]) {
            if (dfsVisit(graph, current->taskID, visited, inStack))
                return 1;
        }
        current = current->next;
    }
    return 0;
}


/* ─────────────────────────────────────────────────────────────────────────
 * Dependency Management
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * addDependency  —  Adds a directed edge fromID → toID with cycle prevention.
 *
 * Purpose:
 *   Validates both task IDs, checks for self-dependency and duplicates, then
 *   tentatively inserts the edge into the graph. If DFS detects a cycle, the
 *   edge is immediately removed and the graph is restored. Only if the graph
 *   remains acyclic does the function return success.
 *
 * Parameters:
 *   graph  — the workspace graph (modified on success).
 *   head   — the task list (used for ID validation and cycle detection).
 *   fromID — the prerequisite task (must be done first).
 *   toID   — the dependent task (can only start after fromID is done).
 *
 * Return Value:
 *   1   — Edge accepted; dependency is now in effect.
 *   0   — Rejected: would create a circular dependency.
 *  -1   — fromID does not exist in the task list.
 *  -2   — toID does not exist in the task list.
 *  -3   — Self-dependency: fromID == toID.
 *  -4   — Duplicate: this exact edge is already present.
 *  -5   — Memory allocation failure.
 */
int addDependency(TaskGraph *graph, Task *head, int fromID, int toID) {
    AdjNode *existing;
    AdjNode *newNode;

    /* Validate that both tasks exist in the list. */
    if (findTaskByID(head, fromID) == NULL) return -1;
    if (findTaskByID(head, toID)   == NULL) return -2;

    /* A task cannot be its own prerequisite. */
    if (fromID == toID) return -3;

    /* Check for a duplicate edge before allocating. */
    existing = graph->adjList[fromID];
    while (existing != NULL) {
        if (existing->taskID == toID) return -4;
        existing = existing->next;
    }

    /* Allocate the new adjacency node. */
    newNode = (AdjNode *)malloc(sizeof(AdjNode));
    if (newNode == NULL) return -5;

    /* Tentatively insert the edge (prepend for O(1) insertion). */
    newNode->taskID        = toID;
    newNode->next          = graph->adjList[fromID];
    graph->adjList[fromID] = newNode;
    graph->inDegree[toID]++;

    /* Run DFS on the updated graph to check for cycles. */
    if (detectCycle(graph, head)) {
        /* Cycle detected — roll back the edge immediately. */
        graph->adjList[fromID] = newNode->next;
        graph->inDegree[toID]--;
        free(newNode);
        return 0;
    }

    return 1; /* Edge accepted; graph is still a valid DAG. */
}


/* ─────────────────────────────────────────────────────────────────────────
 * Dashboard Display
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * sortByPriority  —  In-place insertion sort of task ID array by priority.
 *
 * Purpose:
 *   Sorts an array of task IDs so that the highest-urgency tasks appear
 *   first (PRIORITY_HIGH=1 before PRIORITY_MEDIUM=2 before PRIORITY_LOW=3).
 *   Uses insertion sort — acceptable since the array holds at most MAX_TASKS
 *   (50) elements.
 *
 * Parameters:
 *   taskIDs — array of task IDs to sort in place.
 *   count   — number of elements in the array.
 *   head    — the task list used to look up each task's priority.
 */
static void sortByPriority(int *taskIDs, int count, Task *head) {
    int   i, j, tempID;
    Task *taskA;
    Task *taskB;

    for (i = 1; i < count; i++) {
        tempID = taskIDs[i];
        taskA  = findTaskByID(head, tempID);
        j      = i - 1;

        while (j >= 0) {
            taskB = findTaskByID(head, taskIDs[j]);
            /* Shift right if the neighbour has a lower urgency (higher number). */
            if (taskB != NULL && taskA != NULL &&
                taskB->priority > taskA->priority) {
                taskIDs[j + 1] = taskIDs[j];
                j--;
            } else {
                break;
            }
        }
        taskIDs[j + 1] = tempID;
    }
}

/*
 * displayDashboard  —  Renders the two-zone auto-schedule view.
 *
 * Purpose:
 *   Partitions all PENDING tasks into two zones based on their in-degree:
 *     Zone 1 (Ready)   — inDegree == 0; sorted by priority (High first).
 *     Zone 2 (Blocked) — inDegree  > 0; shows which tasks are blocking each.
 *   This is the primary output surface of the auto-scheduler.
 *
 * Parameters:
 *   head  — the task linked list.
 *   graph — the dependency graph (inDegree and adjList are read-only here).
 *
 * Return Value: None.
 */
void displayDashboard(Task *head, TaskGraph *graph) {
    int     readyIDs[MAX_TASKS];
    int     blockedIDs[MAX_TASKS];
    int     readyCount   = 0;
    int     blockedCount = 0;
    int     i;
    int     firstPred;
    Task   *task;
    Task   *scanner;
    AdjNode *adj;

    if (head == NULL) {
        printf("\n  No tasks yet. Use 'Add New Task' to create your first task.\n");
        return;
    }

    /* ── Partition PENDING tasks into ready and blocked ───────────────── */
    task = head;
    while (task != NULL) {
        if (task->status == STATUS_PENDING) {
            if (graph->inDegree[task->taskID] == 0)
                readyIDs[readyCount++]      = task->taskID;
            else
                blockedIDs[blockedCount++]  = task->taskID;
        }
        task = task->next;
    }

    /* Sort the ready list: highest priority (lowest enum value) first. */
    sortByPriority(readyIDs, readyCount, head);

    /* ── Zone 1: Ready ────────────────────────────────────────────────── */
    printf("\n ==========================================\n");
    printf("  ZONE 1 -- READY TO WORK ON  (%d task(s))\n", readyCount);
    printf(" ==========================================\n");

    if (readyCount == 0) {
        printf("  (No tasks are currently ready)\n");
    } else {
        printf("  %-4s  %-26s  %-8s\n",
               "ID", "Task Name", "Priority");
        printf("  %-4s  %-26s  %-8s\n",
               "----", "--------------------------", "--------");
        for (i = 0; i < readyCount; i++) {
            task = findTaskByID(head, readyIDs[i]);
            if (task)
                printf("  %-4d  %-26.26s  %-8s\n",
                       task->taskID, task->name,
                       priorityToString(task->priority));
        }
    }

    /* ── Zone 2: Blocked ──────────────────────────────────────────────── */
    printf("\n ==========================================\n");
    printf("  ZONE 2 -- BLOCKED  (%d task(s))\n", blockedCount);
    printf(" ==========================================\n");

    if (blockedCount == 0) {
        printf("  (No tasks are currently blocked)\n");
    } else {
        printf("  %-4s  %-26s  %-8s  %s\n",
               "ID", "Task Name", "Priority", "Waiting For");
        printf("  %-4s  %-26s  %-8s  %s\n",
               "----", "--------------------------", "--------", "-----------");

        for (i = 0; i < blockedCount; i++) {
            task = findTaskByID(head, blockedIDs[i]);
            if (task == NULL) continue;

            printf("  %-4d  %-26.26s  %-8s  ",
                   task->taskID, task->name,
                   priorityToString(task->priority));

            /* Find all PENDING predecessors by scanning every task's adj list.
             * A task P is a predecessor of `task` if adjList[P] contains task->taskID. */
            scanner   = head;
            firstPred = 1;
            while (scanner != NULL) {
                if (scanner->status == STATUS_PENDING) {
                    adj = graph->adjList[scanner->taskID];
                    while (adj != NULL) {
                        if (adj->taskID == task->taskID) {
                            if (!firstPred) printf(", ");
                            printf("#%d", scanner->taskID);
                            firstPred = 0;
                        }
                        adj = adj->next;
                    }
                }
                scanner = scanner->next;
            }
            printf("\n");
        }
    }

    /* ── Summary footer ───────────────────────────────────────────────── */
    {
        int doneCount = 0;
        task = head;
        while (task != NULL) {
            if (task->status == STATUS_DONE) doneCount++;
            task = task->next;
        }
        printf("\n  Total: %d task(s) | Ready: %d | Blocked: %d | Done: %d\n",
               graph->taskCount, readyCount, blockedCount, doneCount);
    }
}


/* ─────────────────────────────────────────────────────────────────────────
 * File Persistence
 * ───────────────────────────────────────────────────────────────────────── */

/*
 * saveTasksToFile  —  Persists the task list and dependency graph to disk.
 *
 * Purpose:
 *   Writes every Task record and every dependency edge to the per-user file
 *   "<username>_tasks.txt". Also records the nextID counter so that IDs
 *   continue from where they left off after a reload. The file is fully
 *   overwritten on each save (not appended), so it always reflects current state.
 *
 * File format:
 *   TASK|<id>|<priority>|<status>|<name>
 *   DEP|<fromID>|<toID>
 *   NEXTID|<nextID>
 *
 * Parameters:
 *   head     — head of the task list to save.
 *   graph    — the dependency graph to save.
 *   username — used to build the filename; must not contain path separators.
 */
void saveTasksToFile(Task *head, TaskGraph *graph, const char *username) {
    char     filename[MAX_TASK_NAME];
    FILE    *file;
    Task    *current;
    AdjNode *adj;
    int      i;

    snprintf(filename, sizeof(filename), "%s_tasks.txt", username);

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("[ERROR] Could not open '%s' for writing.\n", filename);
        return;
    }

    /* Write task records. */
    current = head;
    while (current != NULL) {
        fprintf(file, "TASK|%d|%d|%d|%s\n",
                current->taskID,
                (int)current->priority,
                (int)current->status,
                current->name);
        current = current->next;
    }

    /* Write dependency edges by walking every adjacency list. */
    for (i = 1; i <= MAX_TASKS; i++) {
        adj = graph->adjList[i];
        while (adj != NULL) {
            fprintf(file, "DEP|%d|%d\n", i, adj->taskID);
            adj = adj->next;
        }
    }

    /* Write the ID counter so auto-increment continues correctly after reload. */
    fprintf(file, "NEXTID|%d\n", graph->nextID);

    fclose(file);
}

/*
 * loadTasksFromFile  —  Restores the task list and graph from disk.
 *
 * Purpose:
 *   Reads the per-user file "<username>_tasks.txt" and reconstructs the
 *   in-memory task list and adjacency list. Dependency edges are added
 *   directly (without cycle checking) because the data was already validated
 *   before it was saved.
 *
 * Parameters:
 *   head     — output: pointer to the list head pointer; populated on load.
 *   graph    — output: graph struct to populate with tasks and edges.
 *   username — identifies which file to read.
 *
 * Return Value:
 *   1 — File found and loaded successfully.
 *   0 — File not found (normal for a new user's first session).
 */
int loadTasksFromFile(Task **head, TaskGraph *graph, const char *username) {
    char  filename[MAX_TASK_NAME];
    char  lineBuffer[MAX_TASK_NAME + 30];
    FILE *file;
    Task *newTask;
    Task *tail;
    AdjNode *newNode;
    int   id, pri, sta, fromID, toID, nextID;
    char  name[MAX_TASK_NAME];

    snprintf(filename, sizeof(filename), "%s_tasks.txt", username);

    file = fopen(filename, "r");
    if (file == NULL)
        return 0; /* No task file yet — normal for a brand-new user. */

    tail = NULL; /* Track the list tail for O(1) append during load. */

    while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        lineBuffer[strcspn(lineBuffer, "\n")] = '\0';

        if (strncmp(lineBuffer, "TASK|", 5) == 0) {
            /* Format: TASK|id|priority|status|name */
            if (sscanf(lineBuffer, "TASK|%d|%d|%d|%99[^\n]",
                       &id, &pri, &sta, name) != 4)
                continue;

            newTask = (Task *)malloc(sizeof(Task));
            if (newTask == NULL) continue;

            newTask->taskID   = id;
            strncpy(newTask->name, name, MAX_TASK_NAME - 1);
            newTask->name[MAX_TASK_NAME - 1] = '\0';
            newTask->priority = (TaskPriority)pri;
            newTask->status   = (TaskStatus)sta;
            newTask->next     = NULL;

            /* Append to the tail — preserves the original insertion order. */
            if (*head == NULL) {
                *head = newTask;
                tail  = newTask;
            } else {
                tail->next = newTask;
                tail       = newTask;
            }
            graph->taskCount++;

        } else if (strncmp(lineBuffer, "DEP|", 4) == 0) {
            /* Format: DEP|fromID|toID */
            if (sscanf(lineBuffer, "DEP|%d|%d", &fromID, &toID) != 2)
                continue;

            newNode = (AdjNode *)malloc(sizeof(AdjNode));
            if (newNode == NULL) continue;

            /* Prepend to adjList (no cycle check — file data is trusted). */
            newNode->taskID           = toID;
            newNode->next             = graph->adjList[fromID];
            graph->adjList[fromID]    = newNode;
            graph->inDegree[toID]++;

        } else if (strncmp(lineBuffer, "NEXTID|", 7) == 0) {
            /* Format: NEXTID|n */
            if (sscanf(lineBuffer, "NEXTID|%d", &nextID) == 1)
                graph->nextID = nextID;
        }
    }

    fclose(file);
    return 1;
}
