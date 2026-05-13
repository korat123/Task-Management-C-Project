# Claude.md — Project Understanding & Execution Plan

---

## Git Workflow Rules

- **DO NOT run `git commit` automatically under any circumstances.**
- When you finish writing or fixing code, you are only allowed to run `git status` and `git add .` to stage the changes.
- After staging, STOP and tell me exactly what you changed. I will manually run the `git commit` command myself.

---

## Permanent Development Standard

> **ENGLISH ONLY — This rule applies for the remainder of the project without exception.**
>
> All User Interface elements (every string passed to `printf`, every menu label,
> every prompt, every system message) and all code comments (inline, block, and
> function headers) must be written in **English**. No Thai text may appear in
> any source file going forward.

---

## Core Objective

Build a console-based **Smart Task Management System** in **Pure C** for a university
Data Structures course. The system lets users manage tasks that have complex dependencies,
auto-schedule ready tasks via topological sorting, rank them by priority through a priority
queue, and supports a one-level undo via a stack.

---

## Technical Constraints (Strict Rules)

| Rule | Detail |
|---|---|
| Language | Pure C only — no C++, no STL, no external libraries |
| Data Structures | Every structure built from scratch — no built-in dynamic lists |
| Input | Use `fgets()` everywhere for stdin — never `scanf()` |
| Cross-Platform UI | `#ifdef _WIN32` → `system("cls")` else `system("clear")` |
| File I/O | Data persistence through plain `.txt` files |
| UI & Comments | **English only** — see Permanent Development Standard above |

---

## Project Directory Layout

```
Task-Management-C-Project/
  src/          ← all .c implementation files
  include/      ← all .h header files
  docs/         ← Work_Progress.md, PDF, pdf_content.txt
  data/         ← runtime data files (users.txt, <user>_tasks.txt)
  README.md
  CLAUDE.md
```

## Build Command

```bash
gcc -Wall -Wextra -I include -o task_manager \
    src/main.c src/auth.c src/task_graph.c \
    src/priority_queue.c src/stack_undo.c src/queue.c
```

## Module Summary

| Module | Files | Core Data Structure | Phase |
|---|---|---|---|
| Cross-Platform UI + Main Menu | `src/main.c` | — | 1 |
| User Authentication | `include/auth.h`, `src/auth.c` | Array of structs + File I/O | 1 |
| Task Storage & Dependency Graph | `include/task_graph.h`, `src/task_graph.c` | Singly Linked List + Adjacency List | 2 |
| Auto-Scheduler (Topological Sort) | `include/task_graph.h`, `src/task_graph.c` | Kahn's Algorithm (BFS + in-degree array) | 2 |
| Priority Queue (Ready Tasks) | `include/priority_queue.h`, `src/priority_queue.c` | Sorted Linked List or Min-Heap | 3 |
| Undo Stack | `include/stack_undo.h`, `src/stack_undo.c` | Stack via Linked List | 4 |

---

## Step-by-Step Execution Plan

### Phase 1 — Foundation (Complete)
1. `main.c`
   - `clearScreen()` using `#ifdef _WIN32` / `#else`
   - `printBanner()` for a consistent UI header
   - `authMenu()` — entry-point loop for Login / Register
   - Stub `mainMenu()` with placeholder options for Phase 2+
   - `main()`: load users on startup, run auth loop, hand off to main menu on success

2. `auth.h`
   - `User` struct (`username[50]`, `password[50]`)
   - Constants: `MAX_USERNAME`, `MAX_PASSWORD`, `MAX_USERS`, `USERS_FILE`
   - Prototypes: `loadUsers`, `saveUser`, `registerUser`, `loginUser`

3. `auth.c`
   - `loadUsers()` — read `users.txt` line-by-line with `fgets` + `strchr`; handle missing
     file gracefully
   - `saveUser()` — append one `username:password\n` record; open with `"a"` mode
   - `registerUser()` — validate non-empty input, reject duplicates, enforce `MAX_USERS`
     cap, call `saveUser`, update in-memory array
   - `loginUser()` — linear search the array for a matching username + password pair

4. Stub files for Phases 2–4 with include guards and structured TODO comments

---

### Phase 2 — Task & Dependency Engine
1. Design `Task` struct: `taskID` (int), `name` (string), `priority` (enum),
   `status` (enum), `*next` pointer
2. Singly linked list: `addTask()`, `findTaskByID()`, `printAllTasks()`
3. `Graph` struct: adjacency list (array of linked lists keyed by task ID)
4. `addDependency(fromID, toID)`: add directed edge A → B (A must finish before B)
5. `detectCycle()` using DFS with a `visited[]` and `inStack[]` boolean array
6. `topologicalSort()` via Kahn's Algorithm — build in-degree array, enqueue zero-
   in-degree nodes, process layer by layer
