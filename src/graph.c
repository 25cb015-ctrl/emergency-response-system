#include <string.h>
#include "graph.h"

void graph_init(Graph *g, int n, const char *names[]) {
    g->n = n;
    for (int i = 0; i < MAX_LOC; i++) {
        for (int j = 0; j < MAX_LOC; j++) g->adj[i][j] = 0;
        g->names[i] = (i < n) ? names[i] : "";
    }
}

void graph_add_road(Graph *g, int u, int v, int km) {
    g->adj[u][v] = km;
    g->adj[v][u] = km;
}

/* Dijkstra, O(V^2) - ideal for a small dense city map */
void graph_dijkstra(const Graph *g, int src, int dist[], int prev[]) {
    int done[MAX_LOC] = {0};
    for (int i = 0; i < g->n; i++) { dist[i] = INF; prev[i] = -1; }
    dist[src] = 0;

    for (int c = 0; c < g->n; c++) {
        int u = -1;
        for (int i = 0; i < g->n; i++)
            if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1 || dist[u] == INF) break;
        done[u] = 1;
        for (int v = 0; v < g->n; v++) {
            int w = g->adj[u][v];
            if (w > 0 && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
            }
        }
    }
}

int graph_path(const Graph *g, const int prev[], int src, int dst,
               char *buf, size_t size) {
    int stack[MAX_LOC], n = 0;
    for (int v = dst; v != -1 && n < MAX_LOC; v = prev[v]) {
        stack[n++] = v;
        if (v == src) break;
    }
    if (n == 0 || stack[n - 1] != src) { buf[0] = '\0'; return 0; }

    buf[0] = '\0';
    for (int i = n - 1; i >= 0; i--) {
        strncat(buf, g->names[stack[i]], size - strlen(buf) - 1);
        if (i > 0) strncat(buf, " -> ", size - strlen(buf) - 1);
    }
    return 1;
}
