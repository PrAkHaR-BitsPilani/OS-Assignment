#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>

#include "src/scheduler.h"
#include "src/RR.h"
#include "src/FCFS.h"
#include "src/process.h"

pid_t* ID;
int** m_c;
int* c_m;
int* num_process;
int* tq;
float* TAT;
float* RT;
bool* condition;
pthread_cond_t* cond;
pthread_mutex_t* mutex;
RR_arg* arg;

void* getSharedMem(size_t size){
    int prot = PROT_READ | PROT_WRITE;
    int vis = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, prot, vis, -1, 0);
}

int main(int argc, char* argv[])
{
    int WORKLOAD;
    if(argc == 1)
        WORKLOAD = 1000;
    else
        WORKLOAD = atoi(argv[1]);

    {
        num_process = (int*)malloc(sizeof(int));
        tq = (int*)malloc(sizeof(int));
        *num_process = 3;
        *tq = 100;
        ID = (pid_t*)malloc(sizeof(pid_t) * (*num_process));
        m_c = (int**)malloc(sizeof(int*) * (*num_process));
        c_m = (int*)malloc(sizeof(int) * 2);
        condition = (bool*)malloc(sizeof(bool) * (*num_process));
        cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * (*num_process));
        mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * (*num_process));
        RT = (float*)malloc(sizeof(float) * (*num_process));
        arg = (RR_arg*)malloc(sizeof(RR_arg));
        
        TAT = (float*)getSharedMem(sizeof(float) * (*num_process));
    }

    
    for(int i = 0 ; i < (*num_process); i++)
    {
        pthread_cond_init(&cond[i], NULL);
        pthread_mutex_init(&mutex[i], NULL);
        TAT[i] = 0.0f;
        condition[i] = false;
        m_c[i] = (int*)malloc(sizeof(int) * 2);
        pipe(m_c[i]);
    }
    pipe(c_m);

    queue* q = (queue*)malloc(sizeof(queue));
    q -> n = 0;

    ID[0] = fork();

    if(ID[0] == 0){

        monitor_arg* m_arg = (monitor_arg*)malloc(sizeof(monitor_arg));
        m_arg -> p_idx = 0;
        m_arg -> n = WORKLOAD;
        m_arg -> condition = condition;
        m_arg -> cond = cond;
        m_arg -> mutex = mutex;
        m_arg -> c_m = c_m;
        m_arg -> m_c = m_c[0];
        m_arg -> TAT = TAT;
        m_arg -> fun = C1;
        monitor((void*)m_arg);

    }
    else{
        enqueue(q, ID[0]);
        ID[1] = fork();

        if(ID[1] == 0){

            monitor_arg* m_arg = (monitor_arg*)malloc(sizeof(monitor_arg));
            m_arg -> p_idx = 1;
            m_arg -> n = WORKLOAD;
            m_arg -> condition = condition;
            m_arg -> cond = cond;
            m_arg -> mutex = mutex;
            m_arg -> c_m = c_m;
            m_arg -> m_c = m_c[1];
            m_arg -> TAT = TAT;
            m_arg -> fun = C2;
            monitor((void*)m_arg);

        }
        else{
            enqueue(q, ID[1]);
            ID[2] = fork();

            if(ID[2] == 0){

                monitor_arg* m_arg = (monitor_arg*)malloc(sizeof(monitor_arg));
                m_arg -> p_idx = 2;
                m_arg -> n = WORKLOAD;
                m_arg -> condition = condition;
                m_arg -> cond = cond;
                m_arg -> mutex = mutex;
                m_arg -> c_m = c_m;
                m_arg -> m_c = m_c[2];
                m_arg -> TAT = TAT;
                m_arg -> fun = C3;
                monitor((void*)m_arg);

            }
            else{
                enqueue(q, ID[2]);

                arg -> q = q;
                arg -> num_process = num_process;
                arg -> ID = ID;
                arg -> m_c = m_c;
                arg -> c_m = c_m;
                arg -> tq = tq;

                init((void*)arg, FCFS_algo);
                RT = (float*)start();

                FILE* fp = fopen("out", "a");
                fprintf(fp, "WORKLOAD = %d\n", WORKLOAD);
                fprintf(fp, "Process\t\tTAT\t\tRT\t\tWT\n");
                for(int i = 0; i < *(num_process); i++){
                    fprintf(fp, "%d\t%.9f\t%.9f\t%.9f\n", i, TAT[i], RT[i], TAT[i] - RT[i]);
                }
                fclose(fp);
            }
        }
    }

    return 0;
}