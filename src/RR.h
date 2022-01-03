#ifndef RR
#define RR

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "queue.h"

/// Input format for RR
struct RR_arg{

    queue* q;

    int* num_process;
    pid_t* ID;

    int* flag;
    int* c_m;

    FILE* fp;
    
    int* tq;
};
typedef struct RR_arg RR_arg;

struct RR_ret{
    float* RT;
    float* TAT;
};
typedef struct RR_ret RR_ret;

/// RR Algorithm
void* RR_algo (void*);

#endif