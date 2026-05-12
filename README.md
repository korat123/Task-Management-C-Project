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
2. **Hash Table (Optional/Bonus):** Used for $O(1)$ fast retrieval of users or tasks.
3. **Directed Graph (Adjacency List):** Used to map task dependencies (Prerequisites).
4. **Topological Sorting (Kahn's Algorithm):** Used to determine which tasks have an in-degree of 0 (ready to execute).
5. **Priority Queue:** Used to sort the "ready tasks" by priority (High, Medium, Low).
6. **Stack:** Used for the "Undo" feature to reverse the last task status change.

## ✨ Key Features & Scope

### 1. User Authentication System
- Basic login/register system.
- Data saved to `users.txt`.
- Each user has their own workspace/task file.

### 2. Task & Dependency Management
- **Add Task:** Create a task with an ID, Name, and Priority.
- **Set Dependency:** Link tasks (e.g., Task A must be completed before Task B).
- **Cycle Detection:** The system MUST prevent circular dependencies (e.g., A -> B -> A) when setting prerequisites using DFS/BFS.

### 3. Smart Auto-Schedule Dashboard
- A dual-zone display interface:
  - **Zone 1 (Ready to do):** Tasks with 0 prerequisites, sorted by Priority via Priority Queue.
  - **Zone 2 (Blocked):** Tasks waiting for other tasks to complete.
- Users can input a Task ID to mark it as DONE, which updates the graph and unlocks subsequent tasks.

### 4. Undo Action (Scope Limited)
- Users can press 'U' to undo the last "Mark as DONE" action.
- The Stack only stores the last completed Task ID. Reverts status to "Undone" and recalculates the graph.

### 5. Data Persistence
- Tasks and dependencies must be saved to `.txt` or `.csv` files so data persists after closing the program.

## 📂 Project Architecture (Target File Structure)
- `main.c`: UI, Menu system, and Input handling.
- `auth.h` / `auth.c`: Login and user management.
- `task_graph.h` / `task_graph.c`: Graph, Dependencies, and Topological Sort.
- `priority_queue.h` / `priority_queue.c`: Queue implementation.
- `stack_undo.h` / `stack_undo.c`: Stack implementation.