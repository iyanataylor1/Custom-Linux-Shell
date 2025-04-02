// OS UM4
// Custom Shell Project
// Iyana Taylor, Raheim Burkett, Breanna Smith, Raul Miller
// Tutor: Mr. Philip Smith

#include <stdio.h>      // library for i/o functions
#include <stdlib.h>     // library for memory allocation and process control
#include <string.h>     // string manipulation functions
#include <unistd.h>     // unix standard functions (system-level operations)
#include <fcntl.h>      // file control operations
#include <errno.h>      // error number definitions
#include <sys/stat.h>   // file status functions for mkdir
#include <time.h>       // time handling for last modified time in stat
#include <sys/types.h>
#include <sys/wait.h>   // needed for wait() and waitpid() functions (using in piping and redirection methods)
#include <dirent.h>     // needed for directory operations (opendir, readdir, and closedir and struct dirent)

#define CUST_MAX_INPUT 1024  // define maximum input size
#define MAX_ARGS 10     // maximum number of arguments for commands

// function declarations
void display_shell_prompt();      // function to display the shell prompt
int read_input(char *input);      // function to read input from the user
void execute_command(char **args);             // execute user commands
void parse_input(char *input, char **args);    // parse input into command and arguments

// file operation functions
void create_file(char *filename);              // create a file
void delete_file(char *filename);              // delete a file
void rename_file(char *old_name, char *new_name); // eename a file

// directory operation functions
void make_directory(char *dirname);            // make a directory
void remove_directory(char *dirname);          // remove a directory
void change_directory(char *dirname);          // change current directory

// functions for file access and permissions
void modify_permissions(char *permissions, char *filename); // modify file permissions
void list_attributes(char *filename);                       // display file attributes

// funtions for environment variable handling
void set_environment_variable(char *arg); // set or update environment variable
void get_environment_variable(char *varname); // retrieve and display environment variable

// functions for redirection and piping
//hold implementation

// function to display the help menu
void help_menu();

int main() {
    char input[CUST_MAX_INPUT];           // buffer to store user input
    char *args[MAX_ARGS];            // array to store parsed arguments

    // infinite loop to keep the shell running until the user types "exit"
    while (1) {
        display_shell_prompt();          // show the shell prompt to the user
        if (read_input(input) == -1) {   // read input and check for errors
            fprintf(stderr, "Error reading input.\n"); // error message if input fails
            continue;                     // skip to the next loop iteration on error
        }
        if (strcmp(input, "exit") == 0) { // check if the user wants to exit
            printf("Exiting shell...\n");
            break;                        // exit the loop (the custom shell) if the command is "exit"
        }
        parse_input(input, args);        // parse (break up, separate) the input into command and arguments
        execute_command(args);           // execute the parsed command
    }
    return 0;  // successful program termination
}

// function to display a simple shell prompt to indicate the user can enter a command
void display_shell_prompt() {
    printf("\n");
    printf("custom_shell> ");
    fflush(stdout);  // ensure the prompt appears immediately by flushing the output buffer
}

// function to read input from the user using fgets; store result in 'input' 
int read_input(char *input) {
    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        return -1;  // return -1 if fgets fails, indicating an error
    }
    
    // remove newline character if present, so the command is clean
    input[strcspn(input, "\n")] = '\0';  // find newline and replace it with null terminator

    // check if the input is empty; if so, return -1 as an error
    if (strlen(input) == 0) {
    }
    return 0;  // successful input reading
}

// function to parse input into command and arguments
void parse_input(char *input, char **args) {
    int i = 0;                           // counter for arguments
    char *token = strtok(input, " ");    // tokenize input string by spaces

    // loop to parse tokens into arguments array
    while (token != NULL && i < MAX_ARGS - 1) {
    	if (token[0] == '\"') {           // check if token starts with a quote
            args[i] = token + 1;          // remove initial quote
            token = strtok(NULL, "\"");   // find ending quote
        } else {
            args[i] = token;              // assign token to args array, regular token without quotes
        }
        i++;
        token = strtok(NULL, " ");       // move to the next token
    }									 // the spaces in the file and directory names are being ignored, cannot create/ make with spaces in the names
    args[i] = NULL;                      // NULL terminate the args array
}

