#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "sched.h"
#include <time.h>

pthread_key_t key;

Deque * constructeurDeque() {
    Deque *d = malloc(sizeof(Deque));
    if (d != NULL){
        d->tete = d->fin = NULL;
        d->size = 0;
    }
    return d;
}


void dequePushHaut(Deque *d, taskfunc v,void * args) {
    pthread_mutex_lock(&d->myMutex);
    Element *n = malloc(sizeof(Element));
    assert(n != NULL);
    n->valeur = v;
    n->args = args;
    n->next = d->tete;
    n->prev = NULL;
    if (d->size == 0) {
        d->tete = d->fin = n;
    } else {
        d->tete->prev = n;
        d->tete = n;
    }
    d->size++;
    pthread_mutex_unlock(&d->myMutex);
}


Element * dequePopHaut(Deque *d) {
    pthread_mutex_lock(&d->myMutex);
    Element *n = d->tete;
    if (d->size == 0){
        d->tete = d->fin = NULL;
        pthread_mutex_unlock(&d->myMutex);
        return NULL;
    } else {
        d->tete = n->next;
        d->size--;
        pthread_mutex_unlock(&d->myMutex);
        return n;
    }

}

Element * dequePopBas(Deque *d) {
    pthread_mutex_lock(&d->myMutex);
    Element *n = d->fin;
    if (d->size == 0){
        d->tete = d->fin = NULL;
        pthread_mutex_unlock(&d->myMutex);
        return NULL;
    } else {
        d->fin = n->prev;
        d->size--;
        pthread_mutex_unlock(&d->myMutex);
        return n;
    }

}


long sched_init(int nthreads, int qlen, taskfunc f, void *closure){// int , int , quicksort , quicksort_args
    struct scheduler *shed = (struct scheduler*)malloc(sizeof(struct scheduler));

    if (shed != NULL){
        if(nthreads <= 0)
            shed->nombreThreads = sched_default_threads();
        else
            shed->nombreThreads = nthreads;

        shed->deque =  malloc(shed->nombreThreads*sizeof(Deque));

        for (int i = 0;i<shed->nombreThreads;i++){
            shed->deque[i] = malloc(sizeof(Deque*));
            shed->deque[i] = constructeurDeque();
            pthread_mutex_init(&shed->deque[i]->myMutex, NULL);
        }

        if (pthread_key_create(&key, NULL ) < 0) {
            printf("pthread_key_create failed, %d ", errno);
            exit(1);
        }

        dequePushHaut(shed->deque[0],f,closure);
        shed->threadOnWork = 0;
        shed->atomicIndex = 0;

        shed->pthread =  malloc(shed->nombreThreads*sizeof(pthread_t));

        for (int i = 0;i<shed->nombreThreads;i++){
            shed->pthread[i] = malloc(sizeof(pthread_t*));
        }

        for (int i = 0;i<shed->nombreThreads;i++){
            if(pthread_create(shed->pthread[i], NULL, touverTaskStealing, shed)) {
                fprintf(stderr, "Error creating thread\n");
                return NULL;
            }
        }

        for (int i = 0;i<shed->nombreThreads;i++){
            pthread_join(*shed->pthread[i],NULL);
        }

        /*
        for (int i = 0;i<shed->nombreThreads;i++){
            if(pthread_create(&shed->deque[i]->mythread, NULL, touverTaskStealing, shed)) {
                fprintf(stderr, "Error creating thread\n");
                return NULL;
            }
        }

        for (int i = 0;i<shed->nombreThreads;i++){
            pthread_join(shed->deque[i]->mythread,NULL);
        }*/

        pthread_key_delete(key);

        for (int i = 0;i<shed->nombreThreads;i++){
            free(shed->deque[i]);
        }

        free(shed);

        return 1;
    }

    return 0;
}


int sched_spawn(taskfunc f, void * closure, struct scheduler *s){// quicksort , quicksort_args , schedule

    dequePushHaut(s->deque[(int)pthread_getspecific(key)],f,closure);

    return 1;

}


void * touverTaskStealing(void * shod) {

    struct scheduler *sched = (struct scheduler *) shod;

    if (pthread_setspecific(key, (_Atomic int *) sched->atomicIndex++) <  0) {
        printf("pthread_setspecific failed, errno %d",errno);
        pthread_exit(NULL);
    }

    while (1) {

        if (sched->deque[(int)pthread_getspecific(key)]->size == 0) {//alors cest vide ya pas de tache dans ma deque

            for (int j = 0; j < sched->nombreThreads; j++) {

                if (sched->deque[j]->size != 0) {//faire un stealing

                    sched->threadOnWork++;

                    Element *elementDuVoisin = dequePopBas(sched->deque[j]);

                    if(elementDuVoisin != NULL){
                        (*elementDuVoisin->valeur)(elementDuVoisin->args, sched);
                        free(elementDuVoisin);
                    }

                    sched->threadOnWork--;

                    break;

                }else{

                    struct timespec time { 0 , 1000 * 1000 };              	
                    nanosleep(&time , NULL);

                    if (sched->threadOnWork == 0) {
                        pthread_exit(NULL);
                    }
                }
            }

        } else {//si ma deque nest pas vide je fait un pop depuis ma deque

            sched->threadOnWork++;

            Element *myElement = dequePopHaut(sched->deque[(int)pthread_getspecific(key)]);

            if(myElement != NULL){
                (*myElement->valeur)(myElement->args, sched);
                free(myElement);
            }

            sched->threadOnWork--;

        }
    }

    return NULL;
}





