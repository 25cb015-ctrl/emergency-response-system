#ifndef HEAP_H
#define HEAP_H
#include "patient.h"

/* Binary heap priority queue: lowest severity number first,
   earlier arrival wins ties. */
typedef struct {
    Patient *data;
    int size;
    int capacity;
} PriorityQueue;

int  pq_init(PriorityQueue *pq, int capacity);
int  pq_push(PriorityQueue *pq, Patient p);
int  pq_pop(PriorityQueue *pq, Patient *out);
int  pq_clone(const PriorityQueue *src, PriorityQueue *dst);
void pq_free(PriorityQueue *pq);

#endif
