// Bugs So Far:
// SJF won't work if burst time is INT_MAX


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define SIZE 50

FILE *output;

void sort(int key[], int n, int sorted[]){
    int i, j, temp;
    for(i = 0; i < n; i++)
        sorted[i] = i;
    for(i = 0; i < n - 1; i++){
        for(j = 0; j < n - 1 - i; j++){
            if(key[sorted[j]] > key[sorted[j+1]]){
                temp = sorted[j];
                sorted[j] = sorted[j+1];
                sorted[j+1] = temp;
            }
        }
    }
}

void fcfs(int arrival[], int burst[], int sorted[], int n){                          // first come first serve
    int turnaround[n];
    int waiting[n];
    int i, j;

    waiting[sorted[0]] = 0;
    turnaround[sorted[0]] = burst[sorted[0]];

    for(i = 1; i < n; i++){
        waiting[sorted[i]] = waiting[sorted[i-1]] + burst[sorted[i-1]] - (arrival[sorted[i]] - arrival[sorted[i-1]]);
        turnaround[sorted[i]] = waiting[sorted[i]] + burst[sorted[i]];
    }
    printf("\n[.] Process scheduling performed using First Come First Serve Algorithm.\n");
    printf("SCHEDULING ALGORITHM - FIRST COME FIRST SERVE\n");
    printf("---------------------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
    printf("____________________________________________\n\n");


}

void sjf(int arrival[], int burst[], int sorted[], int n){                          // shortest job first
    int pending[n];
    int turnaround[n];
    int waiting[n];
    int i, j, current_time, shortest_index, shortest_burst;

    for(i = 0; i < n; i++)
        pending[i] = 1;

    current_time = arrival[sorted[0]];

    for(i = 0; i < n; i++){
        shortest_burst = INT_MAX;
        for(j = 0; j < n && arrival[sorted[j]] <= current_time; j++){
            if(pending[sorted[j]] && burst[sorted[j]] < shortest_burst){
                shortest_burst = burst[sorted[j]];
                shortest_index = sorted[j];
            }
        }
        if(shortest_burst == INT_MAX)
            current_time = arrival[sorted[i--]];
        else{
            waiting[shortest_index] = current_time - arrival[shortest_index];
            current_time += burst[shortest_index];
            turnaround[shortest_index] = current_time - arrival[shortest_index];
            pending[shortest_index] = 0;
        }
    }

    printf("\n[.] Process scheduling performed using Shortest Job First Algorithm.\n");
    printf("SCHEDULING ALGORITHM - SHORTEST JOB FIRST\n");
    printf("--------------------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
    printf("____________________________________________\n\n");

}

void rr(int arrival[], int burst[], int sorted[], int n, int quantum){                          // round robin
    int turnaround[n];
    int waiting[n];
    int remaining_burst[n];
    int remaining_count = n;
    int i, j, current_time, idle;

    for(i = 0; i < n; i++)
        remaining_burst[i] = burst[i];

    // finding starting time
    current_time = arrival[sorted[0]];

    i = 0;
    idle = 0;
    while(remaining_count){
        if(remaining_burst[i] > quantum && arrival[i] <= current_time){
            current_time += quantum;
            remaining_burst[i] -= quantum;
            if(idle)
                idle--;
        }
        else if(remaining_burst[i] && (arrival[i] <= current_time)){
            current_time += remaining_burst[i];
            remaining_burst[i] = 0;
            remaining_count--;
            turnaround[i] = current_time - arrival[i];
            waiting[i] = turnaround[i] - burst[i];
            if(idle)
                idle--;
        }
        i = (i + 1) % n;
        if(i == 0){
            if(!idle)
                idle++;
            else{
                for(j = 0; j < n; j++){
                    if(remaining_burst[sorted[j]]){
                        current_time = arrival[sorted[j]];
                        break;
                    }
                }
            }
        }

    }

    printf("\n[.] Process scheduling performed using Round Robin Scheduling Algorithm.\n");
    printf("SCHEDULING ALGORITHM - ROUND ROBIN\n");
    printf("--------------------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
    printf("____________________________________________\n\n");

}

void p(int arrival[], int burst[], int priority[], int sorted[], int n){                          // priority
    int turnaround[n], waiting[n], pending[n];
    int i, j, current_time, prio_index;

    for(i = 0; i < n; i++)
        pending[i] = 1;

    current_time = arrival[sorted[0]];
    for(i = 0; i < n; i++){
        // find first pending process that has already arrived
        for(j = 0; j < n; j++)
            if(pending[sorted[j]] && (arrival[sorted[j]] <= current_time)){
                prio_index = j;
                break;
            }
        if(j == n){
            current_time = arrival[sorted[i]];
            prio_index = i;
        }
        for(j = 0; j < n; j++){
            if(pending[sorted[j]] && (priority[sorted[j]] < priority[sorted[prio_index]]) && (arrival[sorted[j]] <= current_time))
                prio_index = j;
        }
        pending[sorted[prio_index]]--;
        waiting[sorted[prio_index]] = current_time - arrival[sorted[prio_index]];
        current_time += burst[sorted[prio_index]];
        turnaround[sorted[prio_index]] = current_time - arrival[sorted[prio_index]];
    }

    printf("\n[.] Process scheduling performed using Priority Scheduling Algorithm.\n");
    printf("SCHEDULING ALGORITHM - PRIORITY\n");
    printf("--------------------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
    printf("____________________________________________\n\n");

}

int main(int argc, char const *argv[]) {

    int fcfs_algo = 0, sjf_algo = 0, p_algo = 0, rr_algo = 0;
    int i = 2;
    int n = 0;
    int quantum = 1;                         // default time quantum
    char path[100] = ".\\";
    int arrival[SIZE], burst[SIZE], priority[SIZE];

    if(argc == 1){
        printf("\nUSAGE: %s INPUT_FILENAME [option(s)]\n", argv[0]);
        printf("\nOPTIONS:\n");
        printf("       -fcfs             : Uses First Come First Serve Algorithm (used by default when no option is specified)\n");
        printf("       -sjf              : Uses Shortest Job First Algorithm (Non-Preemptive)\n");
        printf("       -p                : Uses Priority Scheduling Algorithm (Non-Preemptive)\n");
        printf("       -rr TIME_QUANTUM  : Uses Round Robin Algorithm (Preemptive)\n");
        printf("       (Example: %s input.txt -fcfs -rr 3 -p)\n", argv[0]);
        printf("\nINPUT:\n   A text file.\n   Each line represents a single process.\n   Each line contains three 'space-seperated' integers.\n   The arrival time is the first integer, burst time is the second integer and priority is the third.\n   Even if priority scheduling is not used, specify arbitrary priorities for each process.\n   (Eg. 0 can be used as the priority for all the processes.)\n");
        printf("\nOUTPUT:\n   A text file, \"proc_schedule.txt\".\n   If the file doesn't exist, a new file will be automatically created.\n   If the file exists, the new output will be appended to the original file.\n\n");
        exit(0);
    }
    else if(argc == 2){
        strcat(path, argv[1]);
        fcfs_algo++;
    }
    else{
        strcat(path, argv[1]);
        while(i < argc){
            if(!strcmp(argv[i], "-fcfs"))
                fcfs_algo++;
            else if(!strcmp(argv[i], "-sjf"))
                sjf_algo++;
            else if(!strcmp(argv[i], "-p"))
                p_algo++;
            else if(!strcmp(argv[i], "-rr")){
                rr_algo++;
                if((i+1 < argc) && atoi(argv[i+1])){
                    quantum = atoi(argv[i+1]);
                    i++;
                }
            }
            else{
              printf("\n[.] Invalid option.\n");
              exit(0);
            }
            i++;
        }
    }


    FILE *input;
    input = fopen(path, "r");
    if(!input){
        printf("\n[.] Could not open input file.\n");
        exit(1);
    }
    printf("\n[.] Reading %s.\n", argv[1]);

    while(fscanf(input, "%d %d %d", &arrival[n], &burst[n], &priority[n]) == 3)
        n++;

    printf("\n[.] Finished reading %s.\n", argv[1]);
    fclose(input);

    if(!n){
        printf("\n[.] Input file is empty.\n");
        exit(1);
    }


    output = fopen(".\\proc_schedule.txt", "a");
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }
    printf("\n[.] Writing to proc_schedule.txt.\n", );

    int sorted[n];
    sort(arrival, n, sorted);

    if(fcfs_algo)
        fcfs(arrival, burst, sorted, n);
    if(sjf_algo)
        sjf(arrival, burst, sorted, n);
    if(rr_algo)
        rr(arrival, burst, sorted, n, quantum);
    if(p_algo)
        p(arrival, burst, priority, sorted, n);

    fclose(output);
    printf("\n[.] Finished writing to proc_schedule.txt.\n", );

    return 0;
}
