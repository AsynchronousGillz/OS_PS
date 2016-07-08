#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "scheduler.h"

#define LINE_BUF 1024

#define debug
#define print

// <ProcessID>,<Arrival Time>,<Burst Duration>,<Priority>,<Start Tickets> - <End Tickets>
// Process priorities have a range of [1-50]

void initARR_s(ARR_s *a, size_t initialSize) {
    a->array = malloc(initialSize * sizeof(PID_s));
    a->used = 0;
    a->size = initialSize;
}

void insertARR_s(ARR_s *a, PID_s e) {
    if (a->used == a->size) {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(PID_s));
    }
    a->array[a->used++] = e;
}

void freeARR_s(ARR_s *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
    free(a);
}

ARR_s* copyARR_s(const ARR_s *a) {
    ARR_s *b = malloc(1 * sizeof(ARR_s));
    initARR_s(b, a->used);
    memcpy(b->array, a->array, (a->used * sizeof(PID_s)));
    b->used = a->used;
    return(b);
}

void insertTIK_s(PID_s *a, int s, int e) {
    a->t_nums++;
    TIK_s *new = malloc(1*sizeof(TIK_s));
    new->sta = s; new->end = e; new->next = NULL;
    if (a->tik_nums == NULL) {
        a->tik_nums = new; 
    }
}

// give from a to t
void addTIK_s(TIK_s *a, PID_s *b) {
    TIK_s *head = b->tik_nums;
    TIK_s *prev = a;
    for (; prev != NULL; prev = prev->next);
    prev->next = head;
    b->tik_nums = a;
}

TIK_s* get_TIK_s(PID_s *a) {
    TIK_s* ret = a->tik_nums;
    a->tik_nums = NULL;
    return(ret);
}

void removeTIK_s(TIK_s *a) {
    if (a == NULL) return;
    TIK_s *curr = a;
    while ((curr = a) != NULL) {
        a = a->next;
        free (curr);
    }
    free(a);
    a = NULL;
}

void print_pis(const PID_s t) {
    printf("| P%-10d%-10d%-10d%-10d",t.proc_ID, t.a_time, t.b_dur, t.priority);
}

void print_plo(const PID_s t, const int ticket) {
    printf("| P%-10d%-10d%-10d%-10d",t.proc_ID, ticket, t.b_dur, t.priority);
}

PID_s get_pid(const char *line, int num) {
    PID_s ret;
    ret.tik_nums = NULL;
    ret.line_num = num;
    int tik[2] = {0, 0};
    sscanf(line, "%d,%d,%d,%d,%d - %d\n", &ret.proc_ID, &ret.a_time, &ret.b_dur, &ret.priority, &tik[0], &tik[1]);
    insertTIK_s(&ret, tik[0], tik[1]);
    return(ret);
}

void deletePID_s(ARR_s *a, int o) {
    a->used--;
}

void print_menu(const char *name, int v) {
    for(int i = 74;i--;printf("="));
    printf("\n");
    printf("| %-71s|\n",name);
    for(int i = 74;i--;printf("-"));
    printf("\n");
    if (v == 2) {
        printf("| %-10s %-10s%-10s%-10s%-15s%-15s|\n","Process","AT","BT","RT","WT","TAT");
    } else if (v == 1) {
        printf("| %-10s %-10s%-10s%-10s%-15s%-15s|\n","Process","TI","BT","PR","WT","TAT");
    } else {
        printf("| %-10s %-10s%-10s%-10s%-15s%-15s|\n","Process","AT","BT","PR","WT","TAT");
    }
    for(int i = 74;i--;printf("-"));
    printf("\n");
}

void print_avg(const double zero, const double one, const double two) {
    for(int i = 74;i--;printf("-"));
    printf("\n");
    // AVG_WT, AVG_TAT, AVG_T
    printf("| %-60s %10.2lf|\n", "average waiting time: ", zero);
    printf("| %-60s %10.2lf|\n", "average turnaround time: ", one);
    printf("| %-60s %.8lf|\n", "throughput: ", two);
    for(int i = 74;i--;printf("="));
    printf("\n\n");
}

void print_tik(TIK_s *head) {
    for (TIK_s *o = head; o != NULL; o = o->next) {
        printf("| %-2d ,%-2d | ----> ", o->sta, o->end);
    }
    printf("\n");
}

