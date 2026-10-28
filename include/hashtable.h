#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "patient.h"

#define HT_BUCKETS 101

typedef struct HNode {
    Patient p;
    struct HNode *next;
} HNode;

/* Hash table with separate chaining: patient ID -> record */
typedef struct {
    HNode *buckets[HT_BUCKETS];
    int count;
} HashTable;

void     ht_init(HashTable *ht);
int      ht_insert(HashTable *ht, Patient p);   /* 0 if duplicate id */
Patient *ht_find(HashTable *ht, int id);
int      ht_remove(HashTable *ht, int id);
void     ht_free(HashTable *ht);

#endif
