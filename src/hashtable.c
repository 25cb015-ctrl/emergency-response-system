#include <stdlib.h>
#include "hashtable.h"

static int hash(int id) { return (id < 0 ? -id : id) % HT_BUCKETS; }

void ht_init(HashTable *ht) {
    for (int i = 0; i < HT_BUCKETS; i++) ht->buckets[i] = NULL;
    ht->count = 0;
}

Patient *ht_find(HashTable *ht, int id) {
    for (HNode *n = ht->buckets[hash(id)]; n; n = n->next)
        if (n->p.id == id) return &n->p;
    return NULL;
}

int ht_insert(HashTable *ht, Patient p) {
    if (ht_find(ht, p.id)) return 0;
    HNode *n = malloc(sizeof(HNode));
    if (!n) return 0;
    n->p = p;
    int h = hash(p.id);
    n->next = ht->buckets[h];
    ht->buckets[h] = n;
    ht->count++;
    return 1;
}

int ht_remove(HashTable *ht, int id) {
    HNode **link = &ht->buckets[hash(id)];
    while (*link) {
        if ((*link)->p.id == id) {
            HNode *dead = *link;
            *link = dead->next;
            free(dead);
            ht->count--;
            return 1;
        }
        link = &(*link)->next;
    }
    return 0;
}

void ht_free(HashTable *ht) {
    for (int i = 0; i < HT_BUCKETS; i++) {
        HNode *n = ht->buckets[i];
        while (n) { HNode *next = n->next; free(n); n = next; }
        ht->buckets[i] = NULL;
    }
    ht->count = 0;
}
