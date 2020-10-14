#include <pthread.h>                                                // for creating threads
#include <semaphore.h>                                              // for creating semaphores
#include <stdio.h>                                                  // for standard I/O
#include <stdlib.h>                                                 // for exit()
#include <time.h>                                                   // for time()
#include <windows.h>                                                // for Sleep(microseconds) in Windows; use unistd.h for sleep(seconds) in Linux
#define BITES 3                                                     // the number of bites it takes to finish a bowl of spaghetti
#define PHILOSOPHERS 5

sem_t chopstick[PHILOSOPHERS];                                      // define the chopsticks (semaphores)

void * phil(void * num){                                   // the philosopher thread
    int i;                                                          // variable for counting
    int id = (int)num + 1;                                          // to identify the philosophers
    int right = (int)num;                                           // the right chopstick
    int left = ((int)num+1) % PHILOSOPHERS;                         // the left chopstick
    clock_t wait_start, wait_end;                                   // clock variables to measure waiting time
    double waiting_time;                                            // to store waiting time
    srand(time(0));                                                 // seed rand() with current time

    for(i = 1; i <= BITES; i++) {                          // run until the philosopher finishes the bowl of spaghetti
        Sleep(500 + rand() % 1000);                                 // think for a random time
        printf("Philosopher #%d is hungry.\n", id);
        wait_start = clock();
        if(left < right) {                                 // Resource Hierarchy Solution for Avoiding Deadlocks, as proposed by Edsger W. Dijkstra
            sem_wait(&chopstick[left]);                             // pick up the chopstick, or wait for it if it's currently being used
            sem_wait(&chopstick[right]);                            // pick up the chopstick, or wait for it if it's currently being used
        }
        else {
            sem_wait(&chopstick[right]);                            // pick up the chopstick, or wait for it if it's currently being used
            sem_wait(&chopstick[left]);                             // pick up the chopstick, or wait for it if it's currently being used
        }
        wait_end = clock();
        waiting_time = ((double)(wait_end - wait_start)) / CLOCKS_PER_SEC;
        printf("Philosopher #%d picked up the chopsticks (#%d, #%d) and started eating. (Waited for %.3f seconds.)(Bite: %d/%d)\n", id, left+1, right+1, waiting_time, i, BITES);
        Sleep(1000 + rand() % 1000);                                 // eat for a random time
        printf("Philosopher #%d finished eating, set down the chopsticks (#%d, #%d), and started thinking.\n", id, left+1, right+1);
        sem_post(&chopstick[left]);                                 // keep the chopstick back on the table
        sem_post(&chopstick[right]);                                // keep the chopstick back on the table
    }
    printf("Philosopher #%d left the room.\n", id);
    pthread_exit(NULL);                                             // exit from the thread
}

int main(int argc, char const *argv[]) {
    int i;                                                          // counter for loops
    pthread_t philosopher[PHILOSOPHERS];                            // declare philosophers
    pthread_attr_t attr;                                            // thread attributes
    pthread_attr_init(&attr);                                       // initialize thread attributes with default values
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);    // make the threads "joinable"
    for(i = 0; i < PHILOSOPHERS; i++)                               // initialize the chopsticks
        sem_init(&chopstick[i], 0, 1);
    for(i = 0; i < PHILOSOPHERS; i++)                               // create philosophers (as threads)
        pthread_create(&philosopher[i], &attr, phil, (void *)i);
    for(i = 0; i < PHILOSOPHERS; i++)                               // ensure that all philosophers finish their spaghetti
        pthread_join(philosopher[i], NULL);
    pthread_attr_destroy(&attr);                                    // destroy thread attributes
    for(i = 0; i < PHILOSOPHERS; i++)                               // destroy chopsticks (semaphores)
        sem_destroy(&chopstick[i]);
    pthread_exit(NULL);                                             // unnecessary, but recommended way of exiting a main function in a multi-threaded program
}
