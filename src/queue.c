#include <stdlib.h>
#include "queue.h"

void enqueue(queue* q, int ele){

    if(q -> n == 0){
        
        q -> head = (Node*)malloc(sizeof(Node));
        (q -> head) -> ele = ele;
        (q -> head) -> next = NULL;
        q -> tail = q -> head;
    }
    else{
        Node* temp = (Node*)malloc(sizeof(Node));
        temp -> ele = ele;
        temp -> next = NULL;
        (q -> tail) -> next = temp;
        q -> tail = (q -> tail) -> next;
    }
    (q -> n)++;
}

Node* dequeue(queue* q){

    Node* temp = q -> head;
    (q -> n)--;

    if(q -> n)
        q -> head = (q -> head) -> next;

    return temp;
}

Node* getFront(queue* q){
    return q -> head;
}   

bool isEmpty(queue* q){
    return ((q -> n) == 0);
}