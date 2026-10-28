#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "heap.h"
#include "hashtable.h"
#include "graph.h"
#include "linkedlist.h"

static Patient mk(int id, int sev, long arr) {
    Patient p;
    memset(&p, 0, sizeof p);
    p.id = id; p.severity = sev; p.arrival = arr;
    return p;
}

static void test_heap(void) {
    PriorityQueue pq;
    assert(pq_init(&pq, 2));
    pq_push(&pq, mk(1, 3, 1));
    pq_push(&pq, mk(2, 1, 2));
    pq_push(&pq, mk(3, 1, 3));     /* same severity, later arrival */
    pq_push(&pq, mk(4, 5, 4));
    Patient p;
    int expect[] = {2, 3, 1, 4};
    for (int i = 0; i < 4; i++) { assert(pq_pop(&pq, &p)); assert(p.id == expect[i]); }
    assert(!pq_pop(&pq, &p));
    pq_free(&pq);
}

static void test_hash(void) {
    HashTable ht;
    ht_init(&ht);
    for (int i = 1; i <= 300; i++) assert(ht_insert(&ht, mk(i, 2, i)));
    assert(!ht_insert(&ht, mk(5, 2, 5)));          /* duplicate */
    assert(ht_find(&ht, 250) && ht_find(&ht, 250)->id == 250);
    assert(ht_remove(&ht, 250));
    assert(ht_find(&ht, 250) == NULL);
    assert(ht.count == 299);
    ht_free(&ht);
}

static void test_graph(void) {
    const char *names[] = {"A", "B", "C", "D"};
    Graph g;
    graph_init(&g, 4, names);
    graph_add_road(&g, 0, 1, 4);
    graph_add_road(&g, 0, 2, 1);
    graph_add_road(&g, 2, 1, 2);
    graph_add_road(&g, 1, 3, 5);
    int dist[MAX_LOC], prev[MAX_LOC];
    char path[100];
    graph_dijkstra(&g, 0, dist, prev);
    assert(dist[1] == 3 && dist[3] == 8);
    assert(graph_path(&g, prev, 0, 3, path, sizeof path));
    assert(strcmp(path, "A -> C -> B -> D") == 0);
}

static void test_list(void) {
    DispatchLog l;
    log_init(&l);
    log_append(&l, "one %d", 1);
    log_append(&l, "two %d", 2);
    assert(l.count == 2 && strcmp(l.tail->text, "two 2") == 0);
    log_free(&l);
    assert(l.count == 0);
}

int main(void) {
    test_heap(); test_hash(); test_graph(); test_list();
    printf("All tests passed.\n");
    return 0;
}