// function to display the help menu
void help_menu() {
    printf("\n======================= Help Menu =======================\n");
    printf("%-15s %-35s %-30s\n", "Command", "Description", "Example");
    printf("--------------------------------------------------------\n");
    printf("%-15s %-35s %-30s\n", "create", "Create a new file", "create file_name.txt");
    printf("%-15s %-35s %-30s\n", "delete", "Delete an existing file", "delete file_name.txt");
    printf("%-15s %-35s %-30s\n", "rename", "Rename a file", "rename old_name.txt new_name.txt");
    printf("%-15s %-35s %-30s\n", "make", "Create a new directory", "make directory_name");
    printf("%-15s %-35s %-30s\n", "remove", "Remove a directory", "remove directory_name");
    printf("%-15s %-35s %-30s\n", "change", "Change the current directory", "change directory_name");
    printf("%-15s %-35s %-30s\n", "modify", "Modify permissions", "modify 755 file_name.txt");
    printf("%-15s %-35s %-30s\n", "list", "List attributes", "list -l file_name.txt");
    printf("%-15s %-35s %-30s\n", "set", "Set an environment variable", "set VARIABLE_NAME=value");
    printf("%-15s %-35s %-30s\n", "get", "Get an environment variable", "get VARIABLE_NAME");
    printf("%-15s %-35s %-30s\n", "help", "Display this help menu", "help");
    printf("%-15s %-35s %-30s\n", "exit", "Exit the shell", "exit");
    
    printf("\nUse 'change ..' to go back to the previous directory.");

    printf("\nNote: Avoid using spaces in file and directory names.\n");
    printf("Use underscores (_) or hyphens (-) instead, e.g., file_name.txt or file-name.txt.\n");
    printf("========================================================\n");
}


// function to execute the parsed command
void execute_command(char **args) {
    // check if no command was entered
    if (args[0] == NULL) {
        fprintf(stderr, "No command entered.\n");
        return;
    }

    // match command to a specific function
    if (strcmp(args[0], "help") == 0) {
        help_menu(); // display the help menu
    } else if (strcmp(args[0], "create") == 0 && args[1] != NULL) {
        create_file(args[1]);             // call create_file function
    } else if (strcmp(args[0], "delete") == 0 && args[1] != NULL) {
        delete_file(args[1]);             // call delete_file function
    } else if (strcmp(args[0], "rename") == 0 && args[1] != NULL && args[2] != NULL) {
        rename_file(args[1], args[2]);    // call rename_file function
    } else if (strcmp(args[0], "make") == 0 && args[1] != NULL) {
        make_directory(args[1]);          // call make_directory function
    } else if (strcmp(args[0], "remove") == 0 && args[1] != NULL) {
        remove_directory(args[1]);        // call remove_directory function
    } else if (strcmp(args[0], "change") == 0 && args[1] != NULL) {
        change_directory(args[1]);        // call change_directory function
    } else if (strcmp(args[0], "modify") == 0 && args[1] != NULL && args[2] != NULL) {
        modify_permissions(args[1], args[2]);           // modify file permissions
    } else if (strcmp(args[0], "list") == 0 && args[1] != NULL && strcmp(args[1], "-l") == 0 && args[2] != NULL) {
        list_attributes(args[2]);                       // list file attributes
    } else if (strcmp(args[0], "set") == 0 && args[1] != NULL) {
        set_environment_variable(args[1]);           // set environment variable
    } else if (strcmp(args[0], "get") == 0 && args[1] != NULL) {
        get_environment_variable(args[1]);           // get environment variable
    // add the handle_redirection and handle_piping methods here 
    } else {
        fprintf(stderr, "Unknown command or missing arguments.\n");
    }
}

// function to create a file
void create_file(char *filename) {
    printf("Creating file: %s\n", filename);
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644); // create file with write permissions immediately 

    if (fd == -1) { // error handling if file cannot be created
        fprintf(stderr, "Error creating file %s: %s\n", filename, strerror(errno));
    } else {
        printf("File %s created successfully.\n", filename);
        close(fd);  // close file descriptor
    }
}

// function to delete a file
void delete_file(char *filename) {
    printf("Deleting file: %s\n", filename);
    if (unlink(filename) == -1) { // error handling if file cannot be deleted
        fprintf(stderr, "Error deleting file %s: %s\n", filename, strerror(errno));
    } else {
        printf("File %s deleted successfully.\n", filename);
    }
}

// function to rename a file
void rename_file(char *old_name, char *new_name) {
    printf("Renaming file from %s to %s\n", old_name, new_name);
    if (rename(old_name, new_name) == -1) { // error handling if file cannot be renamed
        fprintf(stderr, "Error renaming file from %s to %s: %s\n", old_name, new_name, strerror(errno));
    } else {
        printf("File %s renamed to %s successfully.\n", old_name, new_name);
    }
}

// function to create a directory
void make_directory(char *dirname) {
    printf("Creating directory: %s\n", dirname);
    if (mkdir(dirname, 0755) == -1) {         // create directory with rwxr-xr-x permissions immediately 
        fprintf(stderr, "Error creating directory %s: %s\n", dirname, strerror(errno));
    } else {
        printf("Directory %s created successfully.\n", dirname);
    }
}

