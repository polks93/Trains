#include <stdbool.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_h

/**
 * Station manager task
 * Gestisce il traffico di tutti i treni all'interno della stazione eseguendo le seguenti operazioni
 * - Assegna il binario per nuovi treni in base al traffico attuale
 * - Invia ai singoli treni la posizione della prossima fermata sul loro percorso
 * - Quando ci sono più treni fermi allo stesso semaforo genera la coda
 * - Trasla le code quando il semaforo diventa verde
 * - Decide quale treno lascerà per primo la stazione
 * - Muove i binari mobili in modo coerente con la destinazione dei treni
 */
void *station_manager(void *p);

/**
 * Assegnazione del binario ai nuovi treni in base al traffico attuale
 */
void binary_assignment();

/**
 * Funzione che si occupa di muovere l'eventuale coda presente a uno dei semafori
 * che si trova prima di un binario mobile
 */
void move_semaphore_queue_in(int semId);

/**
 * Funzione che si occupa di muovere l'eventuale coda presente a una stazione
 */
void move_station_queue(int stationId);

/**
 * Train task. 
 * Gestisce tutto il movimento del treno:
 * - Attende l'asssegnazione del binario da parte della stazione
 * - Controlla il prossimo stop, che gli viene indicato dal task station manager
 * - Decide se rallentare, acellerare o muoversi alla massima velocità a seconda della condizione in cui si trova
 */
void *train(void *p);

/**
 * Inizializza i paramentri di un nuovo task treno
 */
void set_train_parameters(int i);

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
 * Controlla se il treno si trova in prossimità di un semaforo di un binario mobile.
 * Se il binario non è nella posizione richiesta dal treno, fa diventare il semaforo rosso
 * e sposta il binario nella posizione desiderata
 */
void checkSemaphoreIn(int trainId, int semId, int semStateRequired);

/**
 * Decide quale sarà il primo treno a lasciare la stazione lato SX,
 * dopo averlo fatto cambia lo status del semaforo a cui si trova quel treno
 */
void stationOutSx();

/**
 * Decide quale sarà il primo treno a lasciare la stazione lato DX
 * dopo averlo fatto cambia lo status del semaforo a cui si trova quel treno
 */
void stationOutDx();

/**
 * Controlla se il treno si trova in prossimità della stazione
 */
void checkStation (int trainId);

/**
 * Chiude tutti i thread attivi e esce dal programma
 */
void exit_all();


/**
 * Restituisce il segno dell'intero      
 */
int sign(int x);

#endif