/*
    OS Assignment
    
    MEMBER                      ID
    
    Arjun Prasad                2019A7PS0183H
    Aryan Saraf                 2019A7PS0138H
    Baraiya Kruti HarshadKumar  2019A7PS1260H
    Akshat Jain                 2019A7PS0117H
    Kommepally Deva Sai Harsha  2019A7PS0012H
    Prakhar Agarwal             2019A7PS0174H
    Tejas Tripathi              2019A7PS0059H
    Vedansh Srivastava          2019A7PS0323H

    ***NOTE***
    MONITOR Thread checks for messages from Master Process at every 10us.
    TIME QUANTUM = 100us.

*/


#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "src/scheduler.h"
#include "src/RR.h"
#include "src/FCFS.h"
#include "src/process.h"

pid_t* ID;
int* c_m;
int* num_process;
int* tq;
bool* condition;
pthread_cond_t* cond;
pthread_mutex_t* mutex;
RR_arg* arg;
RR_ret* ret;
int* flag;

/// Returns a block of memory shared between parent and child processes.
void* getSharedMem(size_t size){
    int prot = PROT_READ | PROT_WRITE;
    int vis = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, prot, vis, -1, 0);
}

int main(int argc, char* argv[])
{

    num_process = (int*)malloc(sizeof(int));
    tq = (int*)malloc(sizeof(int));
    *num_process = 3;
    *tq = 100;

    int n[*num_process];
    int mode;
    // USER INPUT
    {
        printf("Enter the value for n1: ");scanf("%d",&n[0]);
        printf("Enter the value for n2: ");scanf("%d",&n[1]);
        printf("Enter the value for n3: ");scanf("%d",&n[2]);
        printf("Enter 0 for FCFS or 1 for RR: ");scanf("%d", &mode);
    }

    // INITIALZATION BLOCK
    {
        ID = (pid_t*)malloc(sizeof(pid_t) * (*num_process));
        c_m = (int*)malloc(sizeof(int) * 2);
        condition = (bool*)malloc(sizeof(bool) * (*num_process));
        cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * (*num_process));
        mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * (*num_process));
        // ret = (RR_ret*)malloc(sizeof(float) * (*num_process));
        arg = (RR_arg*)malloc(sizeof(RR_arg));
        
        flag = (int*)getSharedMem(sizeof(int) * (*num_process));
    
        for(int i = 0 ; i < (*num_process); i++)
        {
            pthread_cond_init(&cond[i], NULL);
            pthread_mutex_init(&mutex[i], NULL);
            condition[i] = false;
            flag[i] = 0;
        }
        pipe(c_m);
    }

    queue* q = (queue*)malloc(sizeof(queue));
    q -> n = 0;
    void* (*fun[])(void*) = {C1, C2, C3};

    // Create N child processes
    for(int i = 0; i < *(num_process); i++){

        ID[i] = fork();

        if(ID[i] == 0){

            monitor_arg* m_arg = (monitor_arg*)malloc(sizeof(monitor_arg));
            m_arg -> p_idx = i;
            m_arg -> n = n[i];
            m_arg -> condition = condition;
            m_arg -> cond = cond;
            m_arg -> mutex = mutex;
            m_arg -> c_m = c_m;
            m_arg -> flag = flag;
            m_arg -> fun = fun[i];
            monitor((void*)m_arg); // Launch Monitor Thread
            exit(0);

        }
        enqueue(q, ID[i]);
    }
    
    FILE* fp = fopen("out", "a");

    arg -> q = q;
    arg -> num_process = num_process;
    arg -> ID = ID;
    arg -> flag = flag;
    arg -> c_m = c_m;
    arg -> tq = tq;
    arg -> fp = fp;

    // Initialize the Scheduler
    if(mode == 1)
        init((void*)arg, RR_algo);
    else
        init((void*)arg, FCFS_algo);

    if(mode)
        fprintf(fp, "Round Robin\n");
    else
        fprintf(fp, "First Come First Serve\n");
    fprintf(fp, "n1 = %d\nn2 = %d\nn3 = %d\n", n[0], n[1], n[2]);
    fprintf(fp, "STARTED SCHEDULING\n");
    
    ret = (RR_ret*)start();

    fprintf(fp, "\nProcess\t\tTAT\t\tRT\t\t\tWT\n");
    for(int i = 0; i < *(num_process); i++){
        fprintf(fp, "%d\t\t%.9f\t%.9f\t%.9f\n", i, (ret -> TAT)[i], (ret ->RT)[i], (ret -> TAT)[i] - (ret -> RT)[i]);
    }
    fprintf(fp, "\n\n");
    fclose(fp);

    return 0;
}