int cpm_dur (const void *b, const void *a) {
   return ((((PID_s*)a)->b_dur)-(((PID_s*)b)->b_dur));
}

int cpm_time (const void *b, const void *a) {
   int ret = ((((PID_s*)a)->a_time)-(((PID_s*)b)->a_time));
   ret = (ret == 0)? cpm_dur(a, b): ret;
   return(ret);
}

int cpm_priority (const void *b, const void *a) {
    int ret = ((((PID_s*)a)->a_time)-(((PID_s*)b)->a_time));
    ret = (ret == 0)?((((PID_s*)a)->priority)-(((PID_s*)b)->priority)): ret;
    return(ret);
}

int cpm_lottery (const void *b, const void *a) {
    int ret = ((((PID_s*)a)->a_time)-(((PID_s*)b)->a_time));
    int sta = 0; int end = 0;      
    if (ret) {
        sta = 0; end = 0;      
        for (TIK_s *ti = ((PID_s*)a)->tik_nums; ti != NULL; ti = ti->next) {
            sta = ti->sta; end = ti->end;
        }
    }
    ret = (ret == 0)?(sta - end):ret;
    return(ret);
}

void swapPID_s(PID_s *one, PID_s *two) {
    PID_s temp = *one;
    *one = *two;
    *two = temp;
}

void find_sjf_pre (ARR_s *a, const int i, const int ct) {
    if (i == a->used) return;
    int sub = i;
    int i_time = (ct + a->array[i].b_dur);
    int next_time = i_time;
    for (int k = i; k >= 0; k--) {
        next_time = (ct + a->array[i].b_dur);
        if ((next_time+1 > 0) && (next_time < i_time)) {
            sub = k;
            i_time = next_time;
        }
    }
    swapPID_s(&a->array[i], &a->array[sub]);
}

void find_sjf_non (ARR_s *a, const int i, const int ct) {
    if (i == a->used) return;
    int sub = i;
    int i_time = (ct + a->array[i].b_dur);
    int next_time = i_time;
    for (int k = i; k >= 0; k--) {
        next_time = (ct + a->array[k].b_dur);
        if ((next_time+1 > 0) && (next_time < i_time)) {
            sub = k;
            i_time = next_time;
        }
    }
    swapPID_s(&a->array[i], &a->array[sub]);
}

void find_pri_non (ARR_s *a, const int i, const int ct) {
    if (i == a->used) return;
    int sub = i;
    int i_time = (ct + a->array[i].priority);
    int next_time = i_time;
    for (int k = i; k >= 0; k--) {
        next_time = (ct + a->array[k].priority);
        if ((next_time+1 > 0) && (next_time < i_time)) {
            sub = k;
            i_time = next_time;
        }
    }
    swapPID_s(&a->array[i], &a->array[sub]);
}

int find_loto_non (ARR_s *a, const int i, const int ticket) {
    if (i == a->used) return(1);
    for (int k = i; k >= 0; k--) {
        if (hasTicket(&a->array[k], ticket)) {
            swapPID_s(&a->array[i], &a->array[k]);
            return(0);
        }
    }
    return(1);
}

int hasTicket(const PID_s *a, int t) {
    for (TIK_s *o = a->tik_nums; o != NULL; o = o->next) {
        if ((o->sta < t) && (t < o->end)) {
            return(1);
        }
    }
    return(0);
}

//////////////////////////////////////////////////////////////////////////////

void FCFS(ARR_s *a) {
    qsort(a->array, a->used, sizeof(PID_s), cpm_time);
    double proc_val[4] = {0, 0, 0, 0}; // WT, TAT, TOTAL_WT, TOTAL_TAT
    int ct = a->array[0].a_time;

    print_menu("FCFS", 0);

    for (int i = (a->used-1); i >= 0; i--) {

        proc_val[0] = ct - a->array[i].a_time;
        proc_val[1] = proc_val[0] + a->array[i].b_dur;
        proc_val[3] += proc_val[0] + a->array[i].b_dur;
    
        proc_val[2] += (ct > a->array[i].a_time)? ct - a->array[i].a_time: 0;

        ct += a->array[i].b_dur;
    #ifdef print
        print_pis(a->array[i]);
        printf("%-15.1lf%-15.1lf|\n", proc_val[0], proc_val[1]);
    #endif
    }

    double avg[3] = {0, 0, 0}; // AVG_WT, AVG_TAT, AVG_T
    avg[0] = proc_val[2] / a->used;
    avg[1] = proc_val[3] / a->used;
    avg[2] = a->used / (a->array[a->used-1].a_time + proc_val[1]);
    
    print_avg(avg[0], avg[1], avg[2]);
}

