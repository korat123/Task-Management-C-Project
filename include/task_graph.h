#ifndef TASK_GRAPH_H
#define TASK_GRAPH_H

/*
 * task_graph.h  —  Public interface for the Task & Dependency Graph module.
 *
 * Data model summary:
 *   - Tasks are stored as a singly linked list (insertion order preserved).
 *   - Dependencies are stored as a directed adjacency list: an edge A → B
 *     means "Task A must be completed before Task B can start."
 *   - inDegree[id] tracks how many PENDING predecessors a task has.
 *     When inDegree[id] == 0, the task is "ready" (no remaining blockers).
 *   - Cycle detection (DFS) runs before every addDependency() call to
 *     prevent circular dependency chains from ever entering the graph.
 */

/* ── Constants ───────────────────────────────────────────────────────────── */

#define MAX_TASKS     50    /* Hard cap on tasks per user workspace             */
#define MAX_TASK_NAME 100   /* Max task name length, including null terminator  */

/* ── Enumerations ────────────────────────────────────────────────────────── */

/*
 * TaskPriority — Urgency level of a task.
 * Lower numeric value = higher urgency. The priority queue uses this
 * ordering so High tasks are always dequeued before Medium or Low ones.
 */
typedef enum {
    PRIORITY_HIGH   = 1,
    PRIORITY_MEDIUM = 2,
    PRIORITY_LOW    = 3
} TaskPriority;

/*
 * TaskStatus — Completion state of a task.
 * Phase 3 will change a task from PENDING to DONE via markDone().
 * Phase 4 will revert DONE back to PENDING via the Undo stack.
 */
typedef enum {
    STATUS_PENDING = 0,
    STATUS_DONE    = 1
} TaskStatus;

/* ── Data Structures ─────────────────────────────────────────────────────── */

/*
 * Task  —  One node in the task linked list.
 *
 * The `next` pointer makes this an intrusive singly linked list — no
 * separate wrapper node is needed, keeping allocation simple.
 */
typedef struct Task {
    int          taskID;
    char         name[MAX_TASK_NAME];
    TaskPriority priority;
    TaskStatus   status;
    struct Task *next;
} Task;

/*
 * AdjNode  —  One entry in an adjacency list.
 *
 * adjList[fromID] is a linked list of AdjNodes, where each node holds
 * the ID of a task that depends on fromID (i.e., fromID → node.taskID).
 */
typedef struct AdjNode {
    int              taskID;
    struct AdjNode  *next;
} AdjNode;

/*
 * TaskGraph  —  The directed dependency graph for one user's workspace.
 *
 * Indexed by task ID (1-based). Index 0 is reserved and unused.
 * adjList[i]  — head of the list of tasks that depend on task i.
 * inDegree[i] — number of PENDING tasks that must finish before task i.
 * taskCount   — total number of tasks currently in the workspace.
 * nextID      — the ID that will be assigned to the next new task.
 */
typedef struct {
    AdjNode *adjList[MAX_TASKS + 1];
    int      inDegree[MAX_TASKS + 1];
    int      taskCount;
    int      nextID;
} TaskGraph;

/* ── Function Prototypes ─────────────────────────────────────────────────── */

/* ── Graph lifecycle ─────────────────────────────────────────────────────── */

/* Zero-initialize all graph fields; must be called before first use. */
void  initGraph(TaskGraph *graph);

/* Free all AdjNode heap memory and reset all graph fields. */
void  freeGraph(TaskGraph *graph);

/* Free all Task nodes in the linked list and set *head to NULL. */
void  freeTaskList(Task **head);

/* ── Task operations ─────────────────────────────────────────────────────── */

/*
 * addTask  —  Appends a new task to the list and registers it in the graph.
 * Returns the auto-assigned task ID on success, or -1 if the task cap is full.
 */
int   addTask(Task **head, TaskGraph *graph,
              const char *name, TaskPriority priority);

/* Linear search for a task by ID; returns NULL if not found. */
Task *findTaskByID(Task *head, int taskID);

