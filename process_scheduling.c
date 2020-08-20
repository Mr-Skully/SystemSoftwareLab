// Bugs So Far:
// SJF stops if the CPU is idle in before executing all processes
// SJF won't work if burst time is INT_MAX


#include <stdio.h>
#include <limits.h>
#include <string.h>
#define SIZE 6

void fcfs(int arrival[], int burst[], int n){                          // first come first serve
  int sorted[n];
  int turnaround[n];
  int waiting[n];
  int i, j, temp;

  for(i = 0; i < n; i++)
    sorted[i] = i;
  for(i = 0; i < n - 1; i++){
    for(j = 0; j < n - 1 - i; j++){
      if(arrival[sorted[j]] > arrival[sorted[j+1]]){
        temp = sorted[j];
        sorted[j] = sorted[j+1];
        sorted[j+1] = temp;
      }
    }
  }

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

void sjf(int arrival[], int burst[], int n){                          // shortest job first
  int sorted[n];
  int pending[n];
  int turnaround[n];
  int waiting[n];
  int i, j, temp, current_time, shortest_index, shortest_burst;

  for(i = 0; i < n; i++)
    sorted[i] = i;
  for(i = 0; i < n; i++)
    pending[i] = 1;

  for(i = 0; i < n - 1; i++){
    for(j = 0; j < n - 1 - i; j++){
      if(arrival[sorted[j]] > arrival[sorted[j+1]]){
        temp = sorted[j];
        sorted[j] = sorted[j+1];
        sorted[j+1] = temp;
      }
    }
  }

  current_time = arrival[sorted[0]];
  for(i = 0; i < n; i++){
    shortest_burst = INT_MAX;
    for(j = 0; j < n && arrival[sorted[j]] <= current_time; j++){
      if(pending[sorted[j]] && burst[sorted[j]] < shortest_burst){
        shortest_burst = burst[sorted[j]];
        shortest_index = sorted[j];
      waiting[shortest_index] = current_time - arrival[shortest_index];
      current_time += burst[shortest_index];
      turnaround[shortest_index] = current_time - arrival[shortest_index]
      }
    }
  }

  printf("SCHEDULING ALGORITHM - SHORTEST JOB FIRST\n");
  printf("-----------------------------------------\n");
  printf("Process  |  Turnaround Time  |  Waiting Time\n");
  for(i = 0; i < n; i++)
    printf("  %-8d       %-13d      %d\n", i+1, turnaround[i], waiting[i]);
}

void rr(){                          // round robin

}

void p(){                          // priority

}

int main(int argc, char const *argv[]) {
  int arrival[SIZE] = {0, 1, 1, 6, 9, 9};
  int burst[SIZE] = {3, 33, 4, 5, 23, 4};
  int priority[SIZE] = {5, 3, 2, 4, 6, 1};
  int n = SIZE;
  //check if n is > 0
  fcfs(arrival, burst, n);
  return 0;
}
