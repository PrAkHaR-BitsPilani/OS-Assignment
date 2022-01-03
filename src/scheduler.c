#include "scheduler.h"
#include "RR.h"
#include "FCFS.h"

void* arg;
void* (*fun)(void*);

void* init(void* _arg, void* (*_fun)(void*)){

    arg = _arg;
    fun = _fun;
}

void* start(){
    return fun((void*)arg);
}