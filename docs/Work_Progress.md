# Work Progress — Smart Task Management System

**Project:** Smart Task Management System &nbsp;|&nbsp; **Phases Done:** 4 / 4 &nbsp;|&nbsp; **Last Updated:** 2026-05-14

---

## How to Build & Run

Any teammate can clone the repo and be running the project in one command:

```bash
# Compile (run from the project root folder)
gcc -Wall -Wextra -I include -o task_manager \
    src/main.c src/auth.c src/task_graph.c \
    src/priority_queue.c src/stack_undo.c src/queue.c

# Run on Windows
task_manager.exe

# Run on Linux / macOS
./task_manager
```

> All six `.c` files must be on the compile line. The `-I include` flag lets the compiler
> find all headers in the `include/` folder without changing any `#include` directives.

---

## Development Timeline

A stamp-by-stamp log of every significant milestone. New entries are added at the
bottom after each phase or bugfix is completed.

| Date       | Phase   | Milestone |
|------------|---------|-----------|
| 2026-05-12 | Setup   | Repository created on GitHub (`korat123/Task-Management-C-Project`) |
| 2026-05-12 | Setup   | `README.md` written — defines scope, constraints, and 4-phase plan |
| 2026-05-12 | Setup   | All 10 source files scaffolded with include guards and TODO stubs |
| 2026-05-12 | Phase 1 | `auth.h` defined: `User` struct, constants (`MAX_USERNAME`, `MAX_USERS`), prototypes |
| 2026-05-12 | Phase 1 | `auth.c` implemented: `loadUsers`, `saveUser`, `registerUser`, `loginUser` |
| 2026-05-12 | Phase 1 | `main.c` UI scaffold: `clearScreen` (cross-platform), `printBanner`, `authMenu`, `mainMenu` stub |
| 2026-05-12 | Phase 1 | `Claude.md` created — AI execution plan and project design decisions |
| 2026-05-12 | Refactor| Full language refactor: all UI text and code comments changed from Thai → English |
| 2026-05-12 | Refactor| `const` qualifiers added to non-mutating function parameters in `auth.h` |
| 2026-05-12 | Phase 1 | Phase 1 committed to GitHub and tested — register, login, logout all working |
| 2026-05-12 | Phase 2 | `task_graph.h` defined: `TaskPriority` / `TaskStatus` enums, `Task` / `AdjNode` / `TaskGraph` structs |
| 2026-05-12 | Phase 2 | `task_graph.c` — graph lifecycle: `initGraph`, `freeGraph`, `freeTaskList` |
| 2026-05-12 | Phase 2 | `task_graph.c` — task operations: `addTask` (auto-ID, append to tail), `findTaskByID` |
| 2026-05-12 | Phase 2 | `task_graph.c` — DFS cycle detection: `dfsVisit` (recursive), `detectCycle` (full graph) |
| 2026-05-12 | Phase 2 | `task_graph.c` — `addDependency`: tentative insert → DFS check → rollback on cycle (6 error codes) |
| 2026-05-12 | Phase 2 | `task_graph.c` — `displayDashboard`: Zone 1 (ready, sorted by priority), Zone 2 (blocked + predecessor list) |
| 2026-05-12 | Phase 2 | `task_graph.c` — `saveTasksToFile` / `loadTasksFromFile`: flat-file format `TASK\|DEP\|NEXTID` |
| 2026-05-12 | Phase 2 | `main.c` updated — `mainMenu` wired to options 1 (dashboard), 2 (add task), 3 (set dependency) |
| 2026-05-12 | Phase 2 | Per-user task file (`<username>_tasks.txt`) created on first save, loaded on next login |
| 2026-05-12 | Bugfix  | Empty-Enter guard added: bare Enter no longer silently triggers Exit or Logout |
| 2026-05-12 | Bugfix  | `strcspn` newline strip and empty-input check added to Task ID fields in Set Dependency |
| 2026-05-13 | Docs    | `Work_Progress.md` created — team timeline, phase status, file map, contributor rules |
| 2026-05-13 | Docs    | Git Workflow Rules added to `Claude.md`: no auto-commit; user commits manually |
| 2026-05-13 | Docs    | Phase 3 & 4 checklists in `Claude.md` expanded with critical implementation notes |
| 2026-05-14 | Feature | `queue.h` / `queue.c` — new FIFO Queue module (linked list); used by Kahn's BFS |
| 2026-05-14 | Feature | `task_graph.c` — `deleteTask()`: removes task + repairs all graph references |
| 2026-05-14 | Feature | `task_graph.c` — `searchTaskByName()`: case-insensitive substring search |
| 2026-05-14 | Feature | `task_graph.c` — `topologicalSortDisplay()`: Kahn's BFS algorithm using Queue |
| 2026-05-14 | Feature | `main.c` — wired menu options [6] Delete, [7] Search, [8] Topological Order |
| 2026-05-14 | Phase 3 | `priority_queue.h` / `priority_queue.c` — sorted linked-list PQ: `pq_init`, `pq_enqueue` (stable), `pq_dequeue`, `pq_isEmpty`, `pq_free` |
| 2026-05-14 | Phase 3 | `task_graph.c` — replaced `sortByPriority` insertion sort in `displayDashboard` Zone 1 with PriorityQueue enqueue/dequeue |
| 2026-05-14 | Phase 3 | `task_graph.c` — added `markDone()`: PENDING → DONE + decrement successors' `inDegree`; edges left intact for Undo |
| 2026-05-14 | Phase 3 | `task_graph.c` — `loadTasksFromFile` fix-up pass: subtract DONE tasks' edge contributions so previously-unlocked tasks no longer re-block on reload |
| 2026-05-14 | Phase 3 | `main.c` — wired menu option [4] Mark Task as Done (pre-check, list pending, prompt, refresh dashboard) |
| 2026-05-14 | Phase 4 | `stack_undo.h` / `stack_undo.c` — LIFO undo stack: `stack_init`, `stack_push`, `stack_pop`, `stack_peek`, `stack_isEmpty`, `stack_free` |
| 2026-05-14 | Phase 4 | `task_graph.c` — added `undoMarkDone()`: DONE → PENDING + re-increment successors' `inDegree` |
| 2026-05-14 | Phase 4 | `main.c` — wired menu option [5] Undo Last Action; in-memory undo stack lives in `mainMenu`, freed on logout; delete-then-undo handled gracefully |
| 2026-05-14 | Bugfix  | `addDependency` — skip `inDegree[toID]++` (and matching rollback) when source is already DONE; DONE tasks are not PENDING predecessors |
| 2026-05-14 | Refactor | All runtime data files moved into `data/` subfolder; `_mkdir("data")` call added to `main()` startup; `USERS_FILE` and task filename `snprintf` updated to use `data/` prefix |
| 2026-05-14 | Refactor | Project reorganized: `.c` files → `src/`, `.h` files → `include/`, docs → `docs/`; `CLAUDE.md` build command updated with `-I include` and `src/` paths |

