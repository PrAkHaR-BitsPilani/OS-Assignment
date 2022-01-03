#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "RR.h"
#include "signals.h"

void* RR_listener(void*);
bool RR_check(bool*,int);

bool* finished;
RR_arg* arg;

void* RR_algo(void* _arg){

    arg = (RR_arg*)_arg;

    float* RT = (float*)malloc(sizeof(float) * (*(arg -> num_process)));
    float* TAT = (float*)malloc(sizeof(float) * (*(arg -> num_process)));
    queue* q = arg -> q;
    int n = *(arg -> num_process);
    pid_t* ID = arg -> ID;
    int* flag = arg -> flag;
    int* c_m = arg -> c_m;
    int tq = *(arg -> tq);
    finished = (bool*)malloc(sizeof(bool) * n);

    for(int i = 0; i < n; i++)
    {
        RT[i] = TAT[i] = 0.0;
        finished[i] = false;
        flag[i] = 0;
    }

    close(c_m[1]);

    pthread_t listen;
    pthread_create(&listen, NULL, &RR_listener, NULL);              // create listener thread to check for completed signal

    time_t current_time;
    struct tm* time_info;
    char timeString[9];

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC, &begin);

    while(!isEmpty(q)){                                             // loop till queue is not empty

        pid_t t = dequeue(q) -> ele;                                // get top element of queue

        int i = 0;
        for(; i < n; i++){
            if(ID[i] == t)
                break;
        }
        RT[i] += (tq / 1e6);
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
        fprintf((arg -> fp), "Process %d starts at %s\n", i+1, timeString);

        // write(m_c[i][1], AWAKE, strlen(AWAKE) + 1);
        flag[i] = AWAKE;
        usleep(tq);
        // write(m_c[i][1], SLEEP, strlen(SLEEP) + 1);
        flag[i] = SLEEP;

        if(!finished[i]){                                           // check if task thread has completed
            enqueue(q, t);
        }
        else{
            clock_gettime(CLOCK_MONOTONIC, &end);
            TAT[i] = end.tv_sec - begin.tv_sec;
            TAT[i] += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
        }
    }
    RR_ret* ret = (RR_ret*)malloc(sizeof(RR_ret));
    ret -> RT = RT;
    ret -> TAT = TAT;
    return (void*)ret;

}

void* RR_listener(void* _arg)
{
    char readbuffer[20];
    int sum_C1;
    int sum_C3;
    while(RR_check(finished, *(arg -> num_process)))                // check if all processes have completed
    {
        int nbytes = read((arg -> c_m)[0], readbuffer, 20);

        if (readbuffer[0] == '0')
        {
            if (readbuffer[1] == '0'){
                finished[0] = true;
                sum_C1 = atoi(&readbuffer[2]);
                // write((arg -> m_c)[0][1], FINISHED, strlen(FINISHED) + 1);
                (arg -> flag)[0] = FINISHED;
            }
            else{
                finished[1] = true;
                // write((arg -> m_c)[1][1], FINISHED, strlen(FINISHED) + 1);
                (arg -> flag)[1] = FINISHED;
            }
        }
        else{
            finished[2] = true;
            sum_C3 = atoi(&readbuffer[2]);
            // write((arg -> m_c)[2][1], FINISHED, strlen(FINISHED) + 1);
            (arg -> flag)[2] = FINISHED;
        }
    }
    printf("C1 Sum: %d\nC3 Sum: %d\n", sum_C1, sum_C3);
}

bool RR_check(bool* finished, int n){
    for(int i = 0; i < n; i++)
        if(!finished[i])
            return true;
    return false;
}


