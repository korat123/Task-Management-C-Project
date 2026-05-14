# Smart Task Management System

## 📌 Project Overview
This is a C-language, console-based Smart Task Management System developed as a university group project for a Data Structures course. The system helps users plan, manage, and auto-schedule complex tasks using advanced data structures. 

## 🛠️ Technical Constraints (Strict Rules for AI)
- **Language:** Pure C (No C++ STL or external libraries).
- **Data Structures:** MUST be implemented from scratch (do not use built-in arrays for dynamic lists).
- **Cross-Platform UI:** The terminal UI must use a cross-platform clear screen function (`#ifdef _WIN32 system("cls")` else `system("clear")`).
- **Input Handling:** Use `fgets` instead of `scanf` to prevent infinite loops and buffer issues.

## 🧠 Core Data Structures Used
1. **Singly Linked List:** Used to store all task objects dynamically.
2. **Directed Graph (Adjacency List):** Used to map task dependencies (prerequisites).
3. **FIFO Queue (Linked List):** Used by Kahn's BFS algorithm for topological sorting.
4. **Topological Sorting (Kahn's Algorithm):** Determines which tasks have in-degree 0 (ready to execute) and the full suggested completion sequence.
5. **Priority Queue (Sorted Linked List):** Ranks ready tasks by urgency — High before Medium before Low.
6. **Stack (LIFO Linked List):** Powers the Undo feature — records each Mark Done action and reverts the most recent one.

## ✨ Key Features & Scope

### 1. User Authentication System
- Basic login/register system.
- Credentials saved to `data/users.txt` (auto-created at first launch).
- Each user has their own task file at `data/<username>_tasks.txt`.

### 2. Task & Dependency Management
- **Add Task `[2]`:** Create a task with an auto-assigned ID, Name, and Priority (High / Medium / Low).
- **Set Dependency `[3]`:** Link tasks so Task A must be completed before Task B can start.
- **Cycle Detection:** DFS runs before every dependency is accepted — circular chains (A → B → A) are automatically rejected.
- **Delete Task `[6]`:** Removes a task and repairs all in-degree counts for its successors.
- **Search Task by Name `[7]`:** Case-insensitive substring search across all task names.

### 3. Smart Auto-Schedule Dashboard `[1]`
- A dual-zone display interface:
  - **Zone 1 (Ready):** PENDING tasks with no remaining blockers (in-degree = 0), sorted High → Medium → Low by the Priority Queue.
  - **Zone 2 (Blocked):** PENDING tasks still waiting on predecessor tasks, showing which task ID is blocking each.
- **Mark Task as Done `[4]`:** Flips a task to DONE, decrements successors' in-degrees, and instantly refreshes the dashboard.
- **View Topological Execution Order `[8]`:** Kahn's BFS algorithm prints the full suggested completion sequence for all pending tasks.

### 4. Undo Last Action `[5]`
- Select option `[5]` from the menu to undo the most recent Mark as Done.
- The LIFO stack records every completion in the current session; each Undo pops one entry, reverts the task to Pending, and re-increments its successors' in-degrees.
- Undo history is in-memory only — it resets on logout.

### 5. Data Persistence
- All tasks and dependencies are saved to `data/<username>_tasks.txt` immediately after every add, modify, or delete.
- A fix-up pass on load ensures tasks correctly unlocked by Mark Done are not re-blocked after a restart.

## 📂 Project Structure

```
Task-Management-C-Project/
  src/                    ← C implementation files
    main.c                  UI controller, menus, session lifecycle
    auth.c                  Login, register, data/users.txt I/O
    task_graph.c            Task list, graph, DFS, dashboard, delete, search, topo sort, file I/O
    queue.c                 FIFO Queue (used by Kahn's BFS)
    priority_queue.c        Sorted Priority Queue for ranking ready tasks
    stack_undo.c            LIFO Undo Stack
    hash_table.c            Hash Table for tag-based task lookup
  include/                ← Header files (one per module)
    auth.h, task_graph.h, queue.h, priority_queue.h, stack_undo.h, hash_table.h
  data/                   ← Runtime data (auto-created at startup)
    users.txt
    <username>_tasks.txt
  docs/                   ← Documentation
    Work_Progress.md
  README.md
  CLAUDE.md
```

## 🔨 Build Command

```bash
gcc -Wall -Wextra -I include -o task_manager \
    src/main.c src/auth.c src/task_graph.c \
    src/priority_queue.c src/stack_undo.c src/queue.c src/hash_table.c
```