#ifndef	        PTASK_H
#define 	PTASK_H

#define	        NMAX    200 	// Numero massimo di task

//-----------------------------------------------------------------------
// Variabili Globali definte nella libreria
//-----------------------------------------------------------------------

struct  task_par {
        int 	arg;
        int 	period;
        int 	deadline;
        int 	priority;
        int 	dmiss;
	struct	timespec at;
	struct	timespec dl;
        };

struct          task_par        tp[NMAX];
struct	        sched_param	mypar;
pthread_t		        tid[NMAX];
pthread_attr_t                  myatt;

//-----------------------------------------------------------------------
// FUNZIONI
//-----------------------------------------------------------------------
// task_create
//-----------------------------------------------------------------------

int     task_create (void*(*task)(void *), int i, int per, int dline, int prio);

//-----------------------------------------------------------------------
// get_task_id
//-----------------------------------------------------------------------

int     get_task_id(void* p);

//-----------------------------------------------------------------------
// time_add_ms
//-----------------------------------------------------------------------

void    time_add_ms (struct timespec *t, int ms);

//-----------------------------------------------------------------------
// time_copy
//-----------------------------------------------------------------------

void    time_copy(struct timespec *td, struct timespec ts);

//-----------------------------------------------------------------------
// set_activation
//-----------------------------------------------------------------------

void    set_activation(int i);

//-----------------------------------------------------------------------
// wait_for_activation
//-----------------------------------------------------------------------

void    wait_for_activation(int i);

//-----------------------------------------------------------------------
// time_cmp
//-----------------------------------------------------------------------

int     time_cmp(struct timespec t1, struct timespec t2);

//-----------------------------------------------------------------------
// deadline_miss
//-----------------------------------------------------------------------

int     deadline_miss(int i);

//-----------------------------------------------------------------------
//  show_dmiss
//-----------------------------------------------------------------------

void    shoe_dmiss(int i);

//-----------------------------------------------------------------------
// get_dmiss
//-----------------------------------------------------------------------

int     get_dmiss(int i);

//-----------------------------------------------------------------------
// ptask_exit
//-----------------------------------------------------------------------

void    ptask_exit(int n);

//-----------------------------------------------------------------------
// ptask_exit_all
//-----------------------------------------------------------------------

void    ptask_exit_all();


#endif