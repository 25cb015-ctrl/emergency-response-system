#ifndef PATIENT_H
#define PATIENT_H

#define NAME_LEN 50

/* Severity: 1 = Critical ... 5 = Non-urgent */
typedef struct {
    int  id;
    char name[NAME_LEN];
    int  severity;
    int  location;      /* node index in the city graph */
    long arrival;       /* arrival sequence number (tie-breaker) */
    char status[16];    /* WAITING / DISPATCHED */
} Patient;

#endif
