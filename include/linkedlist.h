#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LogNode {
    char text[200];
    struct LogNode *next;
} LogNode;

/* Singly linked list used as the dispatch history log */
typedef struct {
    LogNode *head, *tail;
    int count;
} DispatchLog;

void log_init(DispatchLog *l);
void log_append(DispatchLog *l, const char *fmt, ...);
void log_print(const DispatchLog *l);
void log_free(DispatchLog *l);

#endif
