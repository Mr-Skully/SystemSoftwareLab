#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

int consumer_alive = 1;
int producer_alive = 1;

int n = 3;
int production_rate[] = {3, 7, 9};
int p = 0;
int consumption_rate[] = {4, 3, 8};
int c = 0;

int buffer = 0;
int buffer_size = 20;

void * producer(){
    pthread_mutex_lock(&mutex);
    printf("\nProd locked mutex");
    while (p < n) {
        printf("\np < n");
        while (buffer_size - buffer >= production_rate[p]){
            printf("\nThere is space in buffer");
            buffer += production_rate[p++];
            printf("\nProducer Produced Successfully");
            printf("\nBuffer Spaces Filled: %d", buffer);
            printf("\nSignaling consumer");
            pthread_cond_signal(&cond);
            printf("\nFinished signaling consumer");
        }
        while (buffer_size - buffer < production_rate[p]) {
            printf("\nSpace insufficient for producer");
            if (buffer == buffer_size) {
                printf("\nProducer Cannot Produce: Buffer Full");
                printf("\nBuffer Spaces Filled: %d", buffer);
            }
            printf("\nSignaling consumer");
            pthread_cond_signal(&cond);
            printf("\nFinished signalling consumer");
            if (!consumer_alive) {
                printf("\nConsumer is dead");
                pthread_mutex_unlock(&mutex);
                printf("\nProducer unlocked mutex");
                producer_alive--;
                printf("\nProd dying");
                pthread_exit(NULL);
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    printf("\nProd unlocking mutex");
    producer_alive--;
    printf("\nProd dying");
    pthread_exit(NULL);
}

void * consumer(){
    pthread_mutex_lock(&mutex);
    printf("\ncons locked mutex");
    while (c < n) {
        printf("\nc < n");
        while (buffer < consumption_rate[c]) {
            printf("\nNot enough to consume");
            if (!buffer) {
                printf("\nConsumer Cannot Consume: Buffer Empty");
                printf("\nBuffer Spaces Filled: %d", buffer);
            }
            printf("\ncons is waiting");
            pthread_cond_wait(&cond, &mutex);
            printf("\ncons woke up");
            if(!producer_alive){
                printf("\nprod is dead");
                pthread_mutex_unlock(&mutex);
                printf("\ncons unlocking mutex");
                consumer_alive--;
                printf("\ncons is dying");
                pthread_exit(NULL);
            }
        }
        while ( buffer >= consumption_rate[c] ){
            printf("\ncan consume");
            buffer -= consumption_rate[c++];
            printf("\nConsumer Consumed Successfully");
            printf("\nBuffer Spaces Filled: %d", buffer);
        }
    }
    pthread_mutex_unlock(&mutex);
    printf("\ncons unlocking mutex");
    consumer_alive--;
    printf("\ncons is dying");
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    pthread_t producer_thread, consumer_thread;
    pthread_attr_t attr;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&consumer_thread, &attr, consumer, NULL);
    pthread_create(&producer_thread, &attr, producer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_exit(NULL);
}