---

## Phase Status

| Phase | Name | Key Deliverables | Status |
|-------|------|-----------------|--------|
| 1 | Foundation & Authentication | `clearScreen`, `authMenu`, `registerUser`, `loginUser`, `users.txt` persistence | ✅ Done |
| 2 | Task & Dependency Graph | Task linked list, adjacency list graph, DFS cycle check, auto-schedule dashboard, `<user>_tasks.txt` | ✅ Done |
| 3 | Priority Queue & Mark Done | `PriorityQueue` struct, `pq_enqueue`/`pq_dequeue`, `markDone()`, Zone 1 sorted by queue | ✅ Done |
| 4 | Undo Stack & Full Integration | `UndoStack` struct, `stack_push`/`stack_pop`, Undo reverts last Done, end-to-end test | ✅ Done |

---

## File Map

Quick reference — who owns what, and which phase it belongs to.

| File(s) | Phase | Responsibility |
|---------|-------|---------------|
| `src/main.c` | 1 – 4 | UI controller: all menus, input handling, session lifecycle |
| `include/auth.h` / `src/auth.c` | 1 | User registration, login, `data/users.txt` read/write |
| `include/task_graph.h` / `src/task_graph.c` | 2 | Task linked list, directed dependency graph, DFS, dashboard, delete, search, topo sort, file I/O |
| `include/queue.h` / `src/queue.c` | 2 | FIFO Queue (linked list); used by Kahn's BFS topological sort |
| `include/priority_queue.h` / `src/priority_queue.c` | 3 | Sorted linked-list priority queue for ranking ready tasks |
| `include/stack_undo.h` / `src/stack_undo.c` | 4 | LIFO stack that stores completed task IDs for Undo |
| `README.md` | — | Project specification and technical constraints |
| `CLAUDE.md` | — | AI execution plan and permanent development standards |
| `docs/Work_Progress.md` | — | **This file** — team timeline and status tracker |
| `data/` | — | Runtime data files (`users.txt`, `<user>_tasks.txt`) — auto-created at startup |
| `.gitignore` | — | Excludes `*.exe`, `*.o`, and `data/` contents from git |

---

## What Works Right Now

Teammates can run the program today and test all of these:

