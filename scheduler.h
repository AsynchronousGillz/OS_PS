/*! \file scheduler.h
    \brief Contains the PID_s struct declaration (header)

*/
#ifndef SCH_H_INCLUDED
#define SCH_H_INCLUDED

//! <ProcessID>,<Arrival Time>, <Burst Duration>, <Priority>, <Number of Tickets>
/*! 
  Not all fields are used by all scheduling algorithms. For example, for FCFS
  you only need the process IDs, arrival times, and burst durations. All processes in your
  input files will be provided a unique process ID. The arrival times and burst durations
  are integers. Process priorities have a range of [1-50]; the lower this number, the
  higher the priority i.e. a process with priority=2 has a higher priority than a process with
  priority=2. The range of tickets fields specifies the range of tickets assigned to that
  process; no ticket will be assigned to more than 1 process. 
*/

typedef struct array {
    struct processes *array;
    size_t used;
    size_t size;
} ARR_s;

// Found on stack overflow by searching on google : dynamic growing arrays in c

typedef struct tickets {
    int sta;
    int end;
    struct tickets *next;
} TIK_s;

typedef struct processes {
    int line_num;
    int proc_ID;
    int a_time;
    int b_dur;
    int priority;
    TIK_s *tik_nums;
    int t_nums;
} PID_s;

void initARR_s(ARR_s *a, size_t initialSize);

void insertARR_s(ARR_s *a, PID_s e);

void freeARR_s(ARR_s *a);

ARR_s* copyARR_s(const ARR_s *a);

void insertTIK_s(PID_s *a, int s, int e);

// give from a to t
void addTIK_s(TIK_s *b, PID_s *a);

TIK_s* get_TIK_s(PID_s *a);

void removeTIK_s(TIK_s *a);

void set_pid(PID_s *ret, int run_time);

void print_pir(const PID_s t);

void print_pis(const PID_s t);

void print_plo(const PID_s t, const int ticket);

void print_pct(const PID_s t, const int ct);

PID_s get_pid(const char *line, int num);

void deletePID_s(ARR_s *a, int o);

void print_menu(const char *name, int v);

void print_avg(const double zero, const double one, const double two);

int cpm_dur (const void *a, const void *b);

int cpm_time (const void *a, const void *b);

int cpm_priority (const void *a, const void *b);

int cpm_lottery (const void *a, const void *b);

void swapPID_s(PID_s *one, PID_s *two);

void find_sjf_non (ARR_s *a, const int i, const int ct);

void find_sjf_pre (ARR_s *a, const int i, const int ct);

void find_pri_non (ARR_s *a, const int i, const int ct);

int find_loto_non (ARR_s *a, const int i, const int ticket);

int hasTicket(const PID_s *a, int t);

//////////////////////////////////////////////////////////////////////////////

void FCFS(ARR_s *a);

void SJF_non(ARR_s *a);

void SJF_pre(ARR_s *a);

void Priority(ARR_s *a);

void Lottery(ARR_s *a, int r);

//////////////////////////////////////////////////////////////////////////////

ARR_s* get_array(FILE *s);

void usage(char *name);


#endif // SCH_H_INCLUDED