/* ── Dependency & cycle detection ────────────────────────────────────────── */

/*
 * addDependency  —  Adds a directed edge fromID → toID after cycle-checking.
 *
 * Return codes:
 *   1   — Edge accepted and added.
 *   0   — Rejected: would create a circular dependency.
 *  -1   — fromID task does not exist.
 *  -2   — toID task does not exist.
 *  -3   — Self-dependency (fromID == toID).
 *  -4   — Duplicate: this exact edge already exists.
 *  -5   — Memory allocation failed.
 */
int   addDependency(TaskGraph *graph, Task *head, int fromID, int toID);

/*
 * detectCycle  —  DFS-based cycle detector.
 * Returns 1 if a cycle exists in the current graph, 0 if the graph is a DAG.
 */
int   detectCycle(TaskGraph *graph, Task *head);

/* ── Dashboard ───────────────────────────────────────────────────────────── */

/*
 * displayDashboard  —  Renders the two-zone auto-schedule view to stdout.
 *   Zone 1 (Ready)   — PENDING tasks with inDegree 0, sorted by priority.
 *   Zone 2 (Blocked) — PENDING tasks with inDegree > 0, listing predecessors.
 */
void  displayDashboard(Task *head, TaskGraph *graph);

/* ── File persistence ────────────────────────────────────────────────────── */

/* Write all tasks and edges to "<username>_tasks.txt". */
void  saveTasksToFile(Task *head, TaskGraph *graph, const char *username);

/*
 * loadTasksFromFile  —  Restore task list and graph from "<username>_tasks.txt".
 * Returns 1 if the file was found and loaded, 0 if no file exists yet.
 */
int   loadTasksFromFile(Task **head, TaskGraph *graph, const char *username);

/* ── Task modification ───────────────────────────────────────────────────── */

/*
 * deleteTask  —  Removes a task and cleans up all graph references.
 *
 * Decrements inDegree of successors (if the task was PENDING), frees the
 * adjacency list, removes all incoming edges pointing to this task from
 * other lists, then unlinks and frees the Task node.
 *
 * Return codes:
 *   1  — Task deleted successfully.
 *  -1  — Task not found.
 */
int  deleteTask(Task **head, TaskGraph *graph, int taskID);

/*
 * markDone  —  Marks a PENDING task as DONE and decrements successors'
 * in-degrees so that newly unblocked tasks become ready. Dependency edges
 * are left in place so undoMarkDone can re-increment without rebuilding.
 *
 * Return codes:
 *   1  — Task marked Done successfully.
 *   0  — Task is already Done; no change made.
 *  -1  — Task not found.
 */
int  markDone(Task *head, TaskGraph *graph, int taskID);

/*
 * undoMarkDone  —  Reverts a DONE task back to PENDING and re-increments
 * its successors' in-degrees. Used by the Undo Stack (Phase 4).
 *
 * Return codes:
 *   1  — Task reverted to Pending successfully.
 *   0  — Task is already Pending; no change made.
 *  -1  — Task not found (e.g. it was deleted after being marked Done).
 */
int  undoMarkDone(Task *head, TaskGraph *graph, int taskID);

/*
 * searchTaskByName  —  Case-insensitive substring search on task names.
 * Prints a formatted table of matching tasks to stdout.
 * Returns the number of matching tasks found (0 if none).
 */
int  searchTaskByName(Task *head, const char *keyword);

/*
 * topologicalSortDisplay  —  Kahn's BFS topological sort display.
 *
 * Uses a Queue (queue.h) to process tasks in BFS order. Prints the
 * suggested execution sequence for all PENDING tasks. If a cycle is
 * detected in the PENDING subgraph, a warning is printed instead.
 *
 * Algorithm: O(V + E) where V = PENDING tasks, E = dependency edges.
 */
void topologicalSortDisplay(Task *head, TaskGraph *graph);

/* ── Utility ─────────────────────────────────────────────────────────────── */

const char *priorityToString(TaskPriority priority);
const char *statusToString(TaskStatus status);

#endif /* TASK_GRAPH_H */
