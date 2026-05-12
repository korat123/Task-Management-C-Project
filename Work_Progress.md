# Work Progress — Smart Task Management System

**Project:** Smart Task Management System &nbsp;|&nbsp; **Phases Done:** 2 / 4 &nbsp;|&nbsp; **Last Updated:** 2026-05-13

---

## How to Build & Run

Any teammate can clone the repo and be running the project in one command:

```bash
# Compile (run from the project root folder)
gcc -Wall -o task_mgmt main.c auth.c task_graph.c

# Run on Windows
task_mgmt.exe

# Run on Linux / macOS
./task_mgmt
```

> **Note:** `priority_queue.c` and `stack_undo.c` are not included in the compile
> command yet — they are stubs and will be added in Phase 3 and 4.

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

---

## Phase Status

| Phase | Name | Key Deliverables | Status |
|-------|------|-----------------|--------|
| 1 | Foundation & Authentication | `clearScreen`, `authMenu`, `registerUser`, `loginUser`, `users.txt` persistence | ✅ Done |
| 2 | Task & Dependency Graph | Task linked list, adjacency list graph, DFS cycle check, auto-schedule dashboard, `<user>_tasks.txt` | ✅ Done |
| 3 | Priority Queue & Mark Done | `PriorityQueue` struct, `pq_enqueue`/`pq_dequeue`, `markDone()`, Zone 1 sorted by queue | ⏳ Next |
| 4 | Undo Stack & Full Integration | `UndoStack` struct, `stack_push`/`stack_pop`, Undo reverts last Done, end-to-end test | ⏳ Planned |

---

## File Map

Quick reference — who owns what, and which phase it belongs to.

| File(s) | Phase | Responsibility |
|---------|-------|---------------|
| `main.c` | 1 – 4 | UI controller: all menus, input handling, session lifecycle |
| `auth.h` / `auth.c` | 1 | User registration, login, `users.txt` read/write |
| `task_graph.h` / `task_graph.c` | 2 | Task linked list, directed dependency graph, DFS, dashboard display, per-user file I/O |
| `priority_queue.h` / `priority_queue.c` | 3 | Sorted linked-list priority queue for ranking ready tasks |
| `stack_undo.h` / `stack_undo.c` | 4 | LIFO stack that stores completed task IDs for Undo |
| `README.md` | — | Project specification and technical constraints (do not edit) |
| `Claude.md` | — | AI execution plan and permanent development standards |
| `Work_Progress.md` | — | **This file** — team timeline and status tracker |
| `.gitignore` | — | Excludes `*.exe`, `*.o`, `users.txt`, `*_tasks.txt` from git |

---

## What Works Right Now

Teammates can run the program today and test all of these:

- **Register** a new account — credentials saved to `users.txt`
- **Login** with username and password — incorrect credentials are rejected
- **Add tasks** with a name and priority (High / Medium / Low) — auto-assigns an ID
- **Set dependencies** between tasks — e.g., "Task #1 must be done before Task #2"
- **Circular dependency protection** — the system rejects any chain that would loop back on itself
- **Auto-Schedule Dashboard** — Zone 1 shows tasks ready to work on (sorted by priority); Zone 2 shows tasks still blocked and which task ID is blocking them
- **Data persistence** — tasks and dependencies survive logout and program restart

---

## What Is Not Yet Implemented

These menu options show a "coming soon" message and will be built in Phases 3–4:

| Menu Option | Feature | Phase |
|-------------|---------|-------|
| `[4] Mark Task as Done` | Changes a task to DONE, updates the graph, unlocks dependent tasks | Phase 3 |
| `[5] Undo Last Action` | Reverts the most recent Mark-as-Done | Phase 4 |

The Priority Queue in `priority_queue.c` is also a stub. Zone 1 of the dashboard currently uses a simple insertion sort as a placeholder — Phase 3 will replace this with the proper priority queue.

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
