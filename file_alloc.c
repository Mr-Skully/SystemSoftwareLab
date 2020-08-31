#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXSIZE 20        // no. of files waiting for allocation

FILE *output;

void sequential(char path[]){
    int BLOCKS, i, j, k, n, r = 0;

    FILE *input = fopen(path, "r");
    if(!input){
        printf("\n[.] Could not read %s.\n\n", path);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", path);

    fscanf(input, "%d", &BLOCKS);
    fscanf(input, "%d", &n);

    struct queue_element{                                     // queue of files waiting to be allocated
        int start;
        int length;
        char * contents;
        int allocated;
    } queue[n];

    for(i = 0; i < n; i++)
        fscanf(input, "%d %d %s", &queue[i].start, &queue[i].length, queue[i].contents);

    int requests[BLOCKS];
    while(fscanf(input, "%d", &requests[r]))
        r++;

    char blocks[BLOCKS];                                       // memory blocks
    for(i = 0; i < BLOCKS; blocks[i++] = 0);                // initializing blocks to zero, to denote unallocated blocks

    for(i = 0; i < n; i++) {
        for(j = queue[i].start - 1; j < (queue[i].start + queue[i].length - 1) && j < BLOCKS && blocks[j] == 0; j++);
        if(j == (queue[i].start + queue[i].length - 1)){           // checks whether the required memory is free
            for(k = 0; k < queue[i].length; k++)               // writing string to memory
                blocks[queue[i].start - 1 + k] = queue[i].contents[k];
            queue[i].allocated++;
        }
    }

    for(i = 0; i < n; i++)
        printf("%d\n", queue[i].allocated);
    for(i = 0; i < BLOCKS; i++){
        if(blocks[i])
            printf("%c ", blocks[i]);
        else
            printf("- ");
    }
}

void linkedlist(char path[]){
    int BLOCKS, i, j, k, x, n = 4;

    FILE *input = fopen(path, "r");
    if(!input){
        printf("\n[.] Could not read %s.\n\n", path);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", path);

    fscanf(input, "%d", &BLOCKS);

    struct queue_element{                                     // queue of files waiting to be allocated
        int length;
        char * contents;
        int allocated;
        int start;
        int end;
    } queue[MAXSIZE] = {
            {3, "abc", 0, 0, 0},
            {1, "x", 0, 0, 0},
            {2, "yz", 0, 0, 0},
            {3, "pqr", 0, 0, 0}
    };

    char blocks[BLOCKS];                                       // memory blocks
    for(i = 0; i < BLOCKS; blocks[i++] = 0);                // initializing blocks to zero, to denote unallocated blocks

    int links[BLOCKS];        // links between nodes
    int current = 0;

    for(i = 0; i < n; i++){
        if(current == -1)                                      // stop when free nodes are exhausted
            break;
        // check whether memory is available for allocation
        for(j = current, k = 0; (k < queue[i].length) && !blocks[j]; j = links[j] - 1, k++){
            if(!links[j]){                                     // check whether next node exists
                k++;
                break;
            }
        }

        if(k == queue[i].length){                              // write to memory if memory is available
            queue[i].start = current + 1;
            queue[i].allocated++;
            for(j = current, k = 0; k < queue[i].length; j = links[j] - 1, k++){
                blocks[j] = queue[i].contents[k];
                if(k == queue[i].length - 1)
                    queue[i].end = j + 1;
            }
            current = j;
        }

    }

    for(i = 0; i < n; i++)
        printf("%d %d %d\n", queue[i].start, queue[i].end, queue[i].allocated);
    for(i = 0; i < BLOCKS; i++){
        if(blocks[i])
            printf("%c ", blocks[i]);
        else
            printf("- ");
    }

}

void indexed(char path[]){
    int BLOCKS, i, j, k, n = 4;

    FILE *input = fopen(path, "r");
    if(!input){
        printf("\n[.] Could not read %s.\n\n", path);
        exit(1);
    }
    printf("\n[.] Reading %s.\n", path);

    fscanf(input, "%d", &BLOCKS);

    struct queue_element{                                     // queue of files waiting to be allocated
        int length;
        int index;
        char * contents;
        int allocated;
    } queue[MAXSIZE] = {
            {3, 7, "abc", 0},
            {1, 8, "x", 0},
            {2, 3, "yz", 0},
            {3, 4, "pqr", 0}
    };

    char blocks[BLOCKS];                                       // memory blocks
    for(i = 0; i < BLOCKS; blocks[i++] = 0);                // initializing blocks to zero, to denote unallocated blocks
    int current = 0;

    int index_table[BLOCKS];
    for(i = 0; i < BLOCKS; index_table[i++] = 0);

    for(i = 0; i < n; i++) {
        if(!blocks[queue[i].index - 1] && !index_table[queue[i].index - 1]){
            index_table[queue[i].index - 1]--;
            for(j = current, k = 0; k < queue[i].length && j < BLOCKS; k++, j++)
                    if(blocks[j] || index_table[j])
                        k--;
            if(k == queue[i].length){
                for(j = current, k = 0; k < queue[i].length; k++, j++) {
                    if(blocks[j] || index_table[j])
                        k--;
                    else {
                        blocks[j] = queue[i].contents[k];
                        index_table[j] = i + 1;
                    }
                }
                queue[i].allocated++;
                current = j;
            }
            else
                index_table[queue[i].index - 1]++;
        }
    }
    for(i = 0; i < n; i++)
        printf("%d\n", queue[i].allocated);
    for(i = 0; i < BLOCKS; i++)
        if(blocks[i])
            printf("%c ", blocks[i]);
        else
            printf("- ");

    for(i = 0; i < BLOCKS; i++)
            printf("%d ", index_table[i]);

}

int main(int argc, char const *argv[]) {

    int seq_algo = 0, ll_algo = 0, index_algo = 0;
    int i = 1;

    output = fopen(".\\file_alloc.txt", "a");                        // open file for writing output
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }

    if((argc == 1) || (argc % 2 == 0)){                                                       // display documentation if no input file and options specified
        printf("\nUSAGE: %s -OPTION INPUT_FILENAME [-OPTION(s) INPUT_FILENAME(s)]\n", argv[0]);

        printf("\nOPTIONS:\n");
        printf("       -s INPUT_FILENAME : Uses the Sequential File Allocation Strategy\n");
        printf("       -l INPUT_FILENAME : Uses the Linked List File Allocation Strategy\n");
        printf("       -i INPUT_FILENAME : Uses the Indexed File Allocation Strategy\n");
        printf("       (Example: %s -l linkedlistinput.txt -s seqinput.txt)\n", argv[0]);

        printf("\nINPUT:\n   For Sequential File Allocation Strategy,");
        printf("\n   The first line contains an integer which denotes the total number of blocks available.");
        printf("\n   The second line contains an integer 'n' which denotes the number of files that are in queue to be allotted. ");
        printf("\n   The next 'n' lines contain two integers and a string each (with the 3 components being space separated),");
        printf("\n   which denotes the starting address for file allocation, the length of the file and its contents respectively. (Eg. 1 3 abc)");
        printf("\n   The blocks whose contents are to be displayed make up the remaining lines, with each block number occupying its own line.\n");

        printf("\n   For Linked List File Allocation Strategy,");
        printf("\n   The first line contains an integer which denotes the total number of blocks available.");
        printf("\n   The second line contains an integer 'k' which denotes the total number of links that are present between the nodes.");
        printf("\n   The next 'k' lines contain two space separated integers each which denotes the origin and destination nodes of each of the 'k' links.");
        printf("\n   The next line contains an integer 'n' which denotes the number of files that are in queue to be allotted,");
        printf("\n   followed by 'n' lines containing an integer and a string on each line, denoting the file lengths and contents. (Eg. 3 abc)");
        printf("\n   The processes whose allocation are to be displayed make up the remaining lines, with each process number occupying its own line.\n");

        printf("\n   For Indexed File Allocation Strategy,");
        printf("\n   The first line contains an integer which denotes the total number of blocks available.");
        printf("\n   The second line contains an integer 'n' which denotes the number of files that are in queue to be allotted.");
        printf("\n   The next 'n' lines contain two integers and a string each (with the 3 components being space separated),");
        printf("\n   which denotes the length of the file, the index node and its contents respectively. (Eg. 3 7 abc)");
        printf("\n   The index nodes of the files whose allocation is to be displayed make up the remaining lines,");
        printf("\n   with each index node number occupying its own line.\n");

        printf("\nOUTPUT:\n   A text file, \"file_alloc.txt\".\n   If the file doesn't exist, a new file will be automatically created.\n   If the file exists, the new output will be appended to the original file.\n\n");
        exit(0);
    }
    else{
        while(i < argc){
            switch (argv[i][1]) {
                case 's':   sequential(argv[i+1]);
                            break;
                case 'l':   linkedlist(argv[i+1]);
                            break;
                case 'i':   indexed(argv[i+1]);
                            break;
                default : printf("\n[.] Invalid option.\n");
                          exit(1);
            }
            i += 2;
        }
    }


    printf("\n[.] Writing to file_alloc.txt.\n");
    fclose(output);
    printf("\n[.] Finished writing to file_alloc.txt.\n\n");
    return 0;
}