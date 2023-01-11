#include <stdbool.h>

#ifndef TRAINS_H
#define TRAINS_H

/**
 * Inizializza i paramentri di un nuovo task treno
 */
void set_train_parameters (int i);

/**
 * Ad ogni attivazione del task controlla se è stato assegnato un binario
 */
void wait_for_binary_assignment (int trainId);

/**
 * Check collisioni in ingresso alla stazione. Per evitarle genera il nuovo treno dopo 1000 ms dalla generazione 
 * del treno precedente con la stessa direzione
 */
void collision_check (int trainId);

/**
 * Macchina a stati associata al treno, gestisce il tipo di movimento che deve avere il treno a seconda dello stato
 */
int state_machine (int trainId, int direction, int curr_state);

/**
 * Stato che muove il treno a velocità massima e costante
 */
int state_go_fast (int trainId);

/**
 * Stato che fa rallentare il treno prima di una fermata
 */
int state_slow_down (int trainId, int direction);

/**
 * Stato del treno che si trova in coda a un semaforo o a una stazione
 */
int state_queue(int trainId, int direction);

/**
 * Stato del treno fermo a un semaforo o a una stazione quando è il primo della coda.
 * E' pronto a lasciare questo stato per SPEED_UP
 */
int state_stop (int trainId);

/**
 * Stato che fa acellerare il treno dopo una fermata
 */
int state_speed_up (int trainId);

/**
 * Funzione che decrementa la variabile globale che conta i treni presenti su ciascun binario, SE il treno
 * ha superato la stazione
 */
void clear_binary (int trainId, int binary);

/**
 * Funzione che controlla quando il treno esce dallo schermo, in modo da uscire dal ciclo e chiuedere il relativo task
 */
bool out_of_bound_check (int trainId, int direction);

 /** 
  * Funzione che si occupa di muovere il treno correttamente lungo il binario prestabilito
  */
void move (int trainId, float vel, float acc);

 /** 
  * Funzione che calcola di quanti pixel si deve muovere il treno ad ogni ciclo
  */
int find_step (int trainId, int vel, float acc);

 /** 
  * Funzione che riallinea i vagoni dopo che si sono mossi in diagonale
  */
void wagons_realignment (int trainId, int wagonId, int wagon_posx, int binary);

/**
 * Movimento del treno in diagonale verso l'alto
 */
void move_diag_up (int trainId, int wagonId, int step);

/**
 * Movimento del treno in diagonale verso il basso
 */
void move_diag_down (int trainId, int wagonId, int step);

/**
 * Movimento orrizontale del treno
 */
void move_forward (int trainId, int wagonId, int step);

/**
 * Train task. 
 * Gestisce tutto il movimento del treno:
 * - Attende l'asssegnazione del binario da parte della stazione
 * - Controlla il prossimo stop, che gli viene indicato dal task station manager
 * - Decide se rallentare, acellerare o muoversi alla massima velocità a seconda della condizione in cui si trova
 */
void *train (void *p);

/**
 * Restituisce il segno dell'intero
 */
int sign (int x);

#endif