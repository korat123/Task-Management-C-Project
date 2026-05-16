#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
  #include <direct.h>   /* _mkdir */
#else
  #include <sys/stat.h> /* mkdir  */
#endif
#include "auth.h"
#include "task_graph.h"
#include "stack_undo.h"
#include "hash_table.h"

/*
 * main.c  —  Program entry point, terminal UI, and top-level menu controller.
 *
 * Responsibilities:
 *   - Bootstrap the application (load users, init graph) at startup.
 *   - Provide a cross-platform clear-screen utility used by every screen.
 *   - Drive the authentication menu until the user logs in or exits.
 *   - Load the logged-in user's task workspace, hand control to mainMenu(),
 *     and clean up the workspace on logout.
 *
 * Architecture:
 *   This file owns UI and control flow only.
 *   Auth logic     → auth.c
 *   Task / graph   → task_graph.c
 *   FIFO queue     → queue.c           (used by Kahn's BFS)
 *   Priority queue → priority_queue.c  (used by Dashboard Zone 1)
 *   Undo stack     → stack_undo.c      (one-level Undo for Mark Done)
 */


/* ─────────────────────────────────────────────────────────────────────────
 * clearScreen
 *
 * Purpose:
 *   Clears the terminal between screens. Wraps the OS-specific command so
 *   no other function needs a platform branch.
 *
 * Parameters  : None.
 * Return Value: None.
 * ───────────────────────────────────────────────────────────────────────── */
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


/* ─────────────────────────────────────────────────────────────────────────
 * printBanner
 *
 * Purpose:
 *   Renders the application title at the top of every screen. Always called
 *   immediately after clearScreen() to give each page a consistent header.
 *
 * Parameters  : None.
 * Return Value: None.
 * ───────────────────────────────────────────────────────────────────────── */
void printBanner(void) {
    printf("============================================\n");
    printf("     Smart Task Management System v1.0     \n");
    printf("         Data Structures Project            \n");
    printf("============================================\n\n");
}


/* ─────────────────────────────────────────────────────────────────────────
 * authMenu
 *
 * Purpose:
 *   Presents the pre-login screen (Login / Register / Exit) and loops until
 *   the user either authenticates successfully or chooses to exit.
 *
 * Parameters:
 *   users         — In-memory user array forwarded to auth functions.
 *   userCount     — Pointer to the current user count; may grow via Register.
 *   loggedInUser  — Output buffer populated by loginUser() on success.
 *
 * Return Value:
 *   1 — Login succeeded; caller should proceed to mainMenu().
 *   0 — User selected Exit.
 * ───────────────────────────────────────────────────────────────────────── */
int authMenu(User users[], int *userCount, char *loggedInUser) {
    char menuInput[16];
    int  choice;

    while (1) {
        clearScreen();
        printBanner();

        printf(" --- Main Menu ---\n");
        printf("  [1]  Login\n");
        printf("  [2]  Register\n");
        printf("  [0]  Exit\n");
        printf(" -----------------\n");
        printf(" Enter your choice: ");

        fgets(menuInput, sizeof(menuInput), stdin);
        menuInput[strcspn(menuInput, "\n")] = '\0';
        if (strlen(menuInput) == 0) continue; /* Bare Enter — redraw menu */
        {
            char *endptr;
            choice = (int)strtol(menuInput, &endptr, 10);
            if (endptr == menuInput || *endptr != '\0') {
                printf("\n[ERROR] Invalid option. Please enter 0, 1, or 2.\n");
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                continue;
            }
        }

        switch (choice) {
            case 1:
                if (loginUser(users, *userCount, loggedInUser) == 1)
                    return 1;
                break;

            case 2:
                /* User must log in separately after registering. */
                registerUser(users, userCount);
                break;

            case 0:
                return 0;

            default:
                printf("\n[ERROR] Invalid option. Please enter 0, 1, or 2.\n");
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;
        }
    }
}


