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
void wait_for_binary_assignement (int trainId);

/**
 * Check collisioni in ingresso alla stazione. Per evitarle genera il nuovo treno dopo 1000 ms dalla generazione 
 * del treno precedente con la stessa direzione
 */
void collision_check (int trainId);

int state_machin (int trainId, int direction, int curr_state);

int state_go_fast (int trainId);

int state_slow_down (int trainId, int direction);

int state_queue(int trainId, int direction);

int state_stop (int trainId);

int state_speed_up (int trainId);

void clear_binary (int trainId, int binary);

bool out_of_bound_check(int trainId, int direction);

/**
 * Train task. 
 * Gestisce tutto il movimento del treno:
 * - Attende l'asssegnazione del binario da parte della stazione
 * - Controlla il prossimo stop, che gli viene indicato dal task station manager
 * - Decide se rallentare, acellerare o muoversi alla massima velocità a seconda della condizione in cui si trova
 */
void *train(void *p);

 /** 
  * Si occupa di muovere il treno correttamente lungo il binario prestabilito
  */
void move(int trainId, float vel, float acc);

/**
 * Movimento del treno in diagonale verso l'alto
 */
void move_diag_up(int i, int j, int inc);

/**
 * Movimento del treno in diagonale verso il basso
 */
void move_diag_down(int i, int j, int inc);

/**
 * Movimento orrizontale del treno
 */
void move_forward(int i, int j, int inc);

/**
 * Chiude tutti i thread attivi e esce dal programma
 */
void exit_all();

/**
 * Restituisce il segno dell'intero
 *         
 */
int sign(int x);

#endif