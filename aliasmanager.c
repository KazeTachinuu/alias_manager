#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int IsAliasInLine(char * line, char* alias_name){
    int len = strlen(alias_name);
    int len2 = strlen(line);


    int i = 6;
    int j =0;


    while (i < len2 && j < len && line[i] == alias_name[j]) {
        i++;
        j++;
    }

    return i < len2 && j == len && line[i] == '=';
}

// Function to create an alias
void create_alias(const char *alias_name, const char *alias_command) {
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt", home_dir);

    // Open .my_aliases.txt file for appending
    FILE *file = fopen(alias_file_path, "a");
    if (file == NULL) {
        perror("Failed to open the alias file");
        return;
    }

    // Write alias to the file
    fprintf(file, "alias %s='%s'\n", alias_name, alias_command);

    fclose(file);

    printf("Alias '%s' with command '%s' created successfully!\nReload the terminal to use it.\n", alias_name, alias_command);
}

// Function to remove an alias
void remove_alias(char *alias_name, int is_forced) {
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt", home_dir);

    // Open .my_aliases.txt file for reading
    FILE *file = fopen(alias_file_path, "r");
    if (file == NULL) {
        perror("Failed to open the alias file");
        return;
    }

    char line[1024];
    int found = 0;

    // Read the lines from the file
    while (fgets(line, sizeof(line), file) != NULL) {
        if (IsAliasInLine(line, alias_name)) {
            found = 1;

            // Display the alias to be removed
            printf("Removing alias:\n%s", line);

            if (!is_forced) {
                // Prompt user for confirmation
                printf("Do you want to remove this alias? (Y/n): ");
                char confirmation[10];
                if (fgets(confirmation, sizeof(confirmation), stdin) != NULL) {
                    confirmation[strcspn(confirmation, "\n")] = 0; // Remove newline
                    if (strcmp(confirmation, "y") != 0 && strcmp(confirmation, "Y") != 0 && strcmp(confirmation, "") != 0) {
                        printf("Aborted.\n");
                        fclose(file);
                        return;
                    }
                }
            }
        }
    }

    fclose(file);

    if (found) {
        // Open the file again for writing
        file = fopen(alias_file_path, "a");
        if (file == NULL) {
            perror("Failed to open the alias file");
            return;
        }

        // Write the lines back to the file, excluding the one containing the alias

        while (fgets(line, sizeof(line), file) != NULL) {
            if (!IsAliasInLine(line, alias_name)) {
                fprintf(file, "%s", line);
            }
        }

        fclose(file);

        if (!is_forced) {
            printf("Alias '%s' removed successfully!\n", alias_name);
        }
    } else {
        printf("Alias '%s' not found.\n", alias_name);
    }
}

// Function to list all aliases
void list_aliases() {
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt", home_dir);

    // Open .my_aliases.txt file for reading
    FILE *file = fopen(alias_file_path, "r");
    if (file == NULL) {
        perror("Failed to open the alias file");
        return;
    }

    char line[1024];

    printf("Aliases:\n");

    // Extract aliases from the file content
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "alias") == line) {
            char *alias_name = strtok(line, "=");
            char *alias_command = strtok(NULL, "'");

            printf("%s => %s\n", alias_name, alias_command);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [options]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        if (argc != 4) {
            printf("Usage: %s add <alias_name> <alias_command>\n", argv[0]);
            return 1;
        }

        create_alias(argv[2], argv[3]);
    } else if (strcmp(argv[1], "rm") == 0) {
        if (argc < 3) {
            printf("Usage: %s rm <alias_name> [force]\n", argv[0]);
            return 1;
        }

        int is_forced = 0;
        if (argc == 4 && strcmp(argv[3], "force") == 0) {
            is_forced = 1;
        }

        remove_alias(argv[2], is_forced);
    } else if (strcmp(argv[1], "ls") == 0) {
        if (argc != 2) {
            printf("Usage: %s ls\n", argv[0]);
            return 1;
        }

        list_aliases();
    } else {
        printf("Invalid command. Available commands: add, rm, ls\n");
        return 1;
    }

    return 0;
}