/* ─────────────────────────────────────────────────────────────────────────
 * mainMenu
 *
 * Purpose:
 *   The task-management hub for an authenticated session. Implements the
 *   three Phase 2 features (Dashboard, Add Task, Set Dependency) and holds
 *   stubs for Phases 3 and 4. Saves data to disk and returns to the auth
 *   loop when the user logs out.
 *
 * Parameters:
 *   username  — Display name of the active session; also used as the
 *               file prefix for the per-user task file.
 *   taskList  — Pointer to the task linked-list head pointer. The list may
 *               grow as tasks are added.
 *   graph     — The user's dependency graph (updated by add/dependency ops).
 *
 * Return Value: None (returns to the authMenu loop in main() on logout).
 * ───────────────────────────────────────────────────────────────────────── */
void mainMenu(const char *username, Task **taskList, TaskGraph *graph) {
    char      menuInput[16];
    char      nameBuffer[MAX_TASK_NAME];
    int       choice;
    int       priorityChoice;
    int       fromID;
    int       toID;
    int       result;
    Task     *task;
    UndoStack undoStack;
    /* nameBuffer is reused for keyword input in Search (case 7).
     * undoStack lives for the duration of this session only; it is
     * freed in case 0 (logout) and is not persisted across logins. */

    stack_init(&undoStack);

    while (1) {
        clearScreen();
        printBanner();

        printf(" Logged in as : [%s]   |   Tasks: %d / %d\n\n",
               username, graph->taskCount, MAX_TASKS);
        printf(" --- Task Management Menu ---\n");
        printf("  [1]  View Auto-Schedule Dashboard\n");
        printf("  [2]  Add New Task\n");
        printf("  [3]  Set Task Dependency\n");
        printf("  [4]  Mark Task as Done\n");
        printf("  [5]  Undo Last Action\n");
        printf("  [6]  Delete Task\n");
        printf("  [7]  Search Task by Name\n");
        printf("  [8]  View Topological Execution Order\n");
        printf("  [9]  View Tasks by Tag\n");
        printf("  [0]  Logout\n");
        printf(" ----------------------------\n");
        printf(" Enter your choice: ");

        fgets(menuInput, sizeof(menuInput), stdin);
        menuInput[strcspn(menuInput, "\n")] = '\0';
        if (strlen(menuInput) == 0) continue; /* Bare Enter — redraw menu */
        {
            char *endptr;
            choice = (int)strtol(menuInput, &endptr, 10);
            if (endptr == menuInput || *endptr != '\0') {
                printf("\n[ERROR] Invalid option. Please enter a number from 0 to 9.\n");
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                continue;
            }
        }

        switch (choice) {

            /* ── [0] Logout ─────────────────────────────────────────────── */
            case 0:
                saveTasksToFile(*taskList, graph, username);
                clear_hash_table();
                stack_free(&undoStack);  /* In-memory undo history ends here. */
                printf("\n[SYSTEM] Data saved. Goodbye, %s!\n", username);
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                return;

            /* ── [1] Auto-Schedule Dashboard ────────────────────────────── */
            case 1:
                clearScreen();
                printBanner();
                printf(" Auto-Schedule Dashboard  |  User: [%s]  |  Tasks: %d\n",
                       username, graph->taskCount);
                printf(" ============================================================\n");
                displayDashboard(*taskList, graph);
                printf("\nPress Enter to return to the menu...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            /* ── [2] Add New Task ─────────────────────────────────────────  */
            case 2:
                clearScreen();
                printBanner();
                printf(" --- Add New Task ---\n\n");

                printf(" Task Name : ");
                fgets(nameBuffer, sizeof(nameBuffer), stdin);
                nameBuffer[strcspn(nameBuffer, "\n")] = '\0';

                if (strlen(nameBuffer) == 0) {
                    printf("\n[ERROR] Task name cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                /* The '|' character is used as the file format delimiter,
                 * so it cannot appear inside a task name. */
                if (strchr(nameBuffer, '|') != NULL) {
                    printf("\n[ERROR] Task name cannot contain the '|' character.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                printf("\n Priority:\n");
                printf("   [1]  High\n");
                printf("   [2]  Medium\n");
                printf("   [3]  Low\n");
                printf(" Select: ");
                fgets(menuInput, sizeof(menuInput), stdin);
                menuInput[strcspn(menuInput, "\n")] = '\0';
                priorityChoice = atoi(menuInput);

                if (priorityChoice < 1 || priorityChoice > 3) {
                    printf("\n[ERROR] Invalid priority. Please enter 1, 2, or 3.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                char tagBuffer[30];
                printf(" Task Tag (e.g., Study, Work, None) : ");
                fgets(tagBuffer, sizeof(tagBuffer), stdin);
                tagBuffer[strcspn(tagBuffer, "\n")] = '\0';

                // If they just press Enter, default to "None"
                if (strlen(tagBuffer) == 0) {
                    strcpy(tagBuffer, "None");
                }

                result = addTask(taskList, graph,
                                 nameBuffer, (TaskPriority)priorityChoice, tagBuffer);

                if (result > 0) {
                    saveTasksToFile(*taskList, graph, username);
                    printf("\n[SUCCESS] Task '%s' created with ID #%d.\n",
                           nameBuffer, result);
                } else {
                    printf("\n[ERROR] Cannot add task. "
                           "Maximum limit of %d tasks reached.\n", MAX_TASKS);
                }
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            /* ── [3] Set Task Dependency ──────────────────────────────────  */
            case 3:
                clearScreen();
                printBanner();
                printf(" --- Set Task Dependency ---\n\n");

                if (graph->taskCount < 2) {
                    printf("[INFO] You need at least 2 tasks to set a dependency.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                /* Display the full task list for reference before asking for IDs. */
                printf(" Current Tasks:\n");
                printf("  %-4s  %-26s  %-8s  %s\n",
                       "ID", "Task Name", "Priority", "Status");
                printf("  %-4s  %-26s  %-8s  %s\n",
                       "----", "--------------------------", "--------", "-------");
                task = *taskList;
                while (task != NULL) {
                    printf("  %-4d  %-26.26s  %-8s  %s\n",
                           task->taskID, task->name,
                           priorityToString(task->priority),
                           statusToString(task->status));
                    task = task->next;
                }

                printf("\n Rule: Task A must be fully completed BEFORE Task B can start.\n\n");

                printf(" Task A (prerequisite) ID : ");
                fgets(menuInput, sizeof(menuInput), stdin);
                menuInput[strcspn(menuInput, "\n")] = '\0';
                if (strlen(menuInput) == 0) {
                    printf("\n[ERROR] Task ID cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }
                fromID = atoi(menuInput);

                printf(" Task B (dependent)    ID : ");
                fgets(menuInput, sizeof(menuInput), stdin);
                menuInput[strcspn(menuInput, "\n")] = '\0';
                if (strlen(menuInput) == 0) {
                    printf("\n[ERROR] Task ID cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }
                toID = atoi(menuInput);

                result = addDependency(graph, *taskList, fromID, toID);

                switch (result) {
                    case  1:
                        saveTasksToFile(*taskList, graph, username);
                        printf("\n[SUCCESS] Dependency set: "
                               "Task #%d must be completed before Task #%d.\n",
                               fromID, toID);
                        break;
                    case  0:
                        printf("\n[ERROR] Rejected: adding this dependency would create "
                               "a circular chain (e.g., A -> B -> ... -> A).\n");
                        break;
                    case -1:
                        printf("\n[ERROR] Task #%d does not exist.\n", fromID);
                        break;
                    case -2:
                        printf("\n[ERROR] Task #%d does not exist.\n", toID);
                        break;
                    case -3:
                        printf("\n[ERROR] A task cannot be set as its own prerequisite.\n");
                        break;
                    case -4:
                        printf("\n[INFO] This dependency already exists.\n");
                        break;
                    default:
                        printf("\n[ERROR] Failed to set dependency (code: %d).\n", result);
                        break;
                }
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            /* ── [4] Mark Task as Done ───────────────────────────────────  */
            case 4: {
                int pendingCount = 0;
                int targetID;

                clearScreen();
                printBanner();
                printf(" --- Mark Task as Done ---\n\n");

                /* Pre-check: count PENDING tasks. */
                for (task = *taskList; task != NULL; task = task->next) {
                    if (task->status == STATUS_PENDING) pendingCount++;
                }
                if (pendingCount == 0) {
                    printf("[INFO] There are no pending tasks to mark as done.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                /* List PENDING tasks so the user can pick one. */
                printf(" Pending Tasks:\n");
                printf("  %-4s  %-26s  %-8s\n",
                       "ID", "Task Name", "Priority");
                printf("  %-4s  %-26s  %-8s\n",
                       "----", "--------------------------", "--------");
                for (task = *taskList; task != NULL; task = task->next) {
                    if (task->status == STATUS_PENDING) {
                        printf("  %-4d  %-26.26s  %-8s\n",
                               task->taskID, task->name,
                               priorityToString(task->priority));
                    }
                }

                printf("\n Task ID to mark as done: ");
                fgets(menuInput, sizeof(menuInput), stdin);
                menuInput[strcspn(menuInput, "\n")] = '\0';
                if (strlen(menuInput) == 0) {
                    printf("\n[ERROR] Task ID cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }
                targetID = atoi(menuInput);

                result = markDone(*taskList, graph, targetID);
                if (result == -1) {
                    printf("\n[ERROR] Task #%d not found.\n", targetID);
                } else if (result == 0) {
                    printf("\n[INFO] Task #%d is already marked as Done.\n",
                           targetID);
                } else {
                    saveTasksToFile(*taskList, graph, username);
                    stack_push(&undoStack, targetID);
                    printf("\n[SUCCESS] Task #%d marked as Done.\n", targetID);
                    displayDashboard(*taskList, graph);
                }
                printf("\nPress Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;
            }

            /* ── [5] Undo Last Action ────────────────────────────────────  */
            case 5: {
                int lastID;
                int undoResult;

                clearScreen();
                printBanner();
                printf(" --- Undo Last Action ---\n\n");

                if (stack_isEmpty(&undoStack)) {
                    printf("[INFO] Nothing to undo in this session.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                lastID     = stack_pop(&undoStack);
                undoResult = undoMarkDone(*taskList, graph, lastID);

                if (undoResult == -1) {
                    printf("[INFO] Task #%d was deleted; nothing to undo.\n",
                           lastID);
                } else if (undoResult == 0) {
                    printf("[INFO] Task #%d is already pending.\n", lastID);
                } else {
                    saveTasksToFile(*taskList, graph, username);
                    printf("[SUCCESS] Task #%d reverted to Pending.\n", lastID);
                    displayDashboard(*taskList, graph);
                }
                printf("\nPress Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;
            }

            /* ── [6] Delete Task ──────────────────────────────────────────  */
            case 6:
                clearScreen();
                printBanner();
                printf(" --- Delete Task ---\n\n");

                if (graph->taskCount == 0) {
                    printf("[INFO] No tasks to delete.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                printf(" Current Tasks:\n");
                printf("  %-4s  %-26s  %-8s  %s\n",
                       "ID", "Task Name", "Priority", "Status");
                printf("  %-4s  %-26s  %-8s  %s\n",
                       "----", "--------------------------", "--------", "-------");
                task = *taskList;
                while (task != NULL) {
                    printf("  %-4d  %-26.26s  %-8s  %s\n",
                           task->taskID, task->name,
                           priorityToString(task->priority),
                           statusToString(task->status));
                    task = task->next;
                }

                printf("\n Task ID to delete: ");
                fgets(menuInput, sizeof(menuInput), stdin);
                menuInput[strcspn(menuInput, "\n")] = '\0';
                if (strlen(menuInput) == 0) {
                    printf("\n[ERROR] Task ID cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }
                fromID = atoi(menuInput);
                result = deleteTask(taskList, graph, fromID);
                if (result == 1) {
                    saveTasksToFile(*taskList, graph, username);
                    printf("\n[SUCCESS] Task #%d has been deleted.\n", fromID);
                } else {
                    printf("\n[ERROR] Task #%d not found.\n", fromID);
                }
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            /* ── [7] Search Task by Name ──────────────────────────────────  */
            case 7:
                clearScreen();
                printBanner();
                printf(" --- Search Task by Name ---\n\n");

                if (*taskList == NULL) {
                    printf("[INFO] No tasks exist yet.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                printf(" Keyword: ");
                fgets(nameBuffer, sizeof(nameBuffer), stdin);
                nameBuffer[strcspn(nameBuffer, "\n")] = '\0';
                if (strlen(nameBuffer) == 0) {
                    printf("\n[ERROR] Keyword cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }
                searchTaskByName(*taskList, nameBuffer);
                printf("\nPress Enter to return to the menu...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            /* ── [8] View Topological Execution Order ─────────────────────  */
            case 8:
                clearScreen();
                printBanner();
                printf(" Topological Execution Order  |  User: [%s]  |  Tasks: %d\n",
                       username, graph->taskCount);
                printf(" ============================================================\n");
                topologicalSortDisplay(*taskList, graph);
                printf("\nPress Enter to return to the menu...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            /* ── [9] View Tasks by Tag ────────────────────────────────────  */
            case 9:
                clearScreen();
                printBanner();
                printf(" --- View Tasks by Tag ---\n\n");

                if (graph->taskCount == 0) {
                    printf("[INFO] No tasks exist yet.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                printf(" Enter tag to search: ");
                fgets(nameBuffer, sizeof(nameBuffer), stdin);
                nameBuffer[strcspn(nameBuffer, "\n")] = '\0';

                if (strlen(nameBuffer) == 0) {
                    printf("\n[ERROR] Tag cannot be empty.\n");
                    printf("Press Enter to continue...");
                    fgets(menuInput, sizeof(menuInput), stdin);
                    break;
                }

                /* Call the Hash Table function */
                search_by_tag(nameBuffer);

                printf("\nPress Enter to return to the menu...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;

            default:
                printf("\n[ERROR] Invalid option. Please enter a number from 0 to 9.\n");
                printf("Press Enter to continue...");
                fgets(menuInput, sizeof(menuInput), stdin);
                break;
        }
    }
}


/* ─────────────────────────────────────────────────────────────────────────
 * main
 *
 * Purpose:
 *   Entry point. Initialises all subsystems, then drives the outer
 *   auth → task-menu → auth loop until the user exits.
 *   On each successful login, the previous user's workspace is freed,
 *   the new user's data is loaded from disk, and mainMenu() is entered.
 *
 * Parameters  : None.
 * Return Value: 0 on clean exit.
 * ───────────────────────────────────────────────────────────────────────── */
int main(void) {
    User      users[MAX_USERS];
    int       userCount = 0;
    char      loggedInUser[MAX_USERNAME];
    int       authResult;
    Task     *taskList = NULL;
    TaskGraph graph;

    /* Create the data directory if it does not exist yet. */
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0755);
#endif

    /* Initialise the graph struct before any graph operations are called. */
    initGraph(&graph);

    /* Load persisted user accounts into RAM once at startup. */
    loadUsers(users, &userCount);

    printf("Press Enter to start...");
    getchar();

    /* Outer program loop: auth screen → task menu → back to auth on logout. */
    while (1) {
        authResult = authMenu(users, &userCount, loggedInUser);

        if (authResult == 1) {
            /* Free any workspace belonging to the previous session,
             * then load the newly authenticated user's data. */
            freeTaskList(&taskList);
            freeGraph(&graph);
            loadTasksFromFile(&taskList, &graph, loggedInUser);

            mainMenu(loggedInUser, &taskList, &graph);
            /* mainMenu() returns when the user logs out. Loop re-enters authMenu. */

        } else {
            /* User chose Exit from the auth screen. */
            clearScreen();
            printBanner();
            printf(" Thank you for using Smart Task Management System.\n");
            printf(" Goodbye!\n\n");
            freeTaskList(&taskList);
            freeGraph(&graph);
            break;
        }
    }

    return 0;
}