- **Register** a new account — credentials saved to `data/users.txt`
- **Login** with username and password — incorrect credentials are rejected
- **Add tasks** with a name and priority (High / Medium / Low) — auto-assigns an ID
- **Set dependencies** between tasks — e.g., "Task #1 must be done before Task #2"
- **Circular dependency protection** — the system rejects any chain that would loop back on itself
- **Auto-Schedule Dashboard** — Zone 1 shows tasks ready to work on (sorted by priority); Zone 2 shows tasks still blocked and which task ID is blocking them
- **Delete Task** `[6]` — removes a task and automatically repairs inDegrees for its successors
- **Search Task** `[7]` — case-insensitive substring search on task names, displays all matches
- **Topological Execution Order** `[8]` — Kahn's BFS algorithm shows the correct completion sequence
- **Mark Task as Done** `[4]` — flips a PENDING task to DONE, decrements successors' in-degree, and unlocks newly ready tasks live in the dashboard
- **Undo Last Action** `[5]` — pops the most recent completion off the in-memory undo stack and reverts it to PENDING (in-session only; resets on logout)
- **Priority Queue** powers Dashboard Zone 1 — High before Medium before Low, with stable insertion order within a priority class
- **Data persistence** — tasks and dependencies survive logout and program restart, including the fix-up pass that prevents DONE tasks from wrongly re-blocking successors on reload

---

## What Is Not Yet Implemented

All four phases are complete. The Phase 3 and Phase 4 implementation checklists below
are kept as a historical reference for how the modules were built.

---

## Phase 3 — Full Implementation Checklist

> **Read `Claude.md` Phase 3 section first** — it has all design decisions and critical traps.

### `priority_queue.h` — define types and prototypes

```c
typedef struct PQNode {
    int taskID;
    int priority;       /* 1=High, 2=Medium, 3=Low */
    struct PQNode *next;
} PQNode;

typedef struct {
    PQNode *head;       /* highest-priority node is always at the front */
} PriorityQueue;

void pq_init(PriorityQueue *pq);
void pq_enqueue(PriorityQueue *pq, int taskID, int priority);
int  pq_dequeue(PriorityQueue *pq);          /* returns -1 if empty */
int  pq_isEmpty(const PriorityQueue *pq);
void pq_free(PriorityQueue *pq);
```

### `priority_queue.c` — implement all functions

- `pq_init`: set `head = NULL`
- `pq_enqueue`: walk to the first node where `node->priority > new->priority`, insert before it (sorted ascending so lowest number = highest urgency is at the front)
- `pq_dequeue`: save `head->taskID`, advance `head`, free old node, return taskID
- `pq_isEmpty`: return `head == NULL`
- `pq_free`: call `pq_dequeue` until empty

### `task_graph.c` — add `markDone()`

```c
int markDone(TaskGraph *graph, Task *head, int taskID);
```

- Find task; return `-1` if not found, `0` if already DONE
- Set `task->status = STATUS_DONE`
- Walk `adjList[taskID]`, decrement `inDegree[successor]` for each neighbour
- **Do NOT remove edges** — Phase 4 Undo needs them
- Return `1` on success

### `task_graph.c` — fix `loadTasksFromFile()`

After all file lines are parsed, add this fix-up pass (prevents DONE tasks from wrongly re-blocking successors after a reload):

```c
Task *t = *head;
while (t != NULL) {
    if (t->status == STATUS_DONE) {
        AdjNode *a = graph->adjList[t->taskID];
        while (a != NULL) {
            if (graph->inDegree[a->taskID] > 0)
                graph->inDegree[a->taskID]--;
            a = a->next;
        }
    }
    t = t->next;
}
```

### `task_graph.c` — update `displayDashboard()` (optional improvement)

Replace the `sortByPriority()` call in Zone 1 with a local `PriorityQueue`:
- Enqueue each `readyIDs[i]` with its priority into a local `PriorityQueue`
- Dequeue one-by-one for display (already in correct order)
- Call `pq_free()` when done; add `#include "priority_queue.h"` at top of `task_graph.c`

### `main.c` — wire case 4

```c
case 4:
    /* show PENDING tasks, prompt ID, call markDone() */
    result = markDone(graph, *taskList, taskID);
    if (result == 1) {
        /* TODO Phase 4: stack_push(&undoStack, taskID); */
        saveTasksToFile(*taskList, graph, username);
    }
```

Add prototype in `task_graph.h`: `int markDone(TaskGraph *graph, Task *head, int taskID);`

---

## Phase 4 — Full Implementation Checklist

> **Read Phase 4 section in `Claude.md`** for full detail.

### `stack_undo.h` — define types and prototypes

```c
typedef struct StackNode {
    int taskID;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} UndoStack;

void stack_init(UndoStack *s);
void stack_push(UndoStack *s, int taskID);
int  stack_pop(UndoStack *s);          /* returns -1 if empty */
int  stack_peek(const UndoStack *s);   /* returns -1 if empty */
int  stack_isEmpty(const UndoStack *s);
void stack_free(UndoStack *s);
```

