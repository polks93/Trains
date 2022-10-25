#include <stdio.h>
#include <pthread.h>
#include "ptask.h"



//------------------------------------------------------------------------------
// FUNZIONE task_create
//
// crea un nuovo thread a partire dalla funzione che deve svolgere,
// il suo id, il suo periodo, la sua deadline e la sua priorità.
// Tutte queste informazioni vengono salvate nella struttura tp[]
//------------------------------------------------------------------------------

int     task_create (void*(*task)(void *), int i, int per, int dline, int prio) {
int		tret;
	
	if (i > NMAX ) return -1;		//Messaggio di errore se la dim
						      		// supera il valore max
        tp[i].arg = i;
        tp[i].period = per;
        tp[i].deadline = dline;
        tp[i].priority = prio;
        tp[i].dmiss = 0;
	
	pthread_attr_init(&myatt);
	pthread_attr_setinheritsched(&myatt, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&myatt, SCHED_FIFO);
	mypar.sched_priority = tp[i].priority;
	pthread_attr_setschedparam(&myatt, &mypar);
    tret = pthread_create(&tid[i], NULL, task, (void*)(&tp[i]));
	pthread_attr_destroy(&myatt);
	
        
	return tret;
}

//------------------------------------------------------------------------------
// FUNZIONE get_task_id
//
// Dato un puntatore a una struttura del tipo
// task_par, restituisce una variabile intera
// pari all'id del task
//------------------------------------------------------------------------------

int     get_task_id(void* p)
{
	struct  task_par *tpar;
	tpar = (struct task_par *)p;
    return tpar->arg;
}

//------------------------------------------------------------------------------
// FUNZIONE get_task_per
//
// Dato un puntatore a una struttura del tipo
// task_par, restituisce una variabile intera
// pari al periodo del task
//------------------------------------------------------------------------------
int     get_task_per(void* p)
{
	struct  task_par *tpar;
	tpar = (struct task_par *)p;
    return tpar->period;
}

//------------------------------------------------------------------------------
// FUNZIONE time_add_ms
//
// Aggiunge un valore in ms alla struttura 
// temporale puntata dal puntatore t
//------------------------------------------------------------------------------

void time_add_ms (struct timespec *t, int ms)
{
	t->tv_sec  += ms/1000;
	t->tv_nsec += (ms%1000)*1000000;
	if (t->tv_nsec > 1000000000) {
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}

//------------------------------------------------------------------------------
// FUNZIONE time_copy
//
// Copia la struttura temporale ts nella
// struttura temporale puntata da td
//------------------------------------------------------------------------------

void time_copy(struct timespec *td, struct timespec ts)
{
	td->tv_sec = ts.tv_sec;
	td->tv_nsec = ts.tv_nsec;
}

//------------------------------------------------------------------------------
// FUNZIONE set_activation
//
// legge il tempo corrente e computa il prossimo 
// activation time e la deadline assoluta del task id
//------------------------------------------------------------------------------

void set_activation(int i)
{
struct timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        time_copy(&(tp[i].at), t);
        time_copy(&(tp[i].dl), t);
        time_add_ms(&(tp[i].at), tp[i].period);
        time_add_ms(&(tp[i].dl), tp[i].deadline);
}

//------------------------------------------------------------------------------
// FUNZIONE wait_for_activation
//
// sospende la chimata al thread fino alla prossima
// attivazione e, quando svegliato, aggiorna ativation time e 
// la deadline assoluta
//------------------------------------------------------------------------------

void 	wait_for_activation(int i) {

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(tp[i].at), NULL);
		time_add_ms(&(tp[i].at), tp[i].period);
    	time_add_ms(&(tp[i].dl), tp[i].period);
}

//------------------------------------------------------------------------------
// FUNZIONE time_cmp
//
// compara due strutture temporali t1 e t2 e da i seguenti risultati:
//  1 se t1 > t2
// -1 se t1 < t2 
//  0 se t1 = t2 (improbabile)
//------------------------------------------------------------------------------

int		time_cmp(struct timespec t1, struct timespec t2) {
		if (t1.tv_sec > t2.tv_sec) return 1;
		if (t1.tv_sec < t2.tv_sec) return -1;
		if (t1.tv_nsec > t2.tv_nsec) return 1;
		if (t1.tv_nsec < t2.tv_nsec) return -1;
		return 0;
}


//------------------------------------------------------------------------------
// FUNZIONE deadline_miss
//
// Se c'è stato un deadline miss, aumenta il contatore del task
// e da come valore in uscita 1
//------------------------------------------------------------------------------

int		deadline_miss(int i) {
struct 	timespec now;

		clock_gettime(CLOCK_MONOTONIC, &now);
		if (time_cmp(now, tp[i].dl) > 0) {
			tp[i].dmiss++;
			return 1;
		}
		return 0;
}

//------------------------------------------------------------------------------
// FUNZIONE show_dmiss
// stampa sul terminale il numero di deadline miss accumulati
// dal task "i"
//------------------------------------------------------------------------------

void	show_dmiss(int i) {

	printf("%d \n", tp[i].dmiss);
}

//------------------------------------------------------------------------------
// FUNZIONE get_dmiss
// quando richiamata restituisce il numero di deadline miss
// accumulati dal task "i"
//------------------------------------------------------------------------------

int		get_dmiss(int i) {
	return tp[i].dmiss;
}

//------------------------------------------------------------------------------
// FUNZIONE ptask_exit
//
// chiude tutti i task da 1 a n
//------------------------------------------------------------------------------

void	ptask_exit(int i) {

		pthread_cancel(tid[i]);
}

//------------------------------------------------------------------------------
// FUNZIONE ptask_exit_all
//
// chiude tutti i task attivi
//------------------------------------------------------------------------------

void 	ptask_exit_all() {
	int i;
	for (i = 0; i <= NMAX; i++) {
		pthread_cancel(tid[i]);
	}
	printf("All tasks succesfully closed! Goodbye core dump :) \n");
}