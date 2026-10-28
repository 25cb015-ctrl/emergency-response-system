#include <stdlib.h>
#include <string.h>
#include "heap.h"

static int higher(const Patient *a, const Patient *b) {
    if (a->severity != b->severity) return a->severity < b->severity;
    return a->arrival < b->arrival;
}

static void swap(Patient *a, Patient *b) {
    Patient t = *a; *a = *b; *b = t;
}

int pq_init(PriorityQueue *pq, int capacity) {
    if (capacity < 1) capacity = 1;
    pq->data = malloc((size_t)capacity * sizeof(Patient));
    if (!pq->data) return 0;
    pq->size = 0;
    pq->capacity = capacity;
    return 1;
}

int pq_push(PriorityQueue *pq, Patient p) {
    if (pq->size == pq->capacity) {
        int ncap = pq->capacity * 2;
        Patient *tmp = realloc(pq->data, (size_t)ncap * sizeof(Patient));
        if (!tmp) return 0;
        pq->data = tmp;
        pq->capacity = ncap;
    }
    int i = pq->size++;
    pq->data[i] = p;
    while (i > 0) {                       /* sift up */
        int parent = (i - 1) / 2;
        if (!higher(&pq->data[i], &pq->data[parent])) break;
        swap(&pq->data[i], &pq->data[parent]);
        i = parent;
    }
    return 1;
}

int pq_pop(PriorityQueue *pq, Patient *out) {
    if (pq->size == 0) return 0;
    *out = pq->data[0];
    pq->data[0] = pq->data[--pq->size];
    int i = 0;
    for (;;) {                            /* sift down */
        int l = 2 * i + 1, r = 2 * i + 2, best = i;
        if (l < pq->size && higher(&pq->data[l], &pq->data[best])) best = l;
        if (r < pq->size && higher(&pq->data[r], &pq->data[best])) best = r;
        if (best == i) break;
        swap(&pq->data[i], &pq->data[best]);
        i = best;
    }
    return 1;
}

int pq_clone(const PriorityQueue *src, PriorityQueue *dst) {
    if (!pq_init(dst, src->size > 0 ? src->size : 1)) return 0;
    memcpy(dst->data, src->data, (size_t)src->size * sizeof(Patient));
    dst->size = src->size;
    return 1;
}

void pq_free(PriorityQueue *pq) {
    free(pq->data);
    pq->data = NULL;
    pq->size = pq->capacity = 0;
}
