#ifndef AUTH_H
#define AUTH_H

/*
 * auth.h  —  Public interface for the User Authentication module.
 *
 * This header is included by both auth.c (implementation) and main.c (caller).
 * It exposes:
 *   - Shared constants used across the authentication system.
 *   - The User struct that represents a single registered account.
 *   - Function prototypes for all authentication operations.
 *
 * Include guards (#ifndef AUTH_H / #define AUTH_H / #endif) ensure this
 * file is processed only once per compilation unit, preventing duplicate
 * type and constant definitions.
 */

/* ── Constants ───────────────────────────────────────────────────────────── */

#define MAX_USERNAME  50            /* Max username length, including null terminator  */
#define MAX_PASSWORD  50            /* Max password length, including null terminator  */
#define MAX_USERS     100           /* Hard cap on the total number of registered users */
#define USERS_FILE    "users.txt"   /* Flat-file database for persisted credentials    */

/* ── Data Structures ─────────────────────────────────────────────────────── */

/*
 * User  —  Represents a single registered account.
 *
 * Fixed-size char arrays are used deliberately: the struct is trivially
 * copyable by value (no malloc/free required), and the size is predictable
 * for a project of this scope. A production system would store a password
 * hash rather than the plaintext value.
 */
typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} User;

/* ── Function Prototypes ─────────────────────────────────────────────────── */

/*
 * loadUsers  —  Populates the in-memory user array from USERS_FILE.
 * Parameters : users[]  — output array to fill; count — output user count.
 * Returns    : 1 on success, 0 if the file does not exist yet.
 */
int loadUsers(User users[], int *count);

/*
 * saveUser  —  Appends one user record to USERS_FILE in "username:password" format.
 * Parameters : user — pointer to the User struct to persist (read-only).
 * Returns    : 1 on success, 0 if the file could not be opened for writing.
 */
int saveUser(const User *user);

/*
 * registerUser  —  Interactively collects credentials and registers a new account.
 * Parameters : users[] — in-memory list (new entry appended on success);
 *              count   — pointer to current count (incremented on success).
 * Returns    : 1 on success, 0 on validation failure or I/O error.
 */
int registerUser(User users[], int *count);

/*
 * loginUser  —  Prompts for credentials and authenticates against the user list.
 * Parameters : users[]       — loaded user list (read-only);
 *              count         — number of entries in the list;
 *              loggedInUser  — output buffer (>= MAX_USERNAME) filled on success.
 * Returns    : 1 on success, 0 if no matching credentials were found.
 */
int loginUser(const User users[], int count, char *loggedInUser);

#endif /* AUTH_H */
