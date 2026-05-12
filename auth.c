#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"

/*
 * auth.c  —  Implementation of the User Authentication module.
 *
 * Credential storage format (users.txt):
 *   One entry per line:  username:password\n
 *
 * Design notes:
 *   - Passwords are stored as plaintext. This is acceptable for a university
 *     course project but must never be used in a production environment.
 *   - All records are loaded into an in-memory array at startup so that
 *     authentication lookups do not require repeated file I/O.
 *   - fgets() is used exclusively for all stdin reads to avoid the buffer-
 *     overflow and stuck-input risks associated with scanf().
 */


/* ─────────────────────────────────────────────────────────────────────────
 * loadUsers
 *
 * Purpose:
 *   Reads every user record from USERS_FILE into the caller-supplied array.
 *   Called once at program startup. After this call, the array in RAM is
 *   the authoritative source for all credential checks.
 *
 * Parameters:
 *   users  — Output array of User structs to populate.
 *   count  — Output pointer set to the number of records successfully loaded.
 *
 * Return Value:
 *   1 — File opened and parsed successfully (even if it held zero entries).
 *   0 — File could not be opened; normal on the very first run before any
 *       user has registered. count is set to 0.
 * ───────────────────────────────────────────────────────────────────────── */
int loadUsers(User users[], int *count) {
    char lineBuffer[MAX_USERNAME + MAX_PASSWORD + 5];
    char *separatorPos;

    FILE *file = fopen(USERS_FILE, "r");
    if (file == NULL) {
        printf("[SYSTEM] Credential file '%s' not found. "
               "It will be created on first registration.\n", USERS_FILE);
        *count = 0;
        return 0;
    }

    *count = 0;

    while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        /* Strip the trailing newline that fgets retains. */
        lineBuffer[strcspn(lineBuffer, "\n")] = '\0';

        if (strlen(lineBuffer) == 0) continue;

        /* Find the ':' delimiter that separates username from password. */
        separatorPos = strchr(lineBuffer, ':');
        if (separatorPos == NULL) continue; /* Malformed line — skip silently. */

        /* Replace ':' with '\0' to split the buffer into two null-terminated
         * strings in place: lineBuffer → username, separatorPos+1 → password. */
        *separatorPos = '\0';

        strncpy(users[*count].username, lineBuffer, MAX_USERNAME - 1);
        users[*count].username[MAX_USERNAME - 1] = '\0';

        strncpy(users[*count].password, separatorPos + 1, MAX_PASSWORD - 1);
        users[*count].password[MAX_PASSWORD - 1] = '\0';

        (*count)++;
        if (*count >= MAX_USERS) break;
    }

    fclose(file);
    printf("[SYSTEM] Loaded %d user record(s) from '%s'.\n", *count, USERS_FILE);
    return 1;
}


/* ─────────────────────────────────────────────────────────────────────────
 * saveUser
 *
 * Purpose:
 *   Appends one user record to USERS_FILE so credentials survive restarts.
 *   Uses append mode ("a") so existing records are never overwritten.
 *   If the file does not yet exist, fopen will create it automatically.
 *
 * Parameters:
 *   user — Read-only pointer to the User struct to persist.
 *
 * Return Value:
 *   1 — Record written and file closed successfully.
 *   0 — fopen failed (e.g., insufficient folder permissions).
 * ───────────────────────────────────────────────────────────────────────── */
int saveUser(const User *user) {
    FILE *file = fopen(USERS_FILE, "a");
    if (file == NULL) {
        printf("[ERROR] Cannot open '%s' for writing. "
               "Check folder permissions.\n", USERS_FILE);
        return 0;
    }

    fprintf(file, "%s:%s\n", user->username, user->password);
    fclose(file);
    return 1;
}


/* ─────────────────────────────────────────────────────────────────────────
 * registerUser
 *
 * Purpose:
 *   Guides the user through creating a new account. Enforces three rules:
 *     1. Username and password must not be empty.
 *     2. Username must not already exist in the system (case-sensitive).
 *     3. The total number of users must not exceed MAX_USERS.
 *   On success, the record is written to disk first, then added to the
 *   in-memory array, so both storage layers stay consistent.
 *
 * Parameters:
 *   users  — In-memory user list; the new entry is appended on success.
 *   count  — Pointer to the current user count; incremented on success.
 *
 * Return Value:
 *   1 — Account created successfully.
 *   0 — Registration aborted due to a validation error or I/O failure.
 * ───────────────────────────────────────────────────────────────────────── */
