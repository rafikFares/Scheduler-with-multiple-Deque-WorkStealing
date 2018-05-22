#ifndef SCHED_H
#define SCHED_H

#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

typedef struct scheduler schedulerStealing;

typedef void (*taskfunc)(void*, struct scheduler *);
typedef struct element_struct Element;
typedef struct deque_struct Deque;


struct element_struct {
    Element * next;
    Element * prev;
    taskfunc valeur;
    void * args;
};

struct deque_struct {
    Element * tete;
    Element * fin;
    _Atomic int size;
    pthread_t mythread;
    pthread_mutex_t myMutex;
    // _Atomic unsigned long int mythread;
};

Deque * constructeurDeque();

void dequePushHaut(Deque *d, taskfunc v,void * args);

Element * dequePopHaut(Deque *d);
Element * dequePopBas(Deque *d);


struct scheduler{
    Deque ** deque;
    long nombreThreads;
    _Atomic int threadOnWork; // on doit utiliser ca pour savoir si ya un thread qui boss
    pthread_t **pthread; // a utiliser pour faire un join
    _Atomic int atomicIndex;
};

static inline long sched_default_threads(){

    return sysconf(_SC_NPROCESSORS_ONLN);
}

long sched_init(int nthreads, int qlen, taskfunc f, void *closure); // int , int , quicksort , quicksort_args

int sched_spawn(taskfunc f, void *closure, struct scheduler *s);// quicksort , quicksort_args , scheduler

void * touverTaskStealing(void * d);





#endif