//////////////////////////////////////////////////////////////////////////////

void SJF_non(ARR_s *a) {
    qsort(a->array, a->used, sizeof(PID_s), cpm_time);
    double proc_val[4] = {0, 0, 0, 0}; // WT, TAT, TOTAL_WT, TOTAL_TAT
    int size = a->used, last_a_time = 0;
    int ct = a->array[0].a_time;

    print_menu("SJF non-premtive", 0);
    
    for (int i = (a->used-1); i >= 0; i--) {
        proc_val[0] = 0; proc_val[1] = 0;

        proc_val[0] = (ct > a->array[i].a_time)? (ct - a->array[i].a_time): 0;
        proc_val[1] = proc_val[0] + a->array[i].b_dur;
        proc_val[3] += proc_val[0] + a->array[i].b_dur;
    
        proc_val[2] += (ct > a->array[i].a_time)? (ct - a->array[i].a_time): 0;

        ct += a->array[i].b_dur;
    #ifdef print
        print_pis(a->array[i]);
        printf("%-15.1lf%-15.1lf|\n", proc_val[0], proc_val[1]);
    #endif

        last_a_time = a->array[i].a_time;
        deletePID_s(a , i);
        find_sjf_non(a, i, ct);
        
    }

    double avg[3] = {0, 0, 0}; // AVG_WT, AVG_TAT, AVG_T
    avg[0] = proc_val[2] / size;
    avg[1] = proc_val[3] / size;
    avg[2] = size / (last_a_time + proc_val[1]);

    print_avg(avg[0], avg[1] ,avg[2]);
}

//////////////////////////////////////////////////////////////////////////////

void SJF_pre(ARR_s *a) {
    qsort(a->array, a->used, sizeof(PID_s), cpm_time);
    double proc_val[4] = {0, 0, 0, 0}; // WT, TAT, TOTAL_WT, TOTAL_TAT
    int size = a->used, last_a_time = 0;
    int ct = a->array[0].a_time;

    print_menu("SJF premtive", 0);

    for (int i = (a->used-1); i >= 0; i--) {
        proc_val[0] = 0, proc_val[1] = 0;

        find_sjf_pre(a, i, ct);
        proc_val[0] = (ct > a->array[i].a_time)? (ct - a->array[i].a_time): 0;
        proc_val[1] = proc_val[0] + a->array[i].b_dur;
        proc_val[3] += proc_val[0] + a->array[i].b_dur;
    
        proc_val[2] += (ct > a->array[i].a_time)? (ct - a->array[i].a_time): 0;
        a->array[i].b_dur--;
    #ifdef print
        print_pis(a->array[i]);
        printf("%-15.1lf%-15.1lf|\n", proc_val[0], proc_val[1]);
    #endif

        last_a_time = a->array[i].a_time;
        deletePID_s(a , i);
        find_sjf_non(a, i, ct);
        
    }

    double avg[3] = {0, 0, 0}; // AVG_WT, AVG_TAT, AVG_T
    avg[0] = proc_val[2] / size;
    avg[1] = proc_val[3] / size;
    avg[2] = size / (last_a_time + proc_val[1]);

    print_avg(avg[0], avg[1] ,avg[2]);
}

//////////////////////////////////////////////////////////////////////////////

