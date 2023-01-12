#ifndef     STATION_H
#define     STATION_H

/**
 * Assegnazione del binario ai nuovi treni in base al traffico attuale
 */
void binary_assignment();

/**
 * Politica di individuazione del binario migliore
 */
int find_best_binary (int train_direction, int train_priority);

/**
 * Identifica la prossima fermata di tutti i treni precedenti alla stazione
 */
void find_next_stop();

/**
 * Controlla tutti i semafori sul percorso del treno prima della stazione
 */
void check_all_semaphores_before_station(int trainId, int binary);

/**
 * Controlla se il treno si trova in prossimità di un semaforo di un binario mobile.
 * Se il binario non è nella posizione richiesta dal treno, fa diventare il semaforo rosso
 * e sposta il binario nella posizione desiderata
 */
void check_semaphore_in(int trainId, int semId, int semStateRequired);

/**
 * Funzione che controlla se il treno trainId si trova nel range di frenata del semaforo semId:
 * true: si trova nel range
 * false: non si trova nel range
 */
bool check_semaphore_range(int trainId, int semId);

/**
 * Azioni da eseguire quando il treno si deve fermare al semaforo
 */
void stop_at_semaphore(int trainId, int semId, int SemRequiredState);

/**
 * Gestisce le code dei semafori sugli scambi e delle stazioni
 */
void queue_manager();

/**
 * Funzione che si occupa di muovere l'eventuale coda presente a uno dei semafori
 * che si trova prima di un binario mobile
 */
void move_semaphore_queue (int semId);

/**
 * Funzione che si occupa di muovere l'eventuale coda presente a una stazione
 */
void move_station_queue(int stationId);

/**
 * Muove i binari mobili prima della stazione
 */
void move_trail_before_station (int semId, int SemRequiredState);

/**
 * Manda il sengale al primo treno in coda che può partire e aggiorna la coda del semaforo
 */
void move_first_train_in_sem_queue (int semId);

/**
 * Funzione che controlla se il treno trainId si trova nel range di fermata della stazione a cui
 * è assegnato
 */
void check_station (int trainId);

/**
 * Funzione che controlla se il treno trainId si trova nel range di frenata della sua stazione:
 * true: si trova nel range
 * false: non si trova nel range
 */
bool check_station_range(int trainId);

/**
 * Funzione che controlla se il timer avviato quando un semaforo della stazione diventa rosso
 * è già stato inizializzato oppure no
 */
bool check_init_red_time (int trainId);

/**
 * Funzione che inizializza il timer a partire dal primo sem che diventa rosso in stazione
 */
void red_time_initialize (int trainId);

/**
 * Azioni da eseguire quando il treno si deve fermare al semaforo
 */
void stop_at_station(int trainId);

/**
 * Decide quale sarà il primo treno a lasciare la stazione lato SX.
 * Dopo averlo fatto cambia lo status del semaforo a cui si trova quel treno e muove i binari
 * nella posizione appropriata
 */
void manage_station_out_SX();

/**
 * Decide quale sarà il primo treno a lasciare la stazione lato DX.
 * Dopo averlo fatto cambia lo status del semaforo a cui si trova quel treno e muove i binari
 * nella posizione appropriata
 */
void manage_station_out_DX();

/**
 * Controlla se è il momento di far diventare verde il semaforo della stazione col treno a 
 * massima priorità tra quelli con la stessa direzione
 */
bool check_green_time (int direction);

/**
 * Cerca il treno a priorità maggiore tra quelli in attesa tra tutte le stazioni
 */
int search_for_max_prio_train (int direction);


/**
 * Manda il comando per muovere i binari mobili in uscita dalla stazione
 */
void send_command_to_move_trails_out (int direction, int trainId);

/**
 * Controlla se ci sono altre stazioni in attesa
 */
void check_other_station_waiting(int direction);

/**
 * Legge se è stato inviato il segnale di muovere i binari
 */
bool read_command_to_move_trails (int direction);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_0(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_1(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_2(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_3(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_4(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_5(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_6(int posx);

/**
 * Funzione che muove i binari nella posizione corretta a seconda del binario da cui
 * sta partendo il treno
 */
void move_trails_out_bin_7(int posx);

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

#endif