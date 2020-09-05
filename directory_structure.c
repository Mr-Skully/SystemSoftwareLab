#include <stdio.h>                            // for basic i/o
#include <stdlib.h>                           // for exit()
#include <stdint.h>                           // for uint8_t
#include <string.h>                           // for strcmp()
#define MAX_NODES 10                          // maximum no. of nodes in the filesystem
#define DATE "05/09/2020"

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
    char permission[4], filename[20];
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

    while (!feof(input)){
        fgets(filesystem[nodecount].name, sizeof(filesystem[nodecount].name), input);
        filesystem[nodecount].name[strlen(filesystem[nodecount].name) - 1] = '\0';
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
        if(filesystem[nodecount].type == 'f' || filesystem[nodecount].type == 'F')
            filesystem[nodecount - 1].filelink = filesystem[nodecount].id;
        nodecount++;
    }
    fclose(input);

    printf("\nTwo Level Directory Structure\n-----------------------------\n");







    for(i = 0; i < nodecount; i++){
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
    char permission[4], filename[20];
    int i, j, choice = 1, flag = 0, last_id = 0;

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

    while (!feof(input)){
        fgets(filesystem[nodecount].name, sizeof(filesystem[nodecount].name), input);
        filesystem[nodecount].name[strlen(filesystem[nodecount].name) - 1] = '\0';
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
        if(filesystem[nodecount].type == 'f' || filesystem[nodecount].type == 'F')
            filesystem[nodecount - 1].filelink = filesystem[nodecount].id;
        nodecount++;
    }
    fclose(input);

    printf("\nHierarchical Directory Structure\n-----------------------------\n");











    for(i = 0; i < nodecount; i++){
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
    printf("\n[.] Finished writing current state of filesystem to hierarchical_output.txt.\n\n");
}

int main(int argc, char const *argv[]) {
    int i = 1;



    if((argc == 1) || (argc % 2 == 0)){                              // display documentation if no input file or incorrect options specified
        printf("\nUSAGE: %s -OPTION INPUT_FILENAME [-OPTION(s) INPUT_FILENAME(s)]\n", argv[0]);

        printf("\nOPTIONS:\n");
        printf("       -s INPUT_FILENAME : Uses the Single Level Directory Structure\n");
        printf("       -h INPUT_FILENAME : Uses the Two-Level Directory Structure\n");
        printf("       -h INPUT_FILENAME : Uses the Hierarchical Directory Structure\n");
        printf("       (Example: %s -s onelevelinput.txt -h hierarchicalinput.txt)\n", argv[0]);
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
