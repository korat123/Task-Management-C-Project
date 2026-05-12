# Claude.md — Project Understanding & Execution Plan

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

## Module Summary

| Module | Files | Core Data Structure | Phase |
|---|---|---|---|
| Cross-Platform UI + Main Menu | `main.c` | — | 1 |
| User Authentication | `auth.h`, `auth.c` | Array of structs + File I/O | 1 |
| Task Storage & Dependency Graph | `task_graph.h`, `task_graph.c` | Singly Linked List + Adjacency List | 2 |
| Auto-Scheduler (Topological Sort) | `task_graph.h`, `task_graph.c` | Kahn's Algorithm (BFS + in-degree array) | 2 |
| Priority Queue (Ready Tasks) | `priority_queue.h`, `priority_queue.c` | Sorted Linked List or Min-Heap | 3 |
| Undo Stack | `stack_undo.h`, `stack_undo.c` | Stack via Linked List | 4 |

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

### Phase 3 — Dashboard & Priority Queue
1. Priority Queue implemented as a sorted linked list (High=1 < Medium=2 < Low=3)
2. `pq_enqueue()` inserts in sorted order; `pq_dequeue()` removes the front node
3. Dashboard Zone 1: topological sort → push in-degree-0 tasks into the queue →
   display in priority order
4. Dashboard Zone 2: display all tasks still blocked (in-degree > 0)
5. `markDone(taskID)`: set status DONE, remove outgoing edges, decrement in-degrees,
   push action to undo stack, refresh dashboard

---

### Phase 4 — Undo Stack & Final Integration
1. Stack implemented as a singly linked list: `stack_push()`, `stack_pop()`,
   `stack_peek()`, `stack_isEmpty()`
2. On `markDone`: push the completed task ID onto the stack
3. On Undo: pop task ID, revert its status to PENDING, restore graph edges,
   recalculate in-degrees, refresh dashboard
4. Full integration test: register → login → add tasks → set dependencies →
   view dashboard → mark done → undo → verify persistence after restart

---

## Key Design Decisions

- **`users.txt` format**: `username:password` one entry per line (plaintext — acceptable
  for a university course; would require hashing in production)
- **Per-user task file**: `<username>_tasks.txt` (created in Phase 2)
- **Max users**: 100 (`MAX_USERS` compile-time constant in `auth.h`)
- **Priority encoding**: High = 1, Medium = 2, Low = 3 (lower number = higher priority)
- **Cycle detection**: DFS runs before every `addDependency` call to prevent circular deps
