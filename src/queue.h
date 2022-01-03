#ifndef QUEUE
#define QUEUE

#include <stdbool.h>

struct Node{
    int ele;
    struct Node* next;
};

struct queue{
    struct Node *head, *tail;
    int n;
};

typedef struct Node Node;
typedef struct queue queue;

/// Add an element to the queue
void enqueue(queue* q, int ele);

/// Remove an element from the queue
Node* dequeue(queue* q);

/// Get the front element of the queue
Node* getFront(queue* q);

///Check if the queue is empty
bool isEmpty(queue* q);

#endif