### `stack_undo.c` — implement all functions

- `stack_init`: set `top = NULL`
- `stack_push`: malloc new StackNode, set `top`
- `stack_pop`: save `top->taskID`, advance `top`, free old node, return taskID
- `stack_peek`: return `top->taskID` without removal
- `stack_isEmpty`: return `top == NULL`
- `stack_free`: pop all remaining nodes

### `main.c` — wire case 4 push + case 5 undo

In `mainMenu`, declare `UndoStack undoStack` and call `stack_init(&undoStack)`.

Case 4 — replace the `/* TODO Phase 4: stack_push */` comment with:
```c
stack_push(&undoStack, taskID);
```

Case 5 — Undo Last Action:
```c
int undoID = stack_pop(&undoStack);
if (undoID == -1) {
    printf("[INFO] Nothing to undo.\n");
} else {
    /* Revert: set STATUS_PENDING, re-increment inDegrees */
    Task *t = findTaskByID(*taskList, undoID);
    if (t != NULL) {
        t->status = STATUS_PENDING;
        AdjNode *a = graph->adjList[undoID];
        while (a != NULL) { graph->inDegree[a->taskID]++; a = a->next; }
        saveTasksToFile(*taskList, graph, username);
        printf("[SUCCESS] Task #%d restored to Pending.\n", undoID);
    }
}
```

Also add `#include "stack_undo.h"` at top of `main.c` and call `stack_free(&undoStack)` on logout.

---

## ⚠️ Implementation Notes for Phase 3 & 4

Read this before starting Phase 3 or 4 — these are non-obvious traps that will cause
bugs which are hard to spot during normal testing.

### Phase 3 — The `loadTasksFromFile` fix-up (critical)

When you implement `markDone()` in `task_graph.c`, it will work correctly during the
same session. **But after the user logs out and logs back in, the bug appears:**

`loadTasksFromFile()` rebuilds `inDegree` by reading every `DEP` line in the file.
It counts ALL edges — including edges from tasks that are already marked DONE.
This means a task that was correctly unlocked (inDegree reached 0) will have its
inDegree wrongly restored to 1+ after a reload, making it appear blocked again.

**The fix:** After `loadTasksFromFile()` finishes parsing all lines, add a fix-up pass:

```c
/* Subtract inDegree contributions from tasks that are already DONE */
Task *t = *head;
while (t != NULL) {
    if (t->status == STATUS_DONE) {
        AdjNode *a = graph->adjList[t->taskID];
        while (a != NULL) {
            if (graph->inDegree[a->taskID] > 0)
                graph->inDegree[a->taskID]--;
            a = a->next;
        }
    }
    t = t->next;
}
```

This fix is also documented in `Claude.md` under Phase 3, step 5.

### Phase 3 — Do NOT remove edges in `markDone()`

When marking a task DONE, only change `task->status` and decrement `inDegree` for
successors. **Do not remove the edges from `adjList`.** Phase 4's Undo feature relies
on those edges still being there to re-increment the inDegrees when reverting.

### Phase 4 — Where to add `stack_push`

The Phase 3 team will leave a `/* TODO Phase 4: stack_push(taskID) */` comment at the
exact line in `main.c` case 4 where the push call goes. Find that comment and replace
it with `stack_push(&undoStack, taskID)`.

### Phase 4 — Undo re-increment

When Undo reverts a DONE task back to PENDING, walk `adjList[taskID]` and
**re-increment `inDegree[successor]`** for every neighbour. The edges were never
removed, so this is just the reverse of what `markDone()` did.

---

## Contributor Rules (read before writing any code)

These are enforced across the whole project — check `Claude.md` for the full detail.

- **Language:** Pure C only — no C++, no STL, no external libraries
- **UI & comments:** English only — no Thai text in any source file
- **Input:** Use `fgets()` for all stdin reads — never `scanf()`
- **Clear screen:** Use `clearScreen()` from `main.c` — never call `system()` directly elsewhere
- **Task names:** Cannot contain the `|` character (it is the file-format delimiter)
- **Limits:** Max 50 tasks per user (`MAX_TASKS` in `task_graph.h`), max 100 users (`MAX_USERS` in `auth.h`)
- **Saving:** Call `saveTasksToFile()` immediately after any add/modify operation — do not batch

---

## How to Update This File

After completing a phase or a significant bugfix:

1. Add a new row to the **Development Timeline** table with today's date, the phase label, and a one-line description of what changed.
2. Update the **Phase Status** table icon from ⏳ to ✅ when a phase is fully done.
3. Move the completed feature from **"Not Yet Implemented"** to **"What Works Right Now"**.
4. Commit `Work_Progress.md` together with the source files in the same git commit.
