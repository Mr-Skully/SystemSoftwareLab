// Limitation: Directory names have to be unique in hierarchical directory structure

#include <stdio.h>                            // for basic i/o
#include <stdlib.h>                           // for exit()
#include <stdint.h>                           // for uint8_t
#include <string.h>                           // for strcmp()
#define MAX_NODES 10                          // maximum no. of nodes in the filesystem
#define DATE "05/09/2020"
#define childlink filelink                    // for hierarchical directory structure
#define siblinglink dirlink                   // for hierarchical directory structure
#define childcount filecount                  // for hierarchical directory structure

struct NODE{
    unsigned int id;                          // unique id for each file/directory in the filesystem
    char name[20];                            // file/folder name
    unsigned int filecount;                   // number of files and folders contained inside
    unsigned int filelink;                    // link to next file
    unsigned int dirlink;                     // link to next directory
    char location[20];                        // parent directory of the file/folder
    char type;                                // f - file, d - directory
    unsigned int size;                        // file/folder size
    char created[11];                         // date of creation
    uint8_t permissions;                      // permissions expressed as a sum of integer values of the allowed permissions (4 - read, 2 - write, 1 - execute)
};

void single_level(const char * path){         // use single level directory structure
    char permission[4], filename[20];                       // to temporarily read permissions in string format before converting to int
    int i, j, choice = 1, flag = 0, last_id = 0;

    struct NODE filesystem[MAX_NODES];        // file system with a maximum size of MAX_NODE nodes (files and directories)
    int nodecount = 0;                        // total no. of nodes currently in the filesystem

    FILE *input = fopen(path, "r");           // read input file
    if (!input) {
        printf("\n[.] Could not read %s.\n\n", path);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", path);

    FILE *output = fopen(".\\single_level_output.txt", "w");                            // open file for writing output
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }
    fprintf(output, "------------------------------\n");

    while (!feof(input)){                                                               // reading input from file and storing it in the filesystem
        fgets(filesystem[nodecount].name, sizeof(filesystem[nodecount].name), input);
        filesystem[nodecount].name[strlen(filesystem[nodecount].name) - 1] = '\0';      // fgets() reads the newline character also into the string variable
        fscanf(input, "%d%*c", &filesystem[nodecount].filecount);
        fgets(filesystem[nodecount].location, sizeof(filesystem[nodecount].location), input);
        filesystem[nodecount].location[strlen(filesystem[nodecount].location) - 1] = '\0';
        fscanf(input, "%c", &filesystem[nodecount].type);
        fscanf(input, "%d", &filesystem[nodecount].size);
        fscanf(input, "%s", filesystem[nodecount].created);
        fscanf(input, "%s%*c", permission);                                             // read permissions as a string (like 'rwx') and individually update the permissions inside the node
        filesystem[nodecount].permissions = 0;
        for (i = 0; i < 3 && permission[i]!='\0'; i++) {
            switch (permission[i]) {
                case 'r':   filesystem[nodecount].permissions |= (1 << 2);              // read permission
                            break;
                case 'w':   filesystem[nodecount].permissions |= (1 << 1);              // write permission
                            break;
                case 'x':   filesystem[nodecount].permissions |= (1 << 0);              // execute permission
                            break;
                default:    break;
            }
        }
        filesystem[nodecount].id = ++last_id;                                           // assign a new unique id to the node
        filesystem[nodecount].dirlink = filesystem[nodecount].filelink = 0;             // assuming this the last node in the chain for now
        if(filesystem[nodecount].type == 'f' || filesystem[nodecount].type == 'F')      // files are referenced in directory's file link, or in the file link of other files in the directory.
            filesystem[nodecount - 1].filelink = filesystem[nodecount].id;
        nodecount++;
    }
    fclose(input);

    printf("\nSingle Level Directory Structure\n--------------------------------\n");   // menu
    printf("1. Create a file\n2. Search for a file\n3. Delete a file\n0. Exit\n");
    do {
        printf("\nEnter Option #");
        scanf("%d", &choice);
        if(!choice)                                                 // exit the menu
            break;
        else if(choice == 1){                                       // create a file in the directory
            if(nodecount == MAX_NODES){                                                 // storage full
                printf("Insufficient Storage Space.\n");
                continue;
            }
            printf("Enter new filename: ");
            scanf("%*c%[^\n]%*c", filename);                                            // alternative for gets() that removes the trailing newline from the buffer
            for(i = 1; i < nodecount && strcmp(filename, filesystem[i].name); i++);
            if (i == nodecount){                                                        // file doesn't already exist
                filesystem[nodecount].id = ++last_id;
                strcpy(filesystem[nodecount].name, filename);
                filesystem[nodecount].filecount = 1;
                filesystem[0].filecount++;                                              // increment file count for directory
                filesystem[nodecount].filelink = filesystem[nodecount].dirlink = 0;
                filesystem[nodecount - 1].filelink = filesystem[nodecount].id;
                strcpy(filesystem[nodecount].location, filesystem[0].name);
                filesystem[nodecount].type = 'f';
                printf("Enter filesize in KB: ");
                scanf("%d", &filesystem[nodecount].size);
                filesystem[0].size += filesystem[nodecount].size;                       // increment total size of directory
                strcpy(filesystem[nodecount].created, DATE);
                filesystem[nodecount].permissions = 0;
                printf("Read permission? (1 for yes, 0 for no): ");
                scanf("%d", &flag);
                if (flag == 1) filesystem[nodecount].permissions += 4;
                printf("Write permission? (1 for yes, 0 for no): ");
                scanf("%d", &flag);
                if (flag == 1) filesystem[nodecount].permissions += 2;
                printf("Execute permission? (1 for yes, 0 for no): ");
                scanf("%d", &flag);
                if (flag == 1) filesystem[nodecount].permissions += 1;
                nodecount++;
                printf("%s created successfully.\n", filename);
            }
            else{                                                                       // file already exists
                printf("%s already exists.\n", filename);
            }
        }
        else if(choice == 2){                                       // search for a file in the directory
            printf("Enter filename to search: ");
            scanf("%*c%[^\n]%*c", filename);
            for(i = 1; i < nodecount && strcmp(filename, filesystem[i].name); i++);
            if(i == nodecount)                                                          // file doesn't exist
                printf("File not found in the %s.\n", filesystem[0].name);
            else{                                                                       // file exists
                printf("%s found in %s.\n", filename, filesystem[0].name);
                printf(" Size: %d KB\n", filesystem[i].size);
                printf(" Created On: %s\n", filesystem[i].created);
                printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
            }
        }
        else if(choice == 3){                                       // delete a file in the directory
            printf("Enter filename to delete: ");
            scanf("%*c%[^\n]%*c", filename);
            for(i = 1; i < nodecount && strcmp(filename, filesystem[i].name); i++);
            if(i == nodecount)                                                          // file doesn't exist
                printf("%s not found in the %s.\n", filename, filesystem[0].name);
            else{                                                                       // file exists
                printf("File '%s' found in %s.\n", filename, filesystem[i].location);
                printf(" Size: %d KB\n", filesystem[i].size);
                printf(" Created On: %s\n", filesystem[i].created);
                printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
                printf(" Deleted %s successfully.\n", filename);
                filesystem[0].filecount--;                                              // decrement total file count for directory
                printf(" Remaining file count: %d\n", filesystem[0].filecount - 1);
                filesystem[0].size -= filesystem[i].size;                               // decrement total size of directory
                if(i == nodecount - 1)                                                  // if this is the last node, set the file link of the previous file/folder to NULL
                    filesystem[i-1].filelink = 0;
                else                                                                    // else, set the file link of the previous file/folder to the ID of the next node
                    filesystem[i-1].filelink = filesystem[i+1].id;
                nodecount--;
                for(j = i; j < nodecount; j++)                                          // delete the node (by overwriting)
                    filesystem[j] = filesystem[j+1];
            }
        }
        else                                                        // invalid menu option
            printf("Invalid Option.\n");
    } while (1);

    for(i = 0; i < nodecount; i++){                                 // write the state of the filesystem to file
        fprintf(output, " ID #%d\n", filesystem[i].id);
        fprintf(output, " Name: %s\n", filesystem[i].name);
        fprintf(output, " File Count: %d\n", filesystem[i].filecount);
        if(filesystem[i].filelink)
            fprintf(output, " File Link: ID #%d\n", filesystem[i].filelink);
        if(filesystem[i].dirlink)
            fprintf(output, " Directory Link: ID #%d\n", filesystem[i].dirlink);
        fprintf(output, " Location: %s\n", filesystem[i].location);
        fprintf(output, " Type: %s\n", filesystem[i].type=='f'?"File":"Directory");
        fprintf(output, " Size: %d KB\n", filesystem[i].size);
        fprintf(output, " Created On: %s\n", filesystem[i].created);
        fprintf(output, " Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
        fprintf(output, "------------------------------\n");
    }
    fclose(output);
    printf("\n[.] Finished writing current state of filesystem to single_level_output.txt.\n\n");
}

void two_level(const char * path){
    char permission[4], filename[20], folder[20];
    int i, j, choice = 1, flag = 0, last_id = 0;

    struct NODE filesystem[MAX_NODES];        // file system with a maximum size of MAX_NODE nodes (files and directories)
    int nodecount = 0;                        // total no. of nodes currently in the filesystem

    FILE *input = fopen(path, "r");           // read input file
    if (!input) {
        printf("\n[.] Could not read %s.\n\n", path);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", path);

    FILE *output = fopen(".\\two_level_output.txt", "w");                        // open file for writing output
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }
    fprintf(output, "------------------------------\n");

    while (!feof(input)){                                           // read nodes from file
        fgets(filesystem[nodecount].name, sizeof(filesystem[nodecount].name), input);
        filesystem[nodecount].name[strlen(filesystem[nodecount].name) - 1] = '\0';      // fgets() stores the trailing newline
        fscanf(input, "%d%*c", &filesystem[nodecount].filecount);
        fgets(filesystem[nodecount].location, sizeof(filesystem[nodecount].location), input);
        filesystem[nodecount].location[strlen(filesystem[nodecount].location) - 1] = '\0';
        fscanf(input, "%c", &filesystem[nodecount].type);
        fscanf(input, "%d", &filesystem[nodecount].size);
        fscanf(input, "%s", filesystem[nodecount].created);
        fscanf(input, "%s%*c", permission);
        filesystem[nodecount].permissions = 0;
        for (i = 0; i < 3 && permission[i]!='\0'; i++) {
            switch (permission[i]) {
                case 'r':   filesystem[nodecount].permissions |= (1 << 2);
                    break;
                case 'w':   filesystem[nodecount].permissions |= (1 << 1);
                    break;
                case 'x':   filesystem[nodecount].permissions |= (1 << 0);
                    break;
                default:    break;
            }
        }
        filesystem[nodecount].id = ++last_id;
        filesystem[nodecount].dirlink = filesystem[nodecount].filelink = 0;
        for(i = nodecount - 1; filesystem[nodecount].type == 'f' && i >= 0; i--){       // assigning file links
            if (filesystem[i].type == 'f' && !strcmp(filesystem[nodecount].location, filesystem[i].location)){
                filesystem[i].filelink = filesystem[nodecount].id;
                break;
            }
            if (filesystem[i].type == 'd' && !strcmp(filesystem[nodecount].location, filesystem[i].name)){
                filesystem[i].filelink = filesystem[nodecount].id;
                break;
            }
        }
        for(i = nodecount - 1; filesystem[nodecount].type == 'd' && i >= 0; i--){       // assigning directory links
            if (filesystem[i].type == 'd'){
                filesystem[i].dirlink = filesystem[nodecount].id;
                break;
            }
        }
        nodecount++;
    }
    fclose(input);

    printf("\nTwo Level Directory Structure\n-----------------------------\n");         // menu
    printf("1. Create a file\n2. Create a directory\n3. Search for a file\n4. Search for a directory\n5. Delete a file\n6. Delete a directory\n0. Exit\n");
    do {
        printf("\nEnter Option #");
        scanf("%d", &choice);
        if (!choice)                                                // exit the menu
            break;
        else if (choice == 1) {                                     // create a file
            if (nodecount == MAX_NODES){
                printf("Insufficient Storage Space Available.\n");
                continue;
            }
            printf("Enter new filename: ");
            scanf("%*c%[^\n]%*c", filename);
            printf("Available Directories:");
            for(i = 0; i < nodecount; i++)                          // list available directories
                if (filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter location (directory name): ");
            scanf("%[^\n]%*c", folder);                             // substitute for gets()
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount){
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            for(i = 0; i < nodecount; i++)                                              // check whether file already exists in the specified directory
                if (filesystem[i].type == 'f' && !strcmp(filesystem[i].location, folder) && !strcmp(filesystem[i].name, filename))
                    break;
            if(i < nodecount){
                printf("%s already exists in %s.\n", filename, folder);
                continue;
            }
            filesystem[nodecount].id = ++last_id;                                       // continues with file creation
            strcpy(filesystem[nodecount].name, filename);
            filesystem[nodecount].filecount = 1;
            filesystem[j].filecount++;                                                  // increment file count for directory
            filesystem[nodecount].filelink = filesystem[nodecount].dirlink = 0;
            for(i = nodecount - 1; i >= j; i--){
                if (filesystem[i].type == 'f' && !strcmp(folder, filesystem[i].location)){
                    filesystem[i].filelink = filesystem[nodecount].id;
                    break;
                }
                if (filesystem[i].type == 'd' && !strcmp(folder, filesystem[i].name)){
                    filesystem[i].filelink = filesystem[nodecount].id;
                    break;
                }
            }
            strcpy(filesystem[nodecount].location, folder);
            filesystem[nodecount].type = 'f';
            printf("Enter filesize in KB: ");
            scanf("%d", &filesystem[nodecount].size);
            filesystem[j].size += filesystem[nodecount].size;                       // increment total size of directory
            strcpy(filesystem[nodecount].created, DATE);
            filesystem[nodecount].permissions = 0;
            printf("Read permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 4;
            printf("Write permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 2;
            printf("Execute permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 1;
            nodecount++;
            printf("%s created successfully.\n", filename);
        }
        else if (choice == 2){                                      // create a directory
            if (nodecount == MAX_NODES){
                printf("Insufficient Storage Space Available.\n");
                continue;
            }
            printf("\nEnter new directory name: ");
            scanf("%*c%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j < nodecount){
                printf("%s already exists.\n", folder);
                continue;
            }
            filesystem[nodecount].id = ++last_id;
            strcpy(filesystem[nodecount].name, folder);
            filesystem[nodecount].filecount = 1;
            filesystem[nodecount].filelink = filesystem[nodecount].dirlink = 0;
            for(i = nodecount - 1; i >= 0; i--){
                if (filesystem[i].type == 'd'){
                    filesystem[i].dirlink = filesystem[nodecount].id;
                    break;
                }
            }
            strcpy(filesystem[nodecount].location, "Root");                             // directories can only be created at the root
            filesystem[nodecount].type = 'd';
            filesystem[nodecount].size = 1;
            strcpy(filesystem[nodecount].created, DATE);
            filesystem[nodecount].permissions = 0;
            printf("Read permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 4;
            printf("Write permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 2;
            printf("Execute permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 1;
            nodecount++;
            printf("%s created successfully.\n", folder);
        }
        else if (choice == 3){                                      // search for a file
            printf("Enter filename to search: ");
            scanf("%*c%[^\n]%*c", filename);
            for(i = 0, flag = 0; i < nodecount; i++){               // scan all nodes for the search term
                if(filesystem[i].type == 'f' && !strcmp(filename, filesystem[i].name)){
                    printf("\n%s found in %s.\n", filename, filesystem[i].location);
                    printf(" Size: %d KB\n", filesystem[i].size);
                    printf(" Created On: %s\n", filesystem[i].created);
                    printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
                    flag++;
                }
            }
            if(flag)
                printf("\nSearch completed. %d files found.\n", flag);
            else                                                    // if no matches found
                printf("File not found.\n");
        }
        else if (choice == 4){                                      // search for a directory
            printf("Enter directory to search: ");
            scanf("%*c%[^\n]%*c", folder);
            for(i = 0; i < nodecount; i++){                         // scan nodes for search term
                if(filesystem[i].type == 'd' && !strcmp(folder, filesystem[i].name)){
                    printf("%s found.\n", folder);
                    printf(" File Count: %d\n", filesystem[i].filecount);
                    printf(" Size: %d KB\n", filesystem[i].size);
                    printf(" Created On: %s\n", filesystem[i].created);
                    printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
                    break;
                }
            }
            if (i == nodecount)                                     // if no match found
                printf("Directory not found.\n");
        }
        else if (choice == 5){                                      // delete a file
            printf("Enter filename to delete: ");
            scanf("%*c%[^\n]%*c", filename);
            printf("Available Directories:");
            for(i = 0; i < nodecount; i++)
                if (filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter location (directory name) of file: ");
            scanf("%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount){
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            for(i = 0; i < nodecount; i++)                                              // check whether file exists in the specified directory
                if (filesystem[i].type == 'f' && !strcmp(filesystem[i].location, folder) && !strcmp(filesystem[i].name, filename))
                    break;
            if(i == nodecount){
                printf("%s not found in %s.\n", filename, folder);
                continue;
            }
            printf("File '%s' found in %s.\n", filename, folder);
            printf(" Size: %d KB\n", filesystem[i].size);
            printf(" Created On: %s\n", filesystem[i].created);
            printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
            printf(" Deleted %s successfully.\n", filename);
            filesystem[j].filecount--;                                                  // decrement count in parent dir
            printf(" Remaining file count inside %s: %d\n", filesystem[j].name, filesystem[j].filecount - 1);
            filesystem[j].size -= filesystem[i].size;                                   // decrement size of parent dir
            for (j = 0; j < i; j++) {                                                   // reassign file links referencing this file
                if (filesystem[j].filelink == filesystem[i].id){
                    filesystem[j].filelink = filesystem[i].filelink;
                    break;
                }
            }
            nodecount--;
            for(j = i; j < nodecount; j++)                                          // delete the node (by overwriting)
                filesystem[j] = filesystem[j+1];
        }
        else if (choice == 6){                                      // delete a directory
            printf("Available Directories:");
            for(i = 0; i < nodecount; i++)
                if (filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter directory name to delete: ");
            scanf("%*c%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount){
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            printf(" Size: %d KB\n", filesystem[j].size);
            printf(" File Count (including the directory itself): %d\n", filesystem[j].filecount);
            printf(" Created On: %s\n", filesystem[j].created);
            printf(" Permissions: %s %s %s\n", filesystem[j].permissions&(1<<2)?"Read":"-", filesystem[j].permissions&(1<<1)?"Write":"-", filesystem[j].permissions&1?"Execute":"-");
            printf(" Deleted %s successfully.\n", folder);
            for(i = 0; i < j; i++){                                                     // reassign directory links referencing this directory
                if (filesystem[i].dirlink == filesystem[j].id){
                    filesystem[i].dirlink = filesystem[j].dirlink;
                    break;
                }
            }
            for(i = 0; i < nodecount; i++){                                             // delete directory and the files contained in it by overwriting
                if(!strcmp(filesystem[i].location, folder) || (!strcmp(filesystem[i].name, folder) && filesystem[i].type == 'd')) {
                    for (j = i--; j < nodecount - 1; j++)
                        filesystem[j] = filesystem[j + 1];
                    nodecount--;
                }
            }
        }
        else{
            printf("Invalid Option.\n");
        }
    } while (1);

    for(i = 0; i < nodecount; i++){                                                     // write output to file
        fprintf(output, " ID #%d\n", filesystem[i].id);
        fprintf(output, " Name: %s\n", filesystem[i].name);
        fprintf(output, " File Count: %d\n", filesystem[i].filecount);
        if(filesystem[i].filelink)
            fprintf(output, " File Link: ID #%d\n", filesystem[i].filelink);
        if(filesystem[i].dirlink)
            fprintf(output, " Directory Link: ID #%d\n", filesystem[i].dirlink);
        fprintf(output, " Location: %s\n", filesystem[i].location);
        fprintf(output, " Type: %s\n", filesystem[i].type=='f'?"File":"Directory");
        fprintf(output, " Size: %d KB\n", filesystem[i].size);
        fprintf(output, " Created On: %s\n", filesystem[i].created);
        fprintf(output, " Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
        fprintf(output, "------------------------------\n");
    }
    fclose(output);
    printf("\n[.] Finished writing current state of filesystem to two_level_output.txt.\n\n");
}

void hierarchical(const char * path){
    char permission[4], filename[20], folder[20], loc[20];
    int i, j, k, choice = 1, flag = 0, last_id = 0;

    struct NODE filesystem[MAX_NODES];        // file system with a maximum size of MAX_NODE nodes (files and directories)
    int nodecount = 0;                        // total no. of nodes currently in the filesystem

    FILE *input = fopen(path, "r");           // read input file
    if (!input) {
        printf("\n[.] Could not read %s.\n\n", path);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", path);

    FILE *output = fopen(".\\hierarchical_output.txt", "w");                        // open file for writing output
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }
    fprintf(output, "------------------------------\n");

    while (!feof(input)){                                           // read nodes from file
        fgets(filesystem[nodecount].name, sizeof(filesystem[nodecount].name), input);
        filesystem[nodecount].name[strlen(filesystem[nodecount].name) - 1] = '\0';
        fscanf(input, "%d%*c", &filesystem[nodecount].childcount);
        fgets(filesystem[nodecount].location, sizeof(filesystem[nodecount].location), input);
        filesystem[nodecount].location[strlen(filesystem[nodecount].location) - 1] = '\0';
        fscanf(input, "%c", &filesystem[nodecount].type);
        fscanf(input, "%d", &filesystem[nodecount].size);
        fscanf(input, "%s", filesystem[nodecount].created);
        fscanf(input, "%s%*c", permission);
        filesystem[nodecount].permissions = 0;
        for (i = 0; i < 3 && permission[i]!='\0'; i++) {
            switch (permission[i]) {
                case 'r':   filesystem[nodecount].permissions |= (1 << 2);
                    break;
                case 'w':   filesystem[nodecount].permissions |= (1 << 1);
                    break;
                case 'x':   filesystem[nodecount].permissions |= (1 << 0);
                    break;
                default:    break;
            }
        }
        filesystem[nodecount].id = ++last_id;
        filesystem[nodecount].dirlink = filesystem[nodecount].filelink = 0;
        flag = 1;
        for(i = nodecount - 1; i >= 0; i--){                                            // assigning child and sibling links to files and directories
            if(!strcmp(filesystem[i].location, filesystem[nodecount].location)){
                filesystem[i].siblinglink = filesystem[nodecount].id;
                break;
            }
            if(filesystem[i].type == 'd' && !strcmp(filesystem[i].name, filesystem[nodecount].location)){
                filesystem[i].childlink = filesystem[nodecount].id;
                break;
            }
        }
        nodecount++;
    }
    fclose(input);

    printf("\nHierarchical Directory Structure\n--------------------------------\n");               // menu
    printf("1. Create a file\n2. Create a directory\n3. Search for a file\n4. Search for a directory\n5. Delete a file\n6. Delete a directory\n0. Exit\n");
    do {
        printf("\nEnter Option #");
        scanf("%d", &choice);
        if (!choice)                                                // exit the menu
            break;
        else if (choice == 1) {                                     // create a file
            if (nodecount == MAX_NODES){
                printf("Insufficient Storage Space Available.\n");
                continue;
            }
            printf("Enter new filename: ");
            scanf("%*c%[^\n]%*c", filename);
            printf("Available Locations:");                                             // list directories
            for(i = 0; i < nodecount; i++)
                if(filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter location to create new file: ");
            scanf("%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount){
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            for(i = 0; i < nodecount; i++)                                              // check whether file already exists in the specified directory
                if (filesystem[i].type == 'f' && !strcmp(filesystem[i].location, folder) && !strcmp(filesystem[i].name, filename))
                    break;
            if(i < nodecount){
                printf("%s already exists in %s.\n", filename, folder);
                continue;
            }
            filesystem[nodecount].id = ++last_id;                                       // continue creating the new file
            strcpy(filesystem[nodecount].name, filename);
            filesystem[nodecount].childcount = 1;
            filesystem[nodecount].childlink = filesystem[nodecount].siblinglink = 0;
            printf("Enter filesize in KB: ");
            scanf("%d", &filesystem[nodecount].size);

            strcpy(loc, folder);
            while (1){                                                                  // increment attributes of parents
                for(k = 0; k < nodecount; k++)
                    if (!(strcmp(filesystem[k].name, loc)))
                        break;
                if(k == nodecount)
                    break;
                else{
                    filesystem[k].size += filesystem[nodecount].size;                   // increment total size of directory
                    filesystem[k].childcount += filesystem[nodecount].childcount;       // increment file count for directory
                    strcpy(loc, filesystem[k].location);
                }
            }

            for(i = nodecount - 1; i >= j; i--){                                        // assign child or sibling links
                if (!strcmp(folder, filesystem[i].location)){
                    filesystem[i].siblinglink = filesystem[nodecount].id;
                    break;
                }
                if (filesystem[i].type == 'd' && !strcmp(folder, filesystem[i].name)){
                    filesystem[i].childlink = filesystem[nodecount].id;
                    break;
                }
            }
            strcpy(filesystem[nodecount].location, folder);
            filesystem[nodecount].type = 'f';
            strcpy(filesystem[nodecount].created, DATE);
            filesystem[nodecount].permissions = 0;
            printf("Read permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 4;
            printf("Write permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 2;
            printf("Execute permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 1;
            nodecount++;
            printf("%s created successfully.\n", filename);
        }
        else if (choice == 2){                                      // create a directory
            if (nodecount == MAX_NODES){
                printf("Insufficient Storage Space Available.\n");
                continue;
            }
            printf("Enter new directory name: ");
            scanf("%*c%[^\n]%*c", filename);
            printf("Available Locations:\tRoot");                   // root is also a possible location while creating a directory
            for(i = 0; i < nodecount; i++)
                if (filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter location to create new file: ");
            scanf("%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount && strcmp(folder, "Root")){                               // the location is compared with 'root' separately as root is not a defined directory
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            for(i = 0; i < nodecount; i++)                                              // check whether folder already exists in the specified directory
                if (filesystem[i].type == 'd' && !strcmp(filesystem[i].name, filename))
                    break;
            if(i < nodecount){                                                          // directories have to be unique in the whole filesystem even if they are in different directories themselves
                printf("%s already exists in another location.\n", filename);
                continue;
            }
            filesystem[nodecount].id = ++last_id;
            strcpy(filesystem[nodecount].name, filename);
            filesystem[nodecount].childcount = 1;
            filesystem[nodecount].size = 1;
            filesystem[nodecount].childlink = filesystem[nodecount].siblinglink = 0;

            strcpy(loc, folder);
            while (1){                                                                  // increment attributes of parents
                for(k = 0; k < nodecount; k++)
                    if (!(strcmp(filesystem[k].name, loc)))
                        break;
                if(k == nodecount)
                    break;
                else{
                    filesystem[k].size += filesystem[nodecount].size;                   // increment total size of directory
                    filesystem[k].childcount += filesystem[nodecount].childcount;       // increment file count for directory
                    strcpy(loc, filesystem[k].location);
                }
            }

            for(i = nodecount - 1; i >= 0; i--){
                if (!strcmp(folder, filesystem[i].location)){
                    filesystem[i].siblinglink = filesystem[nodecount].id;
                    break;
                }
                if (filesystem[i].type == 'd' && !strcmp(folder, filesystem[i].name)){
                    filesystem[i].childlink = filesystem[nodecount].id;
                    break;
                }
            }
            strcpy(filesystem[nodecount].location, folder);
            filesystem[nodecount].type = 'd';
            strcpy(filesystem[nodecount].created, DATE);
            filesystem[nodecount].permissions = 0;
            printf("Read permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 4;
            printf("Write permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 2;
            printf("Execute permission? (1 for yes, 0 for no): ");
            scanf("%d", &flag);
            if (flag == 1) filesystem[nodecount].permissions += 1;
            nodecount++;
            printf("%s created successfully.\n", filename);
        }
        else if (choice == 3){                                      // search for a file
            printf("Enter filename to search: ");
            scanf("%*c%[^\n]%*c", filename);
            for(i = 0, flag = 0; i < nodecount; i++){
                if(filesystem[i].type == 'f' && !strcmp(filename, filesystem[i].name)){
                    printf("\n%s found in %s.\n", filename, filesystem[i].location);
                    printf(" Size: %d KB\n", filesystem[i].size);
                    printf(" Created On: %s\n", filesystem[i].created);
                    printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
                    flag++;
                }
            }
            if(flag)
                printf("\nSearch completed. %d files found.\n", flag);
            else                                                                        // when no files are found
                printf("File not found.\n");
        }
        else if (choice == 4){                                      // search for a directory
            printf("Enter directory name to search: ");
            scanf("%*c%[^\n]%*c", folder);
            for(i = 0, flag = 0; i < nodecount; i++){
                if(filesystem[i].type == 'd' && !strcmp(folder, filesystem[i].name)){
                    printf("\n%s found in %s.\n", folder, filesystem[i].location);
                    printf(" Child Count (including %s): %d\n", folder, filesystem[i].filecount);
                    printf(" Size: %d KB\n", filesystem[i].size);
                    printf(" Created On: %s\n", filesystem[i].created);
                    printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
                    flag++;
                }
            }
            if(flag)
                printf("\nSearch completed. %d directory(s) found.\n", flag);
            else                                                                        // when no directories are found
                printf("Directory not found.\n");
        }
        else if (choice == 5){                                      // delete a file
            printf("Enter filename to delete: ");
            scanf("%*c%[^\n]%*c", filename);
            printf("Available Directories:");
            for(i = 0; i < nodecount; i++)
                if (filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter location (directory name) of file: ");
            scanf("%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether directory exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount){
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            for(i = 0; i < nodecount; i++)                                              // check whether file exists in the specified directory
                if (filesystem[i].type == 'f' && !strcmp(filesystem[i].location, folder) && !strcmp(filesystem[i].name, filename))
                    break;
            if(i == nodecount){                                                         // if file does not exist in the specified directory
                printf("%s not found in %s.\n", filename, folder);
                continue;
            }
            printf("File '%s' found in %s.\n", filename, folder);
            printf(" Size: %d KB\n", filesystem[i].size);
            printf(" Created On: %s\n", filesystem[i].created);
            printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
            printf(" Deleted %s successfully.\n", filename);

            strcpy(loc, folder);
            while (1){                                                                  // decrement attributes of parents
                for(k = 0; k < nodecount; k++)
                    if (!(strcmp(filesystem[k].name, loc)))
                        break;
                if(k == nodecount)
                    break;
                else{
                    filesystem[k].size -= filesystem[i].size;                   // decrement total size of directory
                    filesystem[k].childcount--;       // decrement file count for directory
                    strcpy(loc, filesystem[k].location);
                }
            }

            printf(" Remaining file count inside %s: %d\n", filesystem[j].name, filesystem[j].childcount - 1);

            for (j = 0; j < i; j++) {                                                   // reassign links
                if (filesystem[j].childlink == filesystem[i].id){
                    filesystem[j].childlink = filesystem[i].siblinglink;
                    break;
                }
                if (filesystem[j].siblinglink == filesystem[i].id){
                    filesystem[j].siblinglink = filesystem[i].siblinglink;
                    break;
                }
            }
            nodecount--;
            for(j = i; j < nodecount; j++)                                          // delete the node (by overwriting)
                filesystem[j] = filesystem[j+1];
        }
        else if (choice == 6){                                      // delete a directory
            printf("Enter name of the directory to delete: ");
            scanf("%*c%[^\n]%*c", filename);
            printf("Available Locations:\tRoot");                   // root is also a possible location for the directory
            for(i = 0; i < nodecount; i++)
                if (filesystem[i].type == 'd')
                    printf("\t%s", filesystem[i].name);
            printf("\nEnter location of the directory to delete: ");
            scanf("%[^\n]%*c", folder);
            for(j = 0; j < nodecount; j++)                                              // check whether location exists
                if (filesystem[j].type == 'd' && !strcmp(filesystem[j].name, folder))
                    break;
            if(j == nodecount && strcmp(folder, "Root")){
                printf("%s doesn't exist.\n", folder);
                continue;
            }
            for(i = 0; i < nodecount; i++)                                              // check whether directory exists in the specified location
                if (filesystem[i].type == 'd' && !strcmp(filesystem[i].location, folder) && !strcmp(filesystem[i].name, filename))
                    break;
            if(i == nodecount){                                                         // directory not found
                printf("%s not found in %s.\n", filename, folder);
                continue;
            }
            printf("Directory '%s' found in %s.\n", filename, folder);
            printf(" Size: %d KB\n", filesystem[i].size);
            printf(" Child Count (including the directory itself): %d\n", filesystem[i].filecount);
            printf(" Created On: %s\n", filesystem[i].created);
            printf(" Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
            printf(" Deleted %s successfully.\n", folder);

            strcpy(loc, folder);
            while (1){                                                                  // decrement attributes of parents
                for(k = 0; k < nodecount; k++)
                    if (!(strcmp(filesystem[k].name, loc)))
                        break;
                if(k == nodecount)
                    break;
                else{
                    filesystem[k].size -= filesystem[i].size;                   // decrement total size of directory
                    filesystem[k].childcount -= filesystem[i].childcount;       // decrement file count for directory
                    strcpy(loc, filesystem[k].location);
                }
            }

            if(strcmp(folder, "Root")){                                                 // the location is compared with 'root' separately as root is not a defined directory
                printf(" Remaining file count inside %s: %d\n", filesystem[j].name, filesystem[j].childcount - 1);
            }
            for (j = 0; j < i; j++) {                                                   // reassign links
                if (filesystem[j].childlink == filesystem[i].id){
                    filesystem[j].childlink = filesystem[i].siblinglink;
                    break;
                }
                if (filesystem[j].siblinglink == filesystem[i].id){
                    filesystem[j].siblinglink = filesystem[i].siblinglink;
                    break;
                }
            }
            for(i = 0; i < nodecount; i++){                                             // delete by overwriting
                if(!strcmp(filesystem[i].location, filename) || (!strcmp(filesystem[i].name, filename) && filesystem[i].type == 'd')) {
                    for (j = i--; j < nodecount - 1; j++)
                        filesystem[j] = filesystem[j + 1];
                    nodecount--;
                }
            }
        }
        else{
            printf("Invalid Option.\n");
        }
    } while (1);

    for(i = 0; i < nodecount; i++){                                                     // write output to file
        fprintf(output, " ID #%d\n", filesystem[i].id);
        fprintf(output, " Name: %s\n", filesystem[i].name);
        fprintf(output, " Children Count: %d\n", filesystem[i].childcount);
        if(filesystem[i].childlink)
            fprintf(output, " Child Link: ID #%d\n", filesystem[i].childlink);
        if(filesystem[i].siblinglink)
            fprintf(output, " Sibling Link: ID #%d\n", filesystem[i].siblinglink);
        fprintf(output, " Location: %s\n", filesystem[i].location);
        fprintf(output, " Type: %s\n", filesystem[i].type=='f'?"File":"Directory");
        fprintf(output, " Size: %d KB\n", filesystem[i].size);
        fprintf(output, " Created On: %s\n", filesystem[i].created);
        fprintf(output, " Permissions: %s %s %s\n", filesystem[i].permissions&(1<<2)?"Read":"-", filesystem[i].permissions&(1<<1)?"Write":"-", filesystem[i].permissions&1?"Execute":"-");
        fprintf(output, "------------------------------\n");
    }
    fclose(output);
    printf("\n[.] Finished writing current state of filesystem to hierarchical_output.txt.\n\n");
}

int main(int argc, char const *argv[]) {
    int i = 1;

    if((argc == 1) || (argc % 2 == 0)){                              // display documentation if no input file or incorrect options specified
        printf("\nUSAGE: %s -OPTION INPUT_FILENAME [-OPTION(s) INPUT_FILENAME(s)]\n", argv[0]);

        printf("\nOPTIONS:\n");
        printf("       -s INPUT_FILENAME : Uses the Single Level Directory Structure\n");
        printf("       -t INPUT_FILENAME : Uses the Two-Level Directory Structure\n");
        printf("       -h INPUT_FILENAME : Uses the Hierarchical Directory Structure\n");
        printf("       (Example: %s -s single.txt -h hierarchical.txt)\n", argv[0]);

        printf("\nINPUT:\n");
        printf("   The input files provide the initial state of the filesystem.\n");
        printf("   Every directory and file is a node in the filesystem.\n\n");
        printf("   Each node is represented by 7 lines in the input files.\n");
        printf("   (Eg. If there are 3 initial nodes, there will be 21 lines in the input file.)\n");
        printf("   The first line is the name of the file/directory.\n");
        printf("   The second line is the file count (1 for files, 1 + files inside for directories).\n");
        printf("   The third line specifies the location of the file/directory.\n");
        printf("   The fourth line specifies the type of the node (f for files and d for directories).\n");
        printf("   The fifth line specifies the size of the node (1KB fixed for directories).\n");
        printf("   The sixth line is the date of creation of the file/directory.\n");
        printf("   The seventh line specifies the permissions for the node (r for read, w for write and x for execute).\n");
        printf("   (Eg. rwx, wx, rx, etc.)\n\n");
        printf("   For SINGLE LEVEL Directory Structure, the first node in the input file should be the root directory.\n");
        printf("   No additional directories can be created.\n");
        printf("   For TWO LEVEL Directory Structure, directories can only be created at the root,\n   and files only in directories.\n");

        printf("\nOUTPUT:\n   A text file for each type of directory structure used,\n   \"single_level_output.txt\", \"two_level_output.txt\" and \"hierarchical_output.txt\" respectively.\n   If the file doesn't exist, a new file will be automatically created.\n   If the file exists, the new output will overwrite the original file.\n\n");

        exit(0);
    }
    else{
        while(i < argc){
            switch (argv[i][1]) {
                case 's':   single_level(argv[i+1]);
                    break;
                case 't':   two_level(argv[i+1]);
                    break;
                case 'h':   hierarchical(argv[i+1]);
                    break;
                default : printf("\n[.] Invalid option.\n");
                    exit(1);
            }
            i += 2;
        }
    }


    return 0;
}