7. File I/O: save/load tasks and edges per user (`<username>_tasks.txt`)

---

### Phase 3 — Priority Queue & Mark Done
1. Define `PQNode` struct (taskID, priority, \*next) and `PriorityQueue` struct (head pointer)
   in `priority_queue.h`
2. Implement in `priority_queue.c`:
   - `pq_enqueue()` — sorted insert (walk to first node with higher priority number, insert before it)
   - `pq_dequeue()` — remove and return head's taskID; return -1 if empty
   - `pq_isEmpty()` — return 1 if head == NULL
   - `pq_free()` — walk and free all nodes
3. Update `displayDashboard()` in `task_graph.c`:
   - Remove the old static `sortByPriority()` insertion-sort helper
   - Replace it with a local `PriorityQueue`: enqueue every in-degree-0 PENDING task,
     then dequeue one-by-one to display Zone 1 in correct priority order
   - `#include "priority_queue.h"` must be added at the top of `task_graph.c`
4. Add `markDone(taskID)` in `task_graph.c`:
   - Find the task; return -1 if not found, 0 if already DONE
   - Set `task->status = STATUS_DONE`
   - Walk `adjList[taskID]` and decrement `inDegree[successor]` for each neighbour
   - **Do NOT remove edges from the adjacency list** — Phase 4 Undo needs them to
     re-increment inDegrees when reverting
5. **⚠️ Critical fix — `loadTasksFromFile()` in `task_graph.c`:**
   When the file is reloaded, `DEP` lines rebuild `inDegree` from ALL edges — including
   edges whose source task is already DONE. Without a fix-up, tasks that were correctly
   unlocked by `markDone()` will appear wrongly blocked again after the next login.
   **After the full file is parsed, add a fix-up pass:**
   ```
   for every task in the list:
       if task->status == STATUS_DONE:
           for every neighbour in adjList[task->taskID]:
               if inDegree[neighbour] > 0: inDegree[neighbour]--
   ```
6. Wire up menu option `[4]` in `main.c`: show PENDING tasks → prompt Task ID (validate
   non-empty) → call `markDone()` → save → show refreshed dashboard
7. Leave a `/* TODO Phase 4: stack_push(taskID) */` comment at the success point inside
   case 4 of `mainMenu()` so the Phase 4 team knows the exact line to add the push call

---

### Phase 4 — Undo Stack & Final Integration
1. Define `StackNode` struct (taskID, \*next) and `UndoStack` struct (top pointer)
   in `stack_undo.h`
2. Implement in `stack_undo.c`:
   - `stack_push()` — malloc new node, set top
   - `stack_pop()` — save top's taskID, advance top, free old node, return taskID
   - `stack_peek()` — return top->taskID without removing; -1 if empty
   - `stack_isEmpty()` — return 1 if top == NULL
   - `stack_free()` — pop all remaining nodes (call on logout/exit)
3. In `main.c` case 4: replace the `/* TODO Phase 4: stack_push(taskID) */` comment
   with the actual `stack_push(&undoStack, taskID)` call
4. Wire up menu option `[5]` in `main.c`:
   - Call `stack_pop()` to get last completed taskID
   - Revert: set `task->status = STATUS_PENDING`
   - Re-increment `inDegree[successor]` for every neighbour in `adjList[taskID]`
     (the edges were never removed, so this is a simple walk)
   - Save and refresh dashboard
5. Full integration test: register → login → add tasks → set dependencies →
   mark done → verify dashboard updates → undo → verify task returns to Zone 1 →
   logout and back in → verify state persists correctly

---

## Key Design Decisions

- **`users.txt` format**: `username:password` one entry per line (plaintext — acceptable
  for a university course; would require hashing in production)
- **Per-user task file**: `<username>_tasks.txt` (created in Phase 2)
- **Max users**: 100 (`MAX_USERS` compile-time constant in `auth.h`)
- **Priority encoding**: High = 1, Medium = 2, Low = 3 (lower number = higher priority)
- **Cycle detection**: DFS runs before every `addDependency` call to prevent circular deps
- **inDegree tracking**: `graph->inDegree[i]` must always reflect only PENDING predecessors.
  `markDone()` decrements it live. `loadTasksFromFile()` rebuilds it from raw `DEP` lines
  and then must run a fix-up pass to subtract DONE tasks' contributions — otherwise
  previously-unlocked tasks appear blocked again after every login.
