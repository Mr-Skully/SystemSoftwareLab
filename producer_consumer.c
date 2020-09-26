#include <pthread.h>                                                // for creating threads
#include <stdio.h>                                                  // for standard I/O
#include <windows.h>                                                // for Sleep(microseconds) in Windows; use unistd.h for sleep(seconds) in Linux

pthread_mutex_t mutex;                                              // mutex variable
pthread_cond_t cond;                                                // condition variable use for blocking and waking threads

boolean cannot_consume = FALSE;                                     // booleans used to avoid any infinite loops
boolean cannot_produce = FALSE;                                     // booleans used to avoid any infinite loops

int n = 3;                                                          // no. of elements in the production_rate and consumption_rate arrays
int production_rate[] = {3, 7, 9};                                 // production rate of producer at each stage
int p = 0;                                                          // stage of production, used to index the production_rate array
int consumption_rate[] = {4, 3, 8};                                // consumption rate of consumer at each stage
int c = 0;                                                          // stage of consumption, used to index the consumption_rate array

int buffer = 0;                                                     // current size of buffer
int buffer_size = 20;                                               // max size of buffer

void * producer(){                                                  // producer - thread
    while (p < n) {                                                 // loop till the production_rate array is traversed
        if (buffer_size - buffer >= production_rate[p]){            // check whether there is space in buffer for producer to produce
            cannot_produce = FALSE;                                 // producer can produce
            pthread_mutex_lock(&mutex);                             // lock mutex
            buffer += production_rate[p++];                         // update buffer size to simulate production
            printf("\nProducer Produced Successfully");
            printf("\nBuffer Spaces Filled: %d", buffer);           // display current buffer size
            pthread_mutex_unlock(&mutex);                           // unlock mutex
            pthread_cond_signal(&cond);                             // signal the consumer process
            Sleep(10);                                              // wait for 10ms, giving time for consumer to lock mutex if required
        }
        if (buffer_size - buffer < production_rate[p]) {            // when space is insufficient for producer to produce
            cannot_produce = TRUE;                                  // producer cannot produce
            if (buffer == buffer_size) {                            // if buffer is full
                printf("\nProducer Cannot Produce: Buffer Full");
                printf("\nBuffer Spaces Filled: %d", buffer);       // display buffer size
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
        }
    }
    pthread_mutex_unlock(&mutex);                                   // unlock mutex
    cannot_consume = TRUE;                                          // consumer process is terminating
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    pthread_t producer_thread, consumer_thread;                     // threads
    pthread_attr_t attr;                                            // thread attributes

    pthread_mutex_init(&mutex, NULL);                               // initialize mutex with default values
    pthread_cond_init (&cond, NULL);                                // initialize condition variable with default values
    pthread_attr_init(&attr);                                       // initialize thread attributes with default values
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);    // make the threads "joinable"

    pthread_create(&consumer_thread, &attr, consumer, NULL);        // create the consumer thread
    pthread_create(&producer_thread, &attr, producer, NULL);        // create the producer thread

    pthread_join(producer_thread, NULL);                            // ensure that the producer finished execution
    pthread_join(consumer_thread, NULL);                            // ensure that the consumer finished execution

    pthread_attr_destroy(&attr);                                    // destroy thread attributes
    pthread_mutex_destroy(&mutex);                                  // destroy mutex
    pthread_cond_destroy(&cond);                                    // destroy conditional variable
    pthread_exit(NULL);                                             // unnecessary, but recommended way of exiting a main function in a multi-threaded program
}
