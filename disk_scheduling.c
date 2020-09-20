#include <stdio.h>                                                  // for basic I/O
#include <stdlib.h>                                                 // for exit()

#define MAX_REQUESTS 20                                             // maximum no. of disk-op requests
#define diff(a, b) (a>b)?(a-b):(b-a)                                // macro function to find difference

void sort(int array[], int size){                                   // bubble sort
    int i, j, temp;
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - 1 - i; ++j) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

int fcfs(int start, int end, int current, int requests[], int r){   // calculate head movements while using FCFS algorithm
    int i;
    int moves = 0;
    for(i = 0; i < r; i++){                                         // go through each request
        if(requests[i] > end || requests[i] < start){               // request is out of limits
            printf("\n[.] Request is out of bounds.\n");
            exit(1);
        }
        moves += diff(requests[i], current);                     // add the difference in sector indices to total head movements
        current = requests[i];                                      // update the current position
    }
    return moves;
}

int scan(int start, int end, int current, int direction, int requests[], int r){        // calculate head movements while using SCAN algorithm
    if(current < requests[0]){                                      // every request is to the right of the current position
        if (direction)                                                                  // disk head is moving right
            return diff(current, requests[r-1]);
        return (diff(current, start)) + (diff(start, requests[r-1]));                // disk head is moving left
    }
    else if(current > requests[r-1]){                               // every request is to the left of the current position
        if (direction)                                                                  // disk head is moving right
            return (diff(current, end)) + (diff(end, requests[0]));
        return diff(current, requests[0]);                                           // disk head is moving left
    }
    else{                                                           // requests are present on both sides of the current position
        if (direction)                                                                  // disk head is moving right
            return (diff(current, end)) + (diff(end, requests[0]));
        return (diff(current, start)) + (diff(start, requests[r-1]));                // disk head is moving left
    }
}

int cscan(int start, int end, int current, int direction, int requests[], int r){       // calculate head movements while using C-SCAN algorithm
    int i;
    if(current < requests[0]){                                      // every request is to the right of the current position
        if (direction)                                                                  // disk head is moving right
            return diff(current, requests[r-1]);
        return (diff(current, start)) + (diff(start, end)) + (diff(end, requests[0]));// disk head is moving left
    }
    else if(current > requests[r-1]){                               // every request is to the left of the current position
        if (direction)                                                                  // disk head is moving right
            return (diff(current, end)) + (diff(start, end)) + (diff(start, requests[r-1]));
        return diff(current, requests[0]);                                           // disk head is moving left
    }
    else{                                                           // requests are present on both sides of the current position
        for (i = 0; i < r; i++)                                     // to find the relative position of the current location of the disk head w.r.t. the requests
            if (requests[i] > current)
                break;
        if (direction)                                                                  // disk head is moving right
            return (diff(current, end)) + (diff(start, end)) + (diff(start, requests[i-1]));
        return (diff(current, start)) + (diff(start, end)) + (diff(end, requests[i]));// disk head is moving left
    }
}

int main(int argc, char const *argv[]) {
    int current, start, end;                                        // parameters of the disks
    int r = 0;                                                      // no. of requests
    char dirn;                                                      // temporary variable to hold the direction of head movement while being read from file
    int direction = 1;                                              // 0 - left, 1 - right
    int requests[MAX_REQUESTS];                                     // array to hold disk-op requests

    FILE *input, *output;                                           // file pointers

    if(argc != 2) {                                                 // display documentation if no input file is specified
        printf("\nUSAGE: %s INPUT_FILENAME\n", argv[0]);

        printf("\nINPUT:\n   A text file.");
        printf("\n   The first line contains a positive integer 'c',");
        printf("\n   which denotes (the index of) the current head position.");
        printf("\n   The second line contains the letter 'l' or 'r', representing");
        printf("\n   the current direction of motion of the disk head as left or right.");
        printf("\n   The next line contains 2 space separated integers,");
        printf("\n   denoting the starting and ending index of the sectors respectively.");
        printf("\n   The remaining lines contain one integer each, with each integer");
        printf("\n   representing (the sector number/index of) a disk I/O request.");

        printf("\nOUTPUT:\n   A text file, \"ds_output.txt\".\n   If the file doesn't exist, a new file will be automatically created.\n   If the file exists, the new output will be appended the original file.\n\n");
        exit(0);
    }

    input = fopen(argv[1], "r");                                    // open input file
    if(!input){                                                     // if unable to open file
        printf("\n[.] Could not open %s.\n\n", argv[1]);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", argv[1]);

    output = fopen(".\\ds_output.txt", "a");                        // open file for writing output
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }

    fscanf(input, "%d", &current);                                  // read the current head position
    fscanf(input, "%*c%c", &dirn);                                  // read direction of disk head movement
    if (dirn == 'l')
        direction = 0;
    fscanf(input, "%d %d", &start, &end);                           // read the starting and ending index for the sectors
    while(!feof(input)){
        fscanf(input, "%d", &requests[r++]);                        // read requests
    }
    fclose(input);

    fprintf(output, "\n-----------------------------\n");

    printf("\n[.] Using FCFS algorithm for disk scheduling.\n");
    fprintf(output, "FCFS Disk Scheduling Algorithm:\n");
    fprintf(output, "   Total Head Movements - %d\n", fcfs(start, end, current, requests, r));

    sort(requests, r);
    printf("\n[.] Using SCAN algorithm for disk scheduling.\n");
    fprintf(output, "SCAN Disk Scheduling Algorithm:\n");
    fprintf(output, "   Total Head Movements - %d\n", scan(start, end, current, direction, requests, r));

    printf("\n[.] Using C-SCAN algorithm for disk scheduling.\n");
    fprintf(output, "C-SCAN Disk Scheduling Algorithm:\n");
    fprintf(output, "   Total Head Movements - %d\n", cscan(start, end, current, direction, requests, r));

    fprintf(output, "-----------------------------\n");

    fclose(output);
    printf("\n[.] Writing output to ds_output.txt.\n");
    printf("\n[.] DONE\n\n");
}
