#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "linkedlist.h"

void log_init(DispatchLog *l) { l->head = l->tail = NULL; l->count = 0; }

void log_append(DispatchLog *l, const char *fmt, ...) {
    LogNode *n = malloc(sizeof(LogNode));
    if (!n) return;
    va_list args;
    va_start(args, fmt);
    vsnprintf(n->text, sizeof(n->text), fmt, args);
    va_end(args);
    n->next = NULL;
    if (l->tail) l->tail->next = n; else l->head = n;
    l->tail = n;
    l->count++;
}

void log_print(const DispatchLog *l) {
    if (!l->head) { printf("  (no dispatches yet)\n"); return; }
    int i = 1;
    for (LogNode *n = l->head; n; n = n->next)
        printf("  %2d. %s\n", i++, n->text);
}

void log_free(DispatchLog *l) {
    LogNode *n = l->head;
    while (n) { LogNode *next = n->next; free(n); n = next; }
    l->head = l->tail = NULL;
    l->count = 0;
}
