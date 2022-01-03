#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "process.h"

void* monitor(void* _arg){

    monitor_arg* arg = (monitor_arg*)_arg;

    pthread_t t;
    process_arg* p_arg = (process_arg*)malloc(sizeof(process_arg));
    int p_idx = arg -> p_idx;
    p_arg -> p_idx = p_idx;
    p_arg -> n = arg -> n;
    p_arg -> condition = arg -> condition;
    p_arg -> cond = arg -> cond;
    p_arg -> mutex = arg -> mutex;
    p_arg -> c_m = arg -> c_m;

    int flag_prev = 0;

    char readbuffer[80];

    pthread_create(&t, NULL, arg -> fun, (void*)p_arg);

    while(1){
        

        while(flag_prev == (arg -> flag)[p_idx]){
            usleep(10);
        }

        flag_prev = (arg -> flag)[p_idx];

        if(flag_prev == 1){
            (arg ->condition)[p_idx] = true;
            pthread_cond_signal(&(arg -> cond)[p_idx]);         // RESUME TASK THREAD
        }
        else{
            if(flag_prev == 0){
                (arg -> condition)[p_idx] = false;              // PAUSE TASK THREAD
            }
            else
                if(flag_prev == 2){
                    break;                                      // EXIT MONITOR THREAD
                }
        }
    }


}

void* C1(void* _arg){

    // struct timespec currtime;
    // clockid_t threadClockID;

    // pthread_getcpuclockid(pthread_self(), &threadClockID);

    process_arg* arg = (process_arg*)_arg;
    int sum = 0;
    int p_idx = arg -> p_idx;

    pthread_cond_wait(&(arg -> cond)[p_idx], &(arg -> mutex)[p_idx]); // wait before starting

    for(int i = 0; i < arg -> n; i++){
        while(!(arg -> condition)[p_idx])
            pthread_cond_wait(&(arg -> cond)[p_idx], &(arg -> mutex)[p_idx]); // wait for monitor thread to signal
        sum += i;
    }
    char temp[10];
    sprintf(temp,"%d",sum);
    char fin[15] = "00";
    strncat(fin,temp,strlen(temp));
    write((arg -> c_m)[1], fin, strlen(fin) + 1);

    // clock_gettime(threadClockID, &currtime);
    // float RT = currtime.tv_sec + (currtime.tv_nsec * (1e-9));
    // printf("RT for C1 Task Thread: %.9f\n", RT);
    // fflush(stdout);
}

void* C2(void* _arg){

    process_arg* arg = (process_arg*)_arg;
    int t;
    int p_idx = arg -> p_idx;
    pthread_cond_wait(&(arg -> cond)[p_idx], &(arg -> mutex)[p_idx]); // wait before starting

    FILE* fp = fopen("input/in_C2", "r");

    for(int i = 0; i < arg -> n; i++){
        while(!(arg -> condition)[p_idx])
            pthread_cond_wait(&(arg -> cond)[p_idx], &(arg -> mutex)[p_idx]); // wait for monitor thread to signal
        fscanf(fp, "%d", &t);
        printf("%d\n", t);
    }
    fclose(fp);
    char fin[] = "01Done Printing";
    write((arg -> c_m)[1], fin, strlen(fin) + 1);
}

void* C3(void* _arg){

    process_arg* arg = (process_arg*)_arg;
    int sum = 0;
    int t;
    int p_idx = arg -> p_idx;
    pthread_cond_wait(&(arg -> cond)[p_idx], &(arg -> mutex)[p_idx]); // wait before starting

    FILE* fp = fopen("input/in_C3","r");

    for(int i = 0; i < arg -> n; i++){
        while(!(arg -> condition)[p_idx])
            pthread_cond_wait(&(arg -> cond)[p_idx], &(arg -> mutex)[p_idx]); // wait for monitor thread to signal
        fscanf(fp, "%d", &t);
        sum += t;
    }
    fclose(fp);
    char temp[10];
    sprintf(temp,"%d",sum);
    char fin[15] = "10";
    strncat(fin,temp,strlen(temp));
    write((arg -> c_m)[1], fin, strlen(fin) + 1);
}