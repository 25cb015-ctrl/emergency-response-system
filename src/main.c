/*
 * Emergency Response System
 * Triage (heap) + patient records (hash table) + route finding (graph/Dijkstra)
 * + dispatch history (linked list)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "patient.h"
#include "heap.h"
#include "hashtable.h"
#include "graph.h"
#include "linkedlist.h"

#define HOSPITAL 0
#define AMBULANCE_SPEED_KMH 40.0

static const char *LOC_NAMES[MAX_LOC] = {
    "City Hospital", "Railway Station", "Airport", "Market", "University",
    "Tech Park", "Old Town", "Stadium", "Bus Stand", "Lake View"
};
static const int STATIONS[] = {1, 5, 8};          /* ambulance bases */
static const int NUM_STATIONS = 3;
static const char *SEV_LABEL[] = {
    "", "CRITICAL", "EMERGENCY", "URGENT", "SEMI-URGENT", "NON-URGENT"
};

typedef struct {
    Graph graph;
    PriorityQueue queue;
    HashTable records;
    DispatchLog log;
    int next_id;
    long next_arrival;
} System;

/* ---------- input helpers ---------- */
static int read_line(const char *prompt, char *buf, size_t n) {
    printf("%s", prompt);
    if (!fgets(buf, (int)n, stdin)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

static int read_int(const char *prompt, int lo, int hi, int *out) {
    char line[64];
    for (;;) {
        if (!read_line(prompt, line, sizeof line)) return 0;
        char *end;
        long v = strtol(line, &end, 10);
        if (end != line && *end == '\0' && v >= lo && v <= hi) {
            *out = (int)v;
            return 1;
        }
        printf("  Please enter a number between %d and %d.\n", lo, hi);
    }
}

/* ---------- setup ---------- */
static void build_city(Graph *g) {
    graph_init(g, 10, LOC_NAMES);
    graph_add_road(g, 0, 1, 4);  graph_add_road(g, 0, 3, 2);
    graph_add_road(g, 0, 6, 5);  graph_add_road(g, 1, 2, 9);
    graph_add_road(g, 1, 3, 3);  graph_add_road(g, 2, 4, 6);
    graph_add_road(g, 3, 4, 5);  graph_add_road(g, 3, 7, 4);
    graph_add_road(g, 4, 5, 3);  graph_add_road(g, 5, 9, 4);
    graph_add_road(g, 6, 7, 3);  graph_add_road(g, 6, 8, 6);
    graph_add_road(g, 7, 8, 2);  graph_add_road(g, 7, 9, 7);
    graph_add_road(g, 8, 9, 5);
}

static int register_patient(System *s, const char *name, int severity, int loc) {
    Patient p;
    memset(&p, 0, sizeof p);
    p.id = s->next_id++;
    strncpy(p.name, name, NAME_LEN - 1);
    p.severity = severity;
    p.location = loc;
    p.arrival = s->next_arrival++;
    strcpy(p.status, "WAITING");
    if (!ht_insert(&s->records, p) || !pq_push(&s->queue, p)) {
        printf("  Error: could not register patient.\n");
        return -1;
    }
    return p.id;
}

/* ---------- features ---------- */
static void menu_register(System *s) {
    char name[NAME_LEN];
    int sev, loc;
    if (!read_line("Patient name: ", name, sizeof name)) return;
    if (name[0] == '\0') { printf("  Name cannot be empty.\n"); return; }
    printf("Severity: 1=Critical 2=Emergency 3=Urgent 4=Semi-urgent 5=Non-urgent\n");
    if (!read_int("Severity (1-5): ", 1, 5, &sev)) return;
    for (int i = 0; i < s->graph.n; i++) printf("  %d. %s\n", i, LOC_NAMES[i]);
    if (!read_int("Pickup location (0-9): ", 0, 9, &loc)) return;

    int id = register_patient(s, name, sev, loc);
    if (id > 0)
        printf("  Registered #%d %s [%s] at %s\n", id, name, SEV_LABEL[sev], LOC_NAMES[loc]);
}

static void menu_dispatch(System *s) {
    Patient p;
    if (!pq_pop(&s->queue, &p)) { printf("  No patients waiting.\n"); return; }

    int d_pat[MAX_LOC], prev_pat[MAX_LOC];
    graph_dijkstra(&s->graph, p.location, d_pat, prev_pat);

    int best = STATIONS[0];
    for (int i = 1; i < NUM_STATIONS; i++)
        if (d_pat[STATIONS[i]] < d_pat[best]) best = STATIONS[i];

    if (d_pat[best] == INF || d_pat[HOSPITAL] == INF) {
        printf("  No route available for %s!\n", p.name);
        return;
    }

    int d_st[MAX_LOC], prev_st[MAX_LOC];
    graph_dijkstra(&s->graph, best, d_st, prev_st);

    char to_patient[256], to_hospital[256];
    graph_path(&s->graph, prev_st, best, p.location, to_patient, sizeof to_patient);
    graph_path(&s->graph, prev_pat, p.location, HOSPITAL, to_hospital, sizeof to_hospital);

    int total = d_pat[best] + d_pat[HOSPITAL];
    double eta = total * 60.0 / AMBULANCE_SPEED_KMH;

    printf("\n=== AMBULANCE DISPATCHED ===\n");
    printf("Patient   : #%d %s  [%s] at %s\n", p.id, p.name, SEV_LABEL[p.severity], LOC_NAMES[p.location]);
    printf("Ambulance : from %s (%d km away)\n", LOC_NAMES[best], d_pat[best]);
    printf("To patient: %s\n", to_patient);
    printf("To hospital: %s (%d km)\n", to_hospital, d_pat[HOSPITAL]);
    printf("Total trip: %d km, about %.0f min at %.0f km/h\n\n", total, eta, AMBULANCE_SPEED_KMH);

    Patient *rec = ht_find(&s->records, p.id);
    if (rec) strcpy(rec->status, "DISPATCHED");
    log_append(&s->log, "#%d %s [%s] <- %s, %d km, ~%.0f min",
               p.id, p.name, SEV_LABEL[p.severity], LOC_NAMES[best], total, eta);
}

static void menu_queue(System *s) {
    if (s->queue.size == 0) { printf("  Queue is empty.\n"); return; }
    PriorityQueue tmp;
    if (!pq_clone(&s->queue, &tmp)) return;
    printf("\n  Waiting patients (next to be served first):\n");
    Patient p;
    int rank = 1;
    while (pq_pop(&tmp, &p))
        printf("  %2d. #%d %-12s %-12s @ %s\n", rank++, p.id, p.name,
               SEV_LABEL[p.severity], LOC_NAMES[p.location]);
    pq_free(&tmp);
}

static void menu_search(System *s) {
    int id;
    if (!read_int("Patient ID: ", 1, 999999, &id)) return;
    Patient *p = ht_find(&s->records, id);
    if (!p) { printf("  No patient with ID %d.\n", id); return; }
    printf("  #%d %s | %s | %s | status: %s\n", p->id, p->name,
           SEV_LABEL[p->severity], LOC_NAMES[p->location], p->status);
}

static void menu_route(System *s) {
    int a, b;
    for (int i = 0; i < s->graph.n; i++) printf("  %d. %s\n", i, LOC_NAMES[i]);
    if (!read_int("From (0-9): ", 0, 9, &a)) return;
    if (!read_int("To   (0-9): ", 0, 9, &b)) return;
    int dist[MAX_LOC], prev[MAX_LOC];
    char path[256];
    graph_dijkstra(&s->graph, a, dist, prev);
    if (graph_path(&s->graph, prev, a, b, path, sizeof path))
        printf("  %s  (%d km)\n", path, dist[b]);
    else
        printf("  No route.\n");
}

static void load_demo(System *s) {
    register_patient(s, "Asha",   3, 4);
    register_patient(s, "Ravi",   1, 5);
    register_patient(s, "Meena",  5, 9);
    register_patient(s, "Karthik",2, 8);
    register_patient(s, "Divya",  1, 2);
    register_patient(s, "Suresh", 4, 6);
    printf("  Loaded 6 demo patients.\n");
}

int main(void) {
    System s;
    build_city(&s.graph);
    if (!pq_init(&s.queue, 8)) return 1;
    ht_init(&s.records);
    log_init(&s.log);
    s.next_id = 1001;
    s.next_arrival = 1;

    for (;;) {
        printf("\n===== EMERGENCY RESPONSE SYSTEM =====\n");
        printf("1. Register patient\n2. Dispatch ambulance to most critical\n");
        printf("3. View waiting queue\n4. Search patient by ID\n");
        printf("5. Shortest route between locations\n6. Dispatch history\n");
        printf("7. Load demo patients\n0. Exit\n");
        int choice;
        if (!read_int("Choose: ", 0, 7, &choice) || choice == 0) break;
        switch (choice) {
            case 1: menu_register(&s); break;
            case 2: menu_dispatch(&s); break;
            case 3: menu_queue(&s); break;
            case 4: menu_search(&s); break;
            case 5: menu_route(&s); break;
            case 6: printf("\n  Dispatch log:\n"); log_print(&s.log); break;
            case 7: load_demo(&s); break;
        }
    }

    pq_free(&s.queue);
    ht_free(&s.records);
    log_free(&s.log);
    printf("Goodbye.\n");
    return 0;
}
