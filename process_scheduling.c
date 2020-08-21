// Bugs So Far:
// SJF won't work if burst time is INT_MAX


#include <stdio.h>
#include <string.h>
#include <limits.h>
#define SIZE 6

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
    int i, j, temp;

    waiting[sorted[0]] = 0;
    turnaround[sorted[0]] = burst[sorted[0]];

    for(i = 1; i < n; i++){
        waiting[sorted[i]] = waiting[sorted[i-1]] + burst[sorted[i-1]] - (arrival[sorted[i]] - arrival[sorted[i-1]]);
        turnaround[sorted[i]] = waiting[sorted[i]] + burst[sorted[i]];
    }

    printf("SCHEDULING ALGORITHM - FIRST COME FIRST SERVE\n");
    printf("---------------------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);

}

void sjf(int arrival[], int burst[], int sorted[], int n){                          // shortest job first
    int pending[n];
    int turnaround[n];
    int waiting[n];
    int i, j, temp, current_time, shortest_index, shortest_burst;

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

    printf("SCHEDULING ALGORITHM - SHORTEST JOB FIRST\n");
    printf("-----------------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
}

void rr(int arrival[], int burst[], int sorted[], int n, int quantum){                          // round robin
    int turnaround[n];
    int waiting[n];
    int remaining_burst[n];
    int remaining_count = n;
    int i, j, current_time, idle;

    remaining_count = n;
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

    printf("SCHEDULING ALGORITHM - ROUND ROBIN\n");
    printf("----------------------------------\n");
    printf("Process  |  Turnaround Time  |  Waiting Time\n");
    for(i = 0; i < n; i++)
        printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
}

void p(){                          // priority


}

int main(int argc, char const *argv[]) {
    int arrival[SIZE] = {0, 1, 1, 6, 9, 9};
    int burst[SIZE] = {3, 33, 4, 5, 23, 4};
    int priority[SIZE] = {5, 3, 2, 4, 6, 1};
    int n = SIZE;
    int quantum = 3;                         // default time quantum
    int sorted[n];
    sort(arrival, n, sorted);
    //check if n is > 0
    rr(arrival, burst, sorted, n, quantum);
    return 0;
}
