#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Version "6.0.0"

int IsAliasInLine(const char *line, const char *alias_name)
{

    int i = 6;
    int j = 0;

    while (line[i] != '\0' && alias_name[j] != '\0' && line[i] == alias_name[j])
    {
        i++;
        j++;
    }

    return line[i] != '\0' && alias_name[j] =='\0' && line[i] == '=';
}

// Function to create an alias
void create_alias(const char *alias_name, const char *alias_command)
{
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL)
    {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt",
            home_dir);

    // Open .my_aliases.txt file for appending
    FILE *file = fopen(alias_file_path, "a");
    if (file == NULL)
    {
        perror("Failed to open the alias file");
        return;
    }

    // Write alias to the file
    //if char ' in alias_command, replace with \' and surround with double quotes
    if (strchr(alias_command, '\'') != NULL)
    {
        //replace ' with \'
        char *alias_command2 = malloc(strlen(alias_command) + 1);
        int i = 0;
        int j = 0;
        while (alias_command[i] != '\0')
        {
            if (alias_command[i] == '\'')
            {
                alias_command2[j] = '\\';
                j++;
                alias_command2[j] = '\'';
            }
            else
            {
                alias_command2[j] = alias_command[i];
            }
            i++;
            j++;
        }
        alias_command2[j] = '\0';

        fprintf(file, "alias %s=\"%s\"\n", alias_name, alias_command2);

    }
    else
    {
        fprintf(file, "alias %s='%s'\n", alias_name, alias_command);
    }

    fclose(file);

    printf("Alias '%s' with command '%s' created successfully!\nReload the "
            "terminal to use it.\n",
            alias_name, alias_command);
}

// Function to remove an alias
void remove_alias(char *alias_name, int is_forced)
{
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL)
    {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt",
            home_dir);

    // Open .my_aliases.txt file for reading
    FILE *file = fopen(alias_file_path, "r");
    if (file == NULL)
    {
        perror("Failed to open the alias file");
        return;
    }

    char line[1024];
    char lines[1024][1024];
    int found = 0;
    int i = 0;

    // Read the lines from the file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // copy the line to the array
        strcpy(lines[i], line);
        i++;

        if (found != 1 && IsAliasInLine(line, alias_name))
        {
            found = 1;

            // Display the alias to be removed
            printf("Removing alias:\n%s", line);

            if (!is_forced)
            {
                // Prompt user for confirmation
                printf("Do you want to remove this alias? (Y/n): ");
                char confirmation[10];
                if (fgets(confirmation, sizeof(confirmation), stdin) != NULL)
                {
                    confirmation[strcspn(confirmation, "\n")] =
                        0; // Remove newline
                    if (strcmp(confirmation, "y") != 0
                            && strcmp(confirmation, "Y") != 0
                            && strcmp(confirmation, "") != 0)
                    {
                        printf("Aborted.\n");
                        fclose(file);
                        return;
                    }
                }
            }
        }
    }

    fclose(file);

    if (found)
    {
        // Open the file again for writing
        file = fopen(alias_file_path, "w");
        if (file == NULL)
        {
            perror("Failed to open the alias file");
            return;
        }

        // Write the lines back to the file, excluding the one containing the
        // alias

        int j = 0;
        while (j < i)
        {
            if (!IsAliasInLine(lines[j], alias_name))
            {
                fprintf(file, "%s", lines[j]);
            }
            j++;
        }

        fclose(file);

        if (!is_forced)
        {
            printf("Alias '%s' removed successfully!\n", alias_name);
        }
    }
    else
    {
        printf("Alias '%s' not found.\n", alias_name);
    }
}

// Function to list all aliases
void list_aliases()
{
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL)
    {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt",
            home_dir);

    // Open .my_aliases.txt file for reading
    FILE *file = fopen(alias_file_path, "r");
    if (file == NULL)
    {
        perror("Failed to open the alias file");
        return;
    }

    char line[1024];

    printf("Aliases:\n");

    // Extract aliases from the file content
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strstr(line, "alias") == line)
        {
            //print the line but without the "alias" word
            printf("%s", line + 6);
        }
    }
    printf("\n");

    fclose(file);
}

void find_aliases(char *str)
{
    // Get the value of HOME environment variable
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL)
    {
        perror("Failed to get the HOME directory");
        return;
    }

    // Construct the full path to the alias file
    char alias_file_path[1024];
    snprintf(alias_file_path, sizeof(alias_file_path), "%s/.my_aliases.txt",
            home_dir);

    // Open .my_aliases.txt file for reading
    FILE *file = fopen(alias_file_path, "r");
    if (file == NULL)
    {
        perror("Failed to open the alias file");
        return;
    }

    char line[1024];

    printf("Aliases matching \"%s\": \n", str);

    // Extract aliases from the file content
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strstr(line, "alias") == line)
        {

            if (strstr(line, str))
            {
                printf("%s", line + 6);
            }
        }
    }

    fclose(file);
}
void show_version(void)
{
    printf("aliasmanager v%s\n", Version);
}

void show_help(void)
{
    show_version();
    printf("Manage your aliases in ~/.my_aliases.txt\n");
    printf("\n   aliasmanager add <ALIAS_NAME> <COMMAND>     add alias\n");
    printf("   aliasmanager rm <ALIAS_NAME> [-f|--force]   remove alias\n");
    printf("   aliasmanager ls                             list all aliases\n");
    printf("   aliasmanager ls <STRING>                    list all aliases "
            "matching <STRING>");
    printf("\n    -V --version             Show version of this program\n");
    printf("    -h --help                Show this page of help\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        show_help();
        return 1;
    }

    if (strcmp(argv[1], "add") == 0)
    {
        if (argc != 4)
        {
            printf("Usage: %s add <ALIAS_NAME> <COMMAND>\n", argv[0]);
            return 1;
        }

        create_alias(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "rm") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: %s rm <ALIAS_NAME> [-f|--force]\n", argv[0]);
            return 1;
        }

        int is_forced = 0;
        if ((argc == 4 && strcmp(argv[3], "--force") == 0)
                || (argc == 4 && strcmp(argv[3], "-f") == 0))
        {
            is_forced = 1;
        }

        remove_alias(argv[2], is_forced);
    }
    else if (strcmp(argv[1], "ls") == 0)
    {
        if (argc > 3)
        {
            printf("Usage: %s ls [<STRING>]\n", argv[0]);
            return 1;
        }
        if (argc == 2)
        {
            list_aliases();
        }
        if (argc == 3)
        {
            find_aliases(argv[2]);
        }
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        show_help();
        return 0;
    }
    else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-V") == 0)
    {
        show_version();
        return 0;
    }
    else
    {
        printf("Unknown subcommand '%s'\n", argv[1]);
        printf("Usage: %s <SUBCOMMAND> <ARGS>\n", argv[0]);
        printf("\nConsider using '%s -h | --help' for more information\n", argv[0]);
        return 1;
    }

    return 0;
}
