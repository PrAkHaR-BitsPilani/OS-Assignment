#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FCFS.h"
#include "signals.h"

void* FCFS_listener(void*);
bool FCFS_check(bool*, int);

bool* finished;
FCFS_arg* arg;

void* FCFS_algo(void* _arg){

    arg = (FCFS_arg*)_arg;

    float* RT = (float*)malloc(sizeof(float) * (*(arg -> num_process)));
    float* TAT = (float*)malloc(sizeof(float) * (*(arg -> num_process)));
    queue* q = arg -> q;
    int n = *(arg -> num_process);
    pid_t* ID = arg -> ID;
    int* flag = arg -> flag;
    int* c_m = arg -> c_m;
    finished = (bool*)malloc(sizeof(bool) * n);

    for(int i = 0; i < n; i++){

        RT[i] = TAT[i] = 0.0;
        finished[i] = false;
        flag[i] = 0;
    }

    close(c_m[1]);

    pthread_t listen;
    pthread_create(&listen, NULL, &FCFS_listener, NULL);                // create listener thread to check for completed signal

    time_t current_time;
    struct tm* time_info;
    char timeString[9];

    struct timespec begin0;
    clock_gettime(CLOCK_MONOTONIC, &begin0);

    while(!isEmpty(q)){                                                 // loop till queue is not empty

        pid_t t = dequeue(q) -> ele;                                    // get top element of queue

        int i = 0;
        for(; i < n; i++){
            if(ID[i] == t)
                break;
        }

        struct timespec begin, end;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
        fprintf((arg -> fp), "Process %d starts at %s\n", i+1, timeString);

        flag[i] = AWAKE;

        while(!finished[i]){                                            // check if task thread has completed
            usleep(100);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        RT[i] = end.tv_sec - begin.tv_sec;
        RT[i] += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
        TAT[i] = end.tv_sec - begin0.tv_sec;
        TAT[i] += (end.tv_nsec - begin0.tv_nsec) / 1000000000.0;

    }

    FCFS_ret* ret = (FCFS_ret*)malloc(sizeof(FCFS_ret));
    ret -> RT = RT;
    ret -> TAT = TAT;
    return (void*)ret;
}

void* FCFS_listener(void* _arg)
{
    char readbuffer[20];
    int sum_C1;
    int sum_C3;
    while(FCFS_check(finished, *(arg -> num_process)))                  // check if all processes have completed
    {
        int nbytes = read((arg -> c_m)[0], readbuffer, 20);

        if (readbuffer[0] == '0')
        {
            if (readbuffer[1] == '0'){
                finished[0] = true;
                sum_C1 = atoi(&readbuffer[2]);
                (arg -> flag)[0] = FINISHED;
            }
            else{
                finished[1] = true;
                (arg -> flag)[1] = FINISHED;
            }
        }
        else{
            finished[2] = true;
            sum_C3 = atoi(&readbuffer[2]);
            (arg -> flag)[2] = FINISHED;
        }
    }
    printf("C1 Sum: %d\nC3 Sum: %d\n", sum_C1, sum_C3);
}

bool FCFS_check(bool* finished, int n){
    for(int i = 0; i < n; i++)
        if(!finished[i])
            return true;
    return false;
}