void Priority(ARR_s *a) {
    qsort(a->array, a->used, sizeof(PID_s), cpm_priority);
    double proc_val[4] = {0, 0, 0, 0}; // WT, TAT, TOTAL_WT, TOTAL_TAT
    int size = a->used, last_a_time = 0;
    int ct = a->array[0].a_time;

    print_menu("Priority", 0);

    for (int i = (a->used-1); i >= 0; i--) {

        proc_val[0] = ct - a->array[i].a_time;
        proc_val[1] = proc_val[0] + a->array[i].b_dur;
        proc_val[3] += proc_val[0] + a->array[i].b_dur;
    
        proc_val[2] += (ct > a->array[i].a_time)? ct - a->array[i].a_time: 0;

        ct += a->array[i].b_dur;
    #ifdef print
        print_pis(a->array[i]);
        printf("%-15.1lf%-15.1lf|\n", proc_val[0], proc_val[1]);
    #endif

        last_a_time = a->array[i].a_time;
        deletePID_s(a , i);
        find_pri_non(a, i, ct);
    }

    double avg[3] = {0, 0, 0}; // AVG_WT, AVG_TAT, AVG_T
    avg[0] = proc_val[2] / size;
    avg[1] = proc_val[3] / size;
    avg[2] = size / (last_a_time + proc_val[1]);

    print_avg(avg[0], avg[1], avg[2]);
}

//////////////////////////////////////////////////////////////////////////////

void Lottery(ARR_s *a, int r) {
    srand(r);
    qsort(a->array, a->used, sizeof(PID_s), cpm_lottery);
    double proc_val[4] = {0, 0, 0, 0}; // WT, TAT, TOTAL_WT, TOTAL_TAT
    int size = a->used, last_a_time = 0;
    int ct = a->array[0].a_time;
    
    print_menu("Lottery", 1);

    int ticket = rand()%100000;

    for (int i = (a->used-1); i >= 0; i--) {

        do {
            ticket = rand()%100000;
        } while (find_loto_non(a, i, ticket));

        proc_val[0] = ct;
        proc_val[1] = proc_val[0] + a->array[i].b_dur;
        proc_val[3] += proc_val[0] + a->array[i].b_dur;
    
        proc_val[2] += (ct > a->array[i].a_time)? ct - a->array[i].a_time: 0;

        ct += a->array[i].b_dur;

    #ifdef print
        print_plo(a->array[i], ticket);
        printf("%-15.1lf%-15.1lf|\n", proc_val[0], proc_val[1]);
    #endif

        last_a_time = a->array[i].a_time;
        removeTIK_s(get_TIK_s(&a->array[i]));
        deletePID_s(a, i);
    }
    double avg[3] = {0, 0, 0}; // AVG_WT, AVG_TAT, AVG_T
    avg[0] = proc_val[2] / size;
    avg[1] = proc_val[3] / size;
    avg[2] = size / (last_a_time + proc_val[1]);

    print_avg(avg[0], avg[1], avg[2]);
}

//////////////////////////////////////////////////////////////////////////////

ARR_s* get_array(FILE *s) {
    ARR_s *list = calloc(1, sizeof(PID_s));
    initARR_s(list, 64);
    char *line = calloc(LINE_BUF, sizeof(char));
    for (int i = 0; fgets(line, LINE_BUF, s); i++) {
        PID_s val = get_pid(line, i);
        insertARR_s(list, val);
    }
    free(line);
    return(list);
}

void usage(char *name) {
    fprintf(stderr, "Usage: %s <input file> <random_seed>\n", name);
}

int main(int argv, char** argc) {

    if (argv != 3) {
        usage(argc[0]);
        return(1);
    }
    int str_size = strlen(argc[2]);
    for (int o = 0; o < str_size; ++o){
        if (!(isdigit(argc[2][o]))) {
            fprintf(stderr, "%s Error: %s not a valid input.\n", argc[0], argc[2]);
            return(1);
        }
    } 

    int r = atoi(argc[2]);

    printf("Starting %s on file %s\n", argc[0], argc[1]);
    FILE* stream = fopen(argc[1], "r");
    if (stream == NULL) {
        fprintf(stderr, "Error: could not open file to read.\n");
        return(1);
    }
    
    ARR_s* M = get_array(stream);
    fclose(stream);

    qsort(M->array, M->used, sizeof(PID_s), cpm_time);
    ARR_s *A = copyARR_s(M);
    FCFS(A);
    freeARR_s(A);
    
    ARR_s *B = copyARR_s(M);
    SJF_non(B);
    freeARR_s(B);

    ARR_s *C = copyARR_s(M);
    SJF_pre(C);
    freeARR_s(C);
    
    ARR_s *D = copyARR_s(M);
    Priority(D);
    freeARR_s(D);

    ARR_s *E = copyARR_s(M);
    Lottery(E, r);
    freeARR_s(E);

    freeARR_s(M);
    
}