// function to remove a directory and its contents
void remove_directory(char *dirname) {
    printf("Attempting to remove directory: %s\n", dirname);

    // try to remove the directory first (only works if it's empty)
    if (rmdir(dirname) == 0) {
        printf("Directory %s removed successfully.\n", dirname);
        return;
    }

    // if rmdir failed and the error is due to the directory not being empty
    if (errno == ENOTEMPTY) {
        printf("Directory %s is not empty. Do you want to delete its contents? (y/n): ", dirname);
        char choice;
        scanf(" %c", &choice);  // Get user confirmation
        if (choice != 'y' && choice != 'Y') {
            printf("Directory %s was not removed.\n", dirname);
            return;
        }

        // open the directory
        DIR *dir = opendir(dirname);
        if (!dir) {
            fprintf(stderr, "Error opening directory %s: %s\n", dirname, strerror(errno));
            return;
        }

        struct dirent *entry; // pointer to a struct dirent, used to rep directory entries (like file names) 
        char path[1024];      // declares a path array with a size of 1024; unrelated to CUST_MAX_INPUT because its local

        // iterate through the directory entries
        while ((entry = readdir(dir)) != NULL) {
            // skip the "." and ".." entries
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            // construct the full path to the entry
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

            // check if the entry is a directory or a file
            if (entry->d_type == DT_DIR) {
                // if it's a directory, call remove_directory recursively
                remove_directory(path);
            } else {
                // if it's a file, delete it
                if (unlink(path) == -1) {
                    fprintf(stderr, "Error deleting file %s: %s\n", path, strerror(errno));
                } else {
                    printf("File %s removed successfully.\n", path);
                }
            }
        }

        closedir(dir);

        // try removing the directory again after cleaning its contents
        if (rmdir(dirname) == -1) {
            fprintf(stderr, "Error removing directory %s: %s\n", dirname, strerror(errno));
        } else {
            printf("Directory %s removed successfully.\n", dirname);
        }
    } else {
        // handle other errors from rmdir
        fprintf(stderr, "Error removing directory %s: %s\n", dirname, strerror(errno));
    }
}


// function to change the current working directory
void change_directory(char *dirname) {
    printf("Changing directory to: %s\n", dirname);
    if (chdir(dirname) == -1) {               // change directory
        fprintf(stderr, "Error changing to directory %s: %s\n", dirname, strerror(errno));
    } else {
        printf("Changed directory to %s.\n", dirname);
    }
}

// function to modify file permissions
void modify_permissions(char *permissions, char *filename) {
    printf("Changing file permissions for: %s\n", filename);
    // convert permissions string (e.g., "755") to octal mode_t value
    mode_t mode = strtol(permissions, NULL, 8);

    // use chmod() to set the permissions
    if (chmod(filename, mode) == -1) {
        fprintf(stderr, "Error changing permissions for %s: %s\n", filename, strerror(errno));
    } else {
        printf("Permissions for %s set to %s successfully.\n", filename, permissions);
    }
}

// function to list file attributes
void list_attributes(char *file_name) {
    struct stat file_stat;  // structure to store file attributes

    // use stat() to retrieve file attributes
    if (stat(file_name, &file_stat) == -1) {
        fprintf(stderr, "Error retrieving attributes for %s: %s\n", file_name, strerror(errno));
        return;
    }

    // print file type (whether its user, group or others) and permissions
    printf("File: %s\n", file_name);
    printf("Permissions: ");
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    // print other file attributes like size and the last modified time
    printf("Size: %ld bytes\n", file_stat.st_size);
    printf("Last modified: %s", ctime(&file_stat.st_mtime));
}

// function to set or update an environment variable
void set_environment_variable(char *arg) {
    // find the position of '=' in the argument to separate variable name and value
    char *delimiter = strchr(arg, '=');
    if (delimiter == NULL) {
        fprintf(stderr, "Invalid format. Use set <var_name>=<value>.\n");
        return;
    }

    // separate variable name and value by replacing '=' with '\0' to terminate the string at varname
    *delimiter = '\0';                 // temporarily set '=' to '\0'
    char *varname = arg;              // variable name is the beginning of the string
    char *value = delimiter + 1;       // value is the part after '='

    // use setenv to set the environment variable and check for errors
    if (setenv(varname, value, 1) == -1) {
        fprintf(stderr, "Error setting environment variable %s: %s\n", varname, strerror(errno));
    } else {
        printf("Environment variable %s set to %s.\n", varname, value);
    }

    *delimiter = '='; // restore '=' in the original string
}

// function to get the value of an environment variable
void get_environment_variable(char *varname) {
    // use getenv to retrieve the environment variable value
    char *value = getenv(varname);
    if (value == NULL) {
        fprintf(stderr, "Environment variable %s not found.\n", varname);
    } else {
        printf("%s=%s\n", varname, value);
    }
}

// add the function declarations for redirection and piping below
