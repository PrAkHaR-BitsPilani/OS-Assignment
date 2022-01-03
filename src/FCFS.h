#ifndef FCFS
#define FCFS

#include <stdio.h>
#include <unistd.h>
#include "queue.h"


/// Input format for FCFS
struct FCFS_arg{

    queue* q;

    int* num_process;
    pid_t* ID;

    int* flag;
    int* c_m;

    FILE* fp;

};
typedef struct FCFS_arg FCFS_arg;

struct FCFS_ret{
    float* RT;
    float* TAT;
};
typedef struct FCFS_ret FCFS_ret;

/// FCFS Algorithm
void* FCFS_algo(void*);

#endif