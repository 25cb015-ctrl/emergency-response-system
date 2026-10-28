#ifndef GRAPH_H
#define GRAPH_H
#include <stddef.h>

#define MAX_LOC 10
#define INF 1000000

/* City road network: weighted undirected graph (adjacency matrix) */
typedef struct {
    int n;
    int adj[MAX_LOC][MAX_LOC];      /* 0 = no road, else distance in km */
    const char *names[MAX_LOC];
} Graph;

void graph_init(Graph *g, int n, const char *names[]);
void graph_add_road(Graph *g, int u, int v, int km);
void graph_dijkstra(const Graph *g, int src, int dist[], int prev[]);
int  graph_path(const Graph *g, const int prev[], int src, int dst,
                char *buf, size_t size);

#endif
