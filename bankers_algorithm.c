#include <stdio.h>                              // for basic i/o
#include <stdlib.h>                             // for exit()

#define A 65                                    // ASCII value for A
#define ResourceTypesCount 3                    // types of resources in system
#define MaxProcesses 10                         // maximum possible number of processes

int sequence[MaxProcesses];                     // array to store the safe sequence

void safety_check(int need[][ResourceTypesCount], int allocated[][ResourceTypesCount], int available[], int n){              // perform safety check for current state
    int i, j, k = 0;
    int finished[n];
    int work[ResourceTypesCount];
    for(i = 0; i < n; i++)                                          // reset safe sequence array
        sequence[i] = 0;
    for(i = 0; i < n; i++)                                          // initializing FINISHED array to zero
        finished[i] = 0;
    for(i = 0; i < ResourceTypesCount; i++)                         // initializing WORK array to AVAILABLE
        work[i] = available[i];

    while(1) {
        for (i = 0; i < n; i++) {
            if(finished[i])
                continue;
            for(j = 0; j < ResourceTypesCount; j++){
                if(need[i][j] > work[j])
                    break;
            }
            if (j < ResourceTypesCount)
                continue;
            sequence[k++] = i;
            for(j = 0; j < ResourceTypesCount; j++)
                work[j] += allocated[i][j];
            finished[i]++;
            break;
        }
        if (i == n)
            return;
    }
}

void print_need(int need[][ResourceTypesCount], int n){                                 // print NEED matrix
    int i, j;
    printf("\n----- NEED -----\n");
    for(i = 0; i < ResourceTypesCount; i++)
        printf("  %c   ", A + i);
    for(i = 0; i < n; i++){
        printf("\n");
        for(j = 0; j < ResourceTypesCount; j++)
            printf("  %-4d", need[i][j]);
    }
    printf("\n");
}

void print_request_status(int n, int safe){                                                // print safe sequence or reject message
    if(!safe){                                                      // request rejected
        printf("\nRequest Denied.\n----------------\n");
        return;
    }

    int i = 0;                                                      // print safe sequence
    printf("\nSafe Sequence: ");
    for(;i < n - 1; i++)
        printf("P%d -> ", sequence[i]);
    printf("P%d\n----------------\n");
}

