#include <pthread.h>                                                // for creating threads
#include <stdio.h>                                                  // for standard I/O
#include <stdlib.h>                                                 // for exit()
#include <windows.h>                                                // for Sleep(microseconds) in Windows; use unistd.h for sleep(seconds) in Linux
#define MAX_ITERATIONS 10

FILE *input, *output;                                               // input and output file pointers

pthread_mutex_t mutex;                                              // mutex variable
pthread_cond_t cond;                                                // condition variable use for blocking and waking threads

boolean cannot_consume = FALSE;                                     // booleans used to avoid any infinite loops
boolean cannot_produce = FALSE;                                     // booleans used to avoid any infinite loops

int n = 0;                                                          // no. of elements in the production_rate and consumption_rate arrays
int production_rate[MAX_ITERATIONS];                                // production rate of producer at each stage
int p = 0;                                                          // stage of production, used to index the production_rate array
int consumption_rate[MAX_ITERATIONS];                               // consumption rate of consumer at each stage
int c = 0;                                                          // stage of consumption, used to index the consumption_rate array

int buffer = 0;                                                     // current size of buffer
int buffer_size = 0;                                                // max size of buffer

void * producer(){                                                  // producer - thread
    while (p < n) {                                                 // loop till the production_rate array is traversed
        if (buffer_size - buffer >= production_rate[p]){            // check whether there is space in buffer for producer to produce
            cannot_produce = FALSE;                                 // producer can produce
            pthread_mutex_lock(&mutex);                             // lock mutex
            buffer += production_rate[p++];                         // update buffer size to simulate production
            printf("\nProducer Produced Successfully");
            printf("\nBuffer Spaces Filled: %d", buffer);           // display current buffer size
            fprintf(output, "\nProducer Produced Successfully");    // write the same details to output file
            fprintf(output, "\nBuffer Spaces Filled: %d", buffer);
            pthread_mutex_unlock(&mutex);                           // unlock mutex
            pthread_cond_signal(&cond);                             // signal the consumer process
            Sleep(10);                                              // wait for 10ms, giving time for consumer to lock mutex if required
        }
        if (buffer_size - buffer < production_rate[p]) {            // when space is insufficient for producer to produce
            cannot_produce = TRUE;                                  // producer cannot produce
            if (buffer == buffer_size) {                            // if buffer is full
                printf("\nProducer Cannot Produce: Buffer Full");
                printf("\nBuffer Spaces Filled: %d", buffer);       // display buffer size
                fprintf(output, "\nProducer Cannot Produce: Buffer Full");// write the same details to output file
                fprintf(output, "\nBuffer Spaces Filled: %d", buffer);
            }
            pthread_cond_signal(&cond);                             // signal consumer
            Sleep(10);                                              // sleep for 10ms
            if (cannot_consume) {                                   // terminate the thread when producer cannot produce and consumer cannot consume
                pthread_exit(NULL);
            }
        }
    }
    cannot_produce = TRUE;
    pthread_exit(NULL);
}

void * consumer(){                                                  // consumer - thread
    pthread_mutex_lock(&mutex);
    while (c < n) {                                                 // loop till the production_rate array is traversed
        while (buffer < consumption_rate[c]) {                      // when buffer contents not enough for consumer to consume
            cannot_consume = TRUE;                                  // consumer cannot consume
            if (!buffer) {                                          // when buffer is empty
                printf("\nConsumer Cannot Consume: Buffer Empty");
                printf("\nBuffer Spaces Filled: %d", buffer);       // display buffer size
                fprintf(output, "\nConsumer Cannot Consume: Buffer Empty");// write the same details to output file
                fprintf(output, "\nBuffer Spaces Filled: %d", buffer);
            }
            if (cannot_produce)                                     // terminate the thread when producer cannot produce and consumer cannot consume
                pthread_exit(NULL);
            pthread_cond_wait(&cond, &mutex);                       // unlock mutex and wait for producer's signal
        }
        if ( buffer >= consumption_rate[c] ){                       // when consumer can consume
            cannot_consume = FALSE;                                 // consumer can consume
            buffer -= consumption_rate[c++];                        // simulate consumption by updating buffer size
            printf("\nConsumer Consumed Successfully");
            printf("\nBuffer Spaces Filled: %d", buffer);           // display buffer size
            fprintf(output, "\nConsumer Consumed Successfully");    // write the same details to output file
            fprintf(output, "\nBuffer Spaces Filled: %d", buffer);
        }
    }
    pthread_mutex_unlock(&mutex);                                   // unlock mutex
    cannot_consume = TRUE;                                          // consumer process is terminating
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    int i;                                                          // counter for loops

    pthread_t producer_thread, consumer_thread;                     // threads
    pthread_attr_t attr;                                            // thread attributes

    pthread_mutex_init(&mutex, NULL);                               // initialize mutex with default values
    pthread_cond_init (&cond, NULL);                                // initialize condition variable with default values
    pthread_attr_init(&attr);                                       // initialize thread attributes with default values
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);    // make the threads "joinable"

    if(argc != 2) {                                                 // display documentation if no input file is specified
        printf("\nUSAGE: %s INPUT_FILENAME\n", argv[0]);

        printf("\nINPUT:\n   A text file.");
        printf("\n   The first line contains a positive integer");
        printf("\n   which denotes the maximum possible size of the buffer.");
        printf("\n   The next line contains a positive integer 'n',");
        printf("\n   which denotes number of times the producer/consumer can");
        printf("\n   produce/consume into/from the buffer.");
        printf("\n   The next 'n' lines contain one integer on each line,");
        printf("\n   each integer denoting the production rate of the producer");
        printf("\n   each time it is called to produce into the buffer.");
        printf("\n   The next 'n' lines contain one integer on each line,");
        printf("\n   each integer denoting the consumption rate of the consumer");
        printf("\n   each time it is called to consume from the buffer.");

        printf("\nOUTPUT:\n   A text file, \"pc_output.txt\".\n   The information written to the file is also displayed on screen.\n   If the file doesn't exist, a new file will be automatically created.\n   If the file exists, the new output will overwrite the original file.\n\n");
        exit(0);
    }

    input = fopen(argv[1], "r");                                    // open input file
    if(!input){                                                     // if unable to open file
        printf("\n[.] Could not open %s.\n\n", argv[1]);
        exit(1);
    }
    fscanf(input, "%d", &buffer_size);                              // read buffer size from file
    fscanf(input, "%d", &n);                                        // read no. of producer/consumer will run from file
    for (i = 0; i < n; i++)                                         // read production rate array
        fscanf(input, "%d", &production_rate[i]);
    for (i = 0; i < n; i++)                                         // read consumption rate array
        fscanf(input, "%d", &consumption_rate[i]);
    fclose(input);

    output = fopen(".\\pc_output.txt", "w");                        // open file for writing output
    if(!output){
        printf("\n[.] Could not open output file.\n");
        exit(1);
    }

    pthread_create(&consumer_thread, &attr, consumer, NULL);        // create the consumer thread
    pthread_create(&producer_thread, &attr, producer, NULL);        // create the producer thread

    pthread_join(producer_thread, NULL);                            // ensure that the producer finished execution
    pthread_join(consumer_thread, NULL);                            // ensure that the consumer finished execution

    fclose(output);

    pthread_attr_destroy(&attr);                                    // destroy thread attributes
    pthread_mutex_destroy(&mutex);                                  // destroy mutex
    pthread_cond_destroy(&cond);                                    // destroy conditional variable
    pthread_exit(NULL);                                             // unnecessary, but recommended way of exiting a main function in a multi-threaded program
}