int registerUser(User users[], int *count) {
    char inputBuffer[256];
    User newUser;
    int  i;

    printf("\n--- Register New Account ---\n");

    /* ── Step 1: Collect and validate the username ─────────────────────── */
    printf("Username : ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

    if (strlen(inputBuffer) == 0) {
        printf("[ERROR] Username cannot be empty.\n");
        printf("Press Enter to continue...");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        return 0;
    }

    strncpy(newUser.username, inputBuffer, MAX_USERNAME - 1);
    newUser.username[MAX_USERNAME - 1] = '\0';

    /* Check for duplicate usernames (case-sensitive linear search). */
    for (i = 0; i < *count; i++) {
        if (strcmp(users[i].username, newUser.username) == 0) {
            printf("[ERROR] Username '%s' is already taken. "
                   "Please choose another.\n", newUser.username);
            printf("Press Enter to continue...");
            fgets(inputBuffer, sizeof(inputBuffer), stdin);
            return 0;
        }
    }

    /* ── Step 2: Collect and validate the password ─────────────────────── */
    printf("Password : ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

    if (strlen(inputBuffer) == 0) {
        printf("[ERROR] Password cannot be empty.\n");
        printf("Press Enter to continue...");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        return 0;
    }

    strncpy(newUser.password, inputBuffer, MAX_PASSWORD - 1);
    newUser.password[MAX_PASSWORD - 1] = '\0';

    /* ── Step 3: Verify the user cap has not been reached ──────────────── */
    if (*count >= MAX_USERS) {
        printf("[ERROR] The system has reached its maximum of %d users.\n",
               MAX_USERS);
        printf("Press Enter to continue...");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        return 0;
    }

    /* ── Step 4: Persist to disk, then update the in-memory array ──────── */
    /* Write to disk first. If disk write fails, do not update the array,
     * preventing a state where the session has a user the file does not. */
    if (saveUser(&newUser) == 0) {
        printf("[ERROR] Failed to save credentials to disk.\n");
        printf("Press Enter to continue...");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        return 0;
    }

    /* Struct assignment copies all fields at once — no manual field loop needed. */
    users[*count] = newUser;
    (*count)++;

    printf("[SUCCESS] Account '%s' has been registered.\n", newUser.username);
    printf("Press Enter to continue...");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    return 1;
}


/* ─────────────────────────────────────────────────────────────────────────
 * loginUser
 *
 * Purpose:
 *   Prompts for a username and password, then searches the in-memory user
 *   array for a matching pair. If found, copies the authenticated username
 *   into the caller's buffer so the session can identify the current user
 *   (e.g., to resolve the per-user task file path in Phase 2).
 *
 * Parameters:
 *   users         — Loaded user list. Not modified.
 *   count         — Number of entries in the user list.
 *   loggedInUser  — Output buffer (size >= MAX_USERNAME); filled on success.
 *
 * Return Value:
 *   1 — Credentials matched; loggedInUser has been populated.
 *   0 — No match found; loggedInUser is unchanged.
 * ───────────────────────────────────────────────────────────────────────── */
int loginUser(const User users[], int count, char *loggedInUser) {
    char inputUsername[MAX_USERNAME];
    char inputPassword[MAX_PASSWORD];
    char inputBuffer[256];
    int  i;

    printf("\n--- Login ---\n");

    printf("Username : ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
    strncpy(inputUsername, inputBuffer, MAX_USERNAME - 1);
    inputUsername[MAX_USERNAME - 1] = '\0';

    printf("Password : ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
    strncpy(inputPassword, inputBuffer, MAX_PASSWORD - 1);
    inputPassword[MAX_PASSWORD - 1] = '\0';

    /* Linear search — O(n). Acceptable given the MAX_USERS cap of 100. */
    for (i = 0; i < count; i++) {
        if (strcmp(users[i].username, inputUsername) == 0 &&
            strcmp(users[i].password, inputPassword) == 0) {

            strncpy(loggedInUser, users[i].username, MAX_USERNAME - 1);
            loggedInUser[MAX_USERNAME - 1] = '\0';

            printf("[SUCCESS] Welcome back, %s!\n", loggedInUser);
            printf("Press Enter to continue...");
            fgets(inputBuffer, sizeof(inputBuffer), stdin);
            return 1;
        }
    }

    printf("[ERROR] Incorrect username or password.\n");
    printf("Press Enter to continue...");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    return 0;
}