int main(int argc, char const *argv[]) {
    int i, j, n, r, valid;
    int maximum[MaxProcesses][ResourceTypesCount],                  // maximum resource requirements
        allocated[MaxProcesses][ResourceTypesCount],                // resource instances already acquired by processes
        available[ResourceTypesCount],                              // resource instances remaining (free/available)
        need[MaxProcesses][ResourceTypesCount],                     // resources still needed by each process
        request[MaxProcesses][ResourceTypesCount + 1];              // resource allocation requests, where the 4th column represents the process number (numbering starts from 0)

    FILE *input, *output;                                           // file pointers
//    output = fopen(".\\bankers_algo_output.txt", "a");            // open file for writing output
//    if(!output){
//        printf("\n[.] Could not open output file.\n");
//        exit(1);
//    }

    if(argc != 2) {                              // display documentation if no input file is specified
        printf("\nUSAGE: %s INPUT_FILENAME\n", argv[0]);

        printf("\nINPUT:\n   A text file.");
        printf("\n   The first line contains a positive integer 'n',");
        printf("\n   the total number of processes present.");
        printf("\n   The next 'n' lines contains %d space separated integers on each line,", ResourceTypesCount);
        printf("\n   denoting the maximum resource requirements of the 'n' processes.");
        printf("\n   The next 'n' lines contains %d space separated integers on each line,", ResourceTypesCount);
        printf("\n   representing the resources already allotted for the processes.");
        printf("\n   The next line contains %d space separated integers,", ResourceTypesCount);
        printf("\n   denoting the resource instances remaining after allocation.");
        printf("\n   The next line contains a positive integer 'r',");
        printf("\n   which denotes the number of process requests to be checked.");
        printf("\n   The next 'r' lines contain %d space separated integers, where the 1st integer ", ResourceTypesCount + 1);
        printf("\n   represents the process number (numbering starts from 0) and the remaining");
        printf("\n   %d integers represent the number of resource instances requested.\n");

        printf("\nOUTPUT:\n   A text file, \"bankers_algo_output.txt\".\n   If the file doesn't exist, a new file will be automatically created.\n   If the file exists, the new output will overwrite the original file.\n\n");
        exit(0);
    }

    input = fopen(argv[1], "r");                                    // open input file
    if(!input){                                                     // if unable to open file
        printf("\n[.] Could not open %s.\n\n", argv[1]);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", argv[1]);

    fscanf(input, "%d", &n);                                        // read no. of processes
    for(i = 0; i < n; i++)                                          // read MAXIMUM matrix
        for(j = 0; j < ResourceTypesCount; j++)
            fscanf(input, "%d", &maximum[i][j]);
    for(i = 0; i < n; i++)                                          // read ALLOCATED matrix
        for(j = 0; j < ResourceTypesCount; j++)
            fscanf(input, "%d", &allocated[i][j]);
    for(j = 0; j < ResourceTypesCount; j++)                         // read AVAILABLE matrix
        fscanf(input, "%d", &available[j]);
    fscanf(input, "%d", &r);                                        // read no. of requests
    for(i = 0; i < r; i++){                                         // read REQUEST matrix
        fscanf(input, "%d", &request[i][ResourceTypesCount]);
        for(j = 0; j < ResourceTypesCount; j++)
            fscanf(input, "%d", &request[i][j]);
    }

    for(i = 0; i < n; i++)                                          // calculate NEED matrix
        for(j = 0; j < ResourceTypesCount; j++)
            need[i][j] = maximum[i][j] - allocated[i][j];

    printf("\n[.] Using Banker's algorithm for deadlock detection.\n");

    printf("----------------\n\nINITIAL STATE:\n");
    safety_check(need, allocated, available, n);                               // run the safety check algorithm for the initial state
    print_need(need, n);                                            // print NEED matrix initially
    if(sequence[n-1])                                               // system is safe initially
        print_request_status(n, 1);
    else{                                                           // system is in deadlock initially
        printf("\n[.] Deadlocked input provided. All requests denied. Terminating...\n\n");
        exit(1);
    }
    printf("\nREQUESTS:\n");

    for(i = 0; i < r; i++){                                         // perform BANKER'S ALGORITHM for each request
        valid = 1;
        for(j = 0; j < ResourceTypesCount; j++)                     // to check if request > need
            if(request[i][j] > need[request[i][ResourceTypesCount]][j]){
                valid = 0;
                break;
            }
        if(!valid){                                                 // if request > need
            print_need(need, n);
            print_request_status(0, 0);
            continue;
        }

        valid = 1;
        for(j = 0; j < ResourceTypesCount; j++)                     // to check if request > available
            if(request[i][j] > available[j]){
                valid = 0;
                break;
            }
        if(!valid){                                                 // if request > available
            print_need(need, n);
            print_request_status(0, 0);
            continue;
        }

        for(j = 0; j < ResourceTypesCount; j++){                    // allocate resources for the process
            need[request[i][ResourceTypesCount]][j] -= request[i][j];
            available[j] -= request[i][j];
            allocated[request[i][ResourceTypesCount]][j] += request[i][j];
        }
        safety_check(need, allocated, available, n);                           // check if the system is in safe state after servicing the request
        print_need(need, n);                                        // print NEED matrix and safe sequence (if any)
        print_request_status(n, sequence[ResourceTypesCount]?1:0);
        for(j = 0; j < ResourceTypesCount; j++){                    // undo the changes to NEED, ALLOCATION and AVAILABLE
            need[request[i][ResourceTypesCount]][j] += request[i][j];
            available[j] += request[i][j];
            allocated[request[i][ResourceTypesCount]][j] -= request[i][j];
        }
    }

//    fclose(output);
//    printf("\n[.] Writing output to bankers_algo_output.txt.\n");
//    printf("\n[.] DONE\n\n");
}
