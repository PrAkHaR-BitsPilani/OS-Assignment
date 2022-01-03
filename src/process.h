#ifndef PROCESS
#define PROCESS

#include <pthread.h>
#include <stdbool.h>

/// Input format for Task Thread
struct process_arg{

    int p_idx;
    int n;
    bool* condition;
    pthread_cond_t* cond;
    pthread_mutex_t* mutex;
    int* c_m;

};
typedef struct process_arg process_arg;

/// Input format for Monitor Thread
struct monitor_arg{
    
    int p_idx;
    int n;
    bool* condition;
    pthread_cond_t* cond;
    pthread_mutex_t* mutex;
    int* c_m;
    int* flag;
    void* (*fun)(void*);
};
typedef struct monitor_arg monitor_arg;

void* monitor(void*);
void* C1(void*);
void* C2(void*);
void* C3(void*);

#endif