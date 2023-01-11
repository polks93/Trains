#include <allegro.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>

#include "ptask.h"
#include "init.h"
#include "station.h"

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE binary_assignment
//-------------------------------------------------------------------------------------------------------------------------

void binary_assignment(){

    bool    binary_assigned;
    int     i;
    int     bin;
    int     trainId;
    int     priority;
    int     direction;
    int     ready_trains_num_local;
 
    // Salvo il valore dei treni in attesa del binario
    pthread_mutex_lock(&ready_trains_num_mutex);
    ready_trains_num_local = ready_trains_num;
    pthread_mutex_unlock(&ready_trains_num_mutex);

    if(ready_trains_num_local > 0){

        // Assegnamento binario per ogni treno pronto
        for (i = 0; i < ready_trains_num_local; i++){
            
            // Id del prossimo treno a cui assegnare un binario
            pthread_mutex_lock(&last_assigned_train_id_mutex);
            trainId = last_assigned_train_id + 1;
            pthread_mutex_unlock(&last_assigned_train_id_mutex);

            // Dati del treno a cui va assegnato il binario
            pthread_mutex_lock(&train_par[trainId].mutex);
            binary_assigned = train_par[trainId].binary_assigned;
            priority        = train_par[trainId].priority;
            direction       = train_par[trainId].direction;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            if(binary_assigned == false) {
                
                // Politica di assegnazione del binario
                bin = find_best_binary(direction, priority);

                // Update della struttra dati del treno
                pthread_mutex_lock(&train_par[trainId].mutex);
                train_par[trainId].binary             = bin;
                train_par[trainId].binary_occupied    = true;
                train_par[trainId].binary_assigned    = true;
                pthread_mutex_unlock(&train_par[trainId].mutex);

                // Update dell'Id dell'ultimo treno assegnato
                pthread_mutex_lock(&last_assigned_train_id_mutex);
                last_assigned_train_id = trainId;
                pthread_mutex_unlock(&last_assigned_train_id_mutex);

                // Update della variabile che contiene il numero di treni in attesa
                pthread_mutex_lock(&ready_trains_num_mutex);
                ready_trains_num--;
                pthread_mutex_unlock(&ready_trains_num_mutex);

                // Incremento il numero di treni che andranno alla stessa stazione
                pthread_mutex_lock(&trains_in_binary_mutex);
                trains_in_binary[bin]++;   
                pthread_mutex_unlock(&trains_in_binary_mutex);                
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE find_best_binary()
//-------------------------------------------------------------------------------------------------------------------------

int find_best_binary (int train_direction, int train_priority){

    int bin;

    pthread_mutex_lock(&trains_in_binary_mutex);
    switch (train_direction) {

    // Assegnamento binario per direzione SX-DX
    case FROM_SX:                       
        
        switch (train_priority) {

        // Treni a HP prediligono i binari più vicini al centro 
        case HIGH_PRIO:
            if      (trains_in_binary[3] <= trains_in_binary[2])        bin = 3;
            else if (trains_in_binary[2] <= trains_in_binary[1])        bin = 2;
            else if (trains_in_binary[1] <= trains_in_binary[0])        bin = 1;
            else                                                        bin = 0;
            break;

        // Treni a MP non possono andare sul binario centrare
        case MEDIUM_PRIO:
            if      (trains_in_binary[2] <= trains_in_binary[1])        bin = 2;
            else if (trains_in_binary[1] <= trains_in_binary[0])        bin = 1;
            else                                                        bin = 0;
            break;

        // Treni a LP possono andare solo sui due binari esterni
        case LOW_PRIO:
            if (trains_in_binary[1] <= trains_in_binary[0])             bin = 1;
            else                                                        bin = 0;
            break;

        default:
            break;
        }
        break;

    // Assegnamento binario per direzione DX-SX
    case FROM_DX:                       
        
        switch (train_priority) {

        // Treni a HP prediligono i binari più vicini al centro 
        case HIGH_PRIO:
            if      (trains_in_binary[4] <= trains_in_binary[5])        bin = 4;
            else if (trains_in_binary[5] <= trains_in_binary[6])        bin = 5;
            else if (trains_in_binary[6] <= trains_in_binary[7])        bin = 6;
            else                                                        bin = 7;
            break;

        // Treni a MP non possono andare sul binario centrare
        case MEDIUM_PRIO:
            if      (trains_in_binary[5] <= trains_in_binary[6])        bin = 5;
            else if (trains_in_binary[6] <= trains_in_binary[7])        bin = 6;
            else                                                        bin = 7;
            break;

        // Treni a LP possono andare solo sui due binari esterni
        case LOW_PRIO:
            if (trains_in_binary[6] <= trains_in_binary[7])             bin = 6;
            else                                                        bin = 7;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
    pthread_mutex_unlock(&trains_in_binary_mutex);

    return bin;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE find_next_stop()
//-------------------------------------------------------------------------------------------------------------------------

void find_next_stop(){
    bool run;
    int trainId;
    int binary;

    // GESTIONE TRENI
    // Controlla la prossima fermata di ogni treno attivo in base al binario
    for (trainId = 1; trainId < TMAX; trainId++){

        pthread_mutex_lock(&train_par[trainId].mutex);
        run = train_par[trainId].run;
        pthread_mutex_unlock(&train_par[trainId].mutex);

        if (run == true){

            pthread_mutex_lock(&train_par[trainId].mutex);
            binary                          = train_par[trainId].binary;
            train_par[trainId].checked      = false;               // Flag per indicare che non è ancora stato trovato il prossimo stop
            pthread_mutex_unlock(&train_par[trainId].mutex);

            // CHECK DI TUTTI I SEMAFORI PRIMA DELLA STAZIONE
            check_all_semaphores_before_station(trainId, binary);

            // CHECK STAZIONE
            check_station(trainId);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_all_semaphores_before_station();
//-------------------------------------------------------------------------------------------------------------------------

void check_all_semaphores_before_station(int trainId, int binary){
    
    switch(binary){
    //check_semaphore_in (Id del treno, Id dei binari mobili da attraversare, posizione richiesta del binario mobile)
    case(0):
        check_semaphore_in(trainId, 0, TRAIL_ON);
        break;

    case(1):
        check_semaphore_in(trainId, 0, TRAIL_OFF);
        check_semaphore_in(trainId, 1, TRAIL_ON);
        break;

    case(2):
        check_semaphore_in(trainId, 0, TRAIL_OFF);
        check_semaphore_in(trainId, 1, TRAIL_OFF);
        check_semaphore_in(trainId, 2, TRAIL_ON);
        break;

    case(3):
        check_semaphore_in(trainId, 0, TRAIL_OFF);
        check_semaphore_in(trainId, 1, TRAIL_OFF);
        check_semaphore_in(trainId, 2, TRAIL_OFF);
        break;

    case(4):
        check_semaphore_in(trainId, 11, TRAIL_OFF);
        check_semaphore_in(trainId, 10, TRAIL_OFF);
        check_semaphore_in(trainId, 9,  TRAIL_OFF);
        break; 

    case(5):
        check_semaphore_in(trainId, 11, TRAIL_OFF);
        check_semaphore_in(trainId, 10, TRAIL_OFF);
        check_semaphore_in(trainId, 9,  TRAIL_ON);
        break; 

    case(6):
        check_semaphore_in(trainId, 11, TRAIL_OFF);
        check_semaphore_in(trainId, 10, TRAIL_ON);
        break; 

    case(7):
        check_semaphore_in(trainId, 11, TRAIL_ON);
        break;

    default:
        break;   
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_semaphore_in
//-------------------------------------------------------------------------------------------------------------------------
void check_semaphore_in(int trainId, int semId, int SemRequiredState){

    bool    sem_status;
    bool    checked;
    bool    sem_passed;
    bool    semaphore_flag;
    bool    train_in_sem;
    bool    first_of_queue;
    int     trail_state;

    pthread_mutex_lock(&train_par[trainId].mutex);
    checked         = train_par[trainId].checked;
    sem_passed      = train_par[trainId].sem_passed[semId];
    pthread_mutex_unlock(&train_par[trainId].mutex);

    first_of_queue  = false;

    if (checked == false) {
        
        // Controllo se il treno si trova nel range di frenata del semaforo
        train_in_sem = check_semaphore_range(trainId, semId);

        // Codice eseguito ad ogni ciclo se il treno si trova nel range del semaforo e non lo ha ancora superato
        if (train_in_sem == true && sem_passed == false) {
            
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].checked  = true;                                 // Flag per evitare di controllare gli altri semafori
            semaphore_flag              = train_par[trainId].semaphore_flag;    // Leggo la flag che indica se è la prima volta che entro nel range
            pthread_mutex_unlock(&train_par[trainId].mutex);

            // Leggo status del binario mobile e del semaforo
            pthread_mutex_lock(&semaphores[semId].mutex);
            trail_state     = semaphores[semId].trail_state;
            sem_status      = semaphores[semId].status;
            pthread_mutex_unlock(&semaphores[semId].mutex);

            // Controllo se il binario non è nella poszione corretta o il semaforo è rosso, in
            // entrambi i casi il treno deve iniziare la frenata
            if (trail_state != SemRequiredState || sem_status == false )    stop_at_semaphore(trainId, semId, SemRequiredState);
        }
    } 
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_semaphore_range
//-------------------------------------------------------------------------------------------------------------------------
bool check_semaphore_range (int trainId, int semId){

    bool train_in_sem;
    int posx;
    int direction;
    int xPointIn;
    int xPointOut;

    // Leggo le informazioni del treno dalla sua struttura dati
    pthread_mutex_lock(&train_par[trainId].mutex);
    posx            = train_par[trainId].posx;
    direction       = train_par[trainId].direction;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // Leggo le informazioni del semaforo dalla sua struttura dati
    pthread_mutex_lock(&semaphores[semId].mutex);
    xPointIn    = semaphores[semId].xPointIn;
    xPointOut   = semaphores[semId].xPointOut;
    pthread_mutex_unlock(&semaphores[semId].mutex);

    // Flag inizializzata al valore false
    train_in_sem = false;

    // A seconda della direzione aggiorno il flag per indicare se mi trovo nel range del semaforo
    switch (direction) {
    
    case FROM_SX:
        if (posx > xPointIn && posx < xPointOut)    train_in_sem = true;
        break;
    
    case FROM_DX:
        if (posx < xPointIn && posx > xPointOut)    train_in_sem = true;
        break;

    default:
        break;
    }

    return train_in_sem;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE stop_at_semaphore();
//-------------------------------------------------------------------------------------------------------------------------

void stop_at_semaphore(int trainId, int semId, int SemRequiredState) {

    bool    queue;
    int     direction;

    // Leggo le info necessarie dalla struttura dati del treno
    pthread_mutex_lock(&train_par[trainId].mutex);
    direction   = train_par[trainId].direction;
    queue       = train_par[trainId].queue;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // Eseguito solo la prima volta che il treno entra nel range
    if (queue == false) {
        
        // Aggiorno la struttra dati del treno per indicare la prossima fermata 
        pthread_mutex_lock(&train_par[trainId].mutex);
        train_par[trainId].semaphore_flag   = true;
        train_par[trainId].queue            = true;
        train_par[trainId].pos_in_queue     = semaphores[semId].queue;
        train_par[trainId].stop_x           = semaphores[semId].xPointStop;
        train_par[trainId].stop_id          = semId;
        train_par[trainId].stop_type        = SEMAPHORE;
        pthread_mutex_unlock(&train_par[trainId].mutex);

        // Aggiorno la struttra dati del semaforo per aggiungere il treno in coda
        pthread_mutex_lock(&semaphores[semId].mutex);
        semaphores[semId].queue_list[semaphores[semId].queue]   = trainId;              // Id del treno in coda
        semaphores[semId].trail_list[semaphores[semId].queue]   = SemRequiredState;     // Posizione del binario mobile richiesta dal treno
        semaphores[semId].queue                                 += 1;

        // Traslo i punti di ingresso e di stop del semaforo a seconda della direzione
        switch (direction) {

        case FROM_DX:
            semaphores[semId].xPointIn      += TRAIN_SPACE;
            semaphores[semId].xPointStop    += TRAIN_SPACE;
            break;
            
        case FROM_SX:
            semaphores[semId].xPointIn      -= TRAIN_SPACE;
            semaphores[semId].xPointStop    -= TRAIN_SPACE;
            break;

        default:
            break;
        }
        pthread_mutex_unlock(&semaphores[semId].mutex);                    
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE queue_manager();
//-------------------------------------------------------------------------------------------------------------------------

void queue_manager() {

    bool    move_queue;
    int     trail_queue;
    int     i;

    // Controlla per ogni stazione se è possibile muovere la coda e in caso la trasla
    for (i = 0; i < STATIONS_NUM; i++){

        pthread_mutex_lock(&station[i].mutex);
        move_queue = station[i].move_queue;
        pthread_mutex_unlock(&station[i].mutex);

        if (move_queue == true)     move_station_queue(i);
    }

    // Controlla per ogni binario mobile se ci sono dei treni in coda e in caso passa al successivo
    for (i = 0; i < SEMAPHORES_NUM; i++) {

        pthread_mutex_lock(&semaphores[i].mutex);
        trail_queue = semaphores[i].queue;
        pthread_mutex_unlock(&semaphores[i].mutex);

        if (trail_queue > 0)    move_semaphore_queue(i);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_semaphore_queue
//-------------------------------------------------------------------------------------------------------------------------
void move_semaphore_queue (int semId) {
    
    int i;
    int direction;
    int SemRequiredState;
    int trail_state;
    
    // In base all'Id del semaforo capisco la direzione del treno
    if (semId <= 2)         direction = FROM_SX;
    else                    direction = FROM_DX;

    // Salvo in locale le info sul binario mobile
    pthread_mutex_lock(&semaphores[semId].mutex);
    SemRequiredState    = semaphores[semId].trail_list[0];  // Posizione richiesta dal primo treno in coda
    trail_state         = semaphores[semId].trail_state;    // Posizione attuale del binario
    pthread_mutex_unlock(&semaphores[semId].mutex);

    // Se la posizione richiesta è diversa da quella attuale, il semaforo deve diventare rosso
    // e il binario deve muoversi nella posizione corretta
    if (SemRequiredState != trail_state)    move_trail_before_station(semId, SemRequiredState);
    
    // Quando il binario è nella posizione richiesta dal primo treno in coda, il treno viene fatto passare
    else {
        
        // Muove il primo treno della coda e aggiorna la struttra dati della coda stessa
        move_first_train_in_sem_queue(semId);

        // Aggiorno la struttra del semaforo dopo il movimento della coda
        pthread_mutex_lock(&semaphores[semId].mutex);
        semaphores[semId].move_queue   = false;         // Il semaforo non deve più muovere la coda fino a nuovo segnale
        semaphores[semId].queue        --;              // Decremento il numero di treni in coda

        // Shift di xPointIn e xPointStop dopo che la coda è diminuta di un treno
        switch (direction) {

            case FROM_SX:
                semaphores[semId].xPointIn     += TRAIN_SPACE;
                semaphores[semId].xPointStop   += TRAIN_SPACE;
                break;

            case FROM_DX:
                semaphores[semId].xPointIn     -= TRAIN_SPACE;
                semaphores[semId].xPointStop   -= TRAIN_SPACE;
                break;

            default:
                break;
        }
        pthread_mutex_unlock(&semaphores[semId].mutex);
    }

}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_station_queue
//-------------------------------------------------------------------------------------------------------------------------

void move_station_queue(int stationId){

    struct  timespec now;
    int     i;
    int     trainId;
    int     queue;
    int     direction;

    // In base all'Id della stazione capisco la direzione del treno
    if (stationId < 4)      direction = FROM_SX;
    else                    direction = FROM_DX;

    // Salvo in locale la lunghezza della coda presente in stazione
    pthread_mutex_lock(&station[stationId].mutex);
    queue = station[stationId].queue;
    pthread_mutex_unlock(&station[stationId].mutex);

    // Muove la coda
    for (i = 1; i <= queue; i++) {

        pthread_mutex_lock(&station[stationId].mutex);
        station[stationId].queue_list[i-1] = station[stationId].queue_list[i];
        trainId = station[stationId].queue_list[i-1];
        pthread_mutex_unlock(&station[stationId].mutex);

        // Esegue queste operazioni solo se c'è un altro treno in coda
        if (trainId != 0) {
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].ready_to_go_from_queue = true;
            pthread_mutex_unlock(&train_par[trainId].mutex);
        }
    }

    pthread_mutex_lock(&station[stationId].mutex);
    station[stationId].move_queue   =  false;
    station[stationId].queue        --;

    switch (direction) {

    case FROM_DX:
        station[stationId].xPointIn     -= TRAIN_SPACE;
        station[stationId].xPointStop   -= TRAIN_SPACE;
        break;

    case FROM_SX:
        station[stationId].xPointIn     += TRAIN_SPACE;
        station[stationId].xPointStop   += TRAIN_SPACE;
        break;

    default:
        break;
    }
    pthread_mutex_unlock(&station[stationId].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trail_before_station
//-------------------------------------------------------------------------------------------------------------------------

void move_trail_before_station (int semId, int SemRequiredState) {
    int direction;
    
    // In base all'Id del semaforo capisco la direzione del treno
    if (semId <= 2)         direction = FROM_SX;
    else                    direction = FROM_DX; 

    pthread_mutex_lock(&semaphores[semId].mutex);
    
    // Il semaforo diventa rosso
    semaphores[semId].status = false;

    // In base alla direzione del treno i binari si devono muovere in modo diverso
    switch(direction) {

        case FROM_DX:
            if (SemRequiredState == TRAIL_ON) {
                semaphores[semId].trail_angle -= TRAIL_ANGLE_INC;       // Rotazione antioraria
                if (semaphores[semId].trail_angle == TRAIL_DOWN_BIN_IN_SWITCH_ON) {
                    semaphores[semId].trail_state = TRAIL_ON;
                }
            }
            else if (SemRequiredState == TRAIL_OFF) {
                semaphores[semId].trail_angle += TRAIL_ANGLE_INC;       // Rotazione oraria
                if (semaphores[semId].trail_angle == TRAIL_DOWN_BIN_IN_SWITCH_OFF) {
                    semaphores[semId].trail_state = TRAIL_OFF;
                }
            }
            break;

        case FROM_SX:
            if      (SemRequiredState == TRAIL_ON) {
                semaphores[semId].trail_angle -= TRAIL_ANGLE_INC;       // Rotazione antioraria
                if (semaphores[semId].trail_angle == TRAIL_UP_BIN_IN_SWITCH_ON) {
                    semaphores[semId].trail_state = TRAIL_ON;
                }
            }
            else if (SemRequiredState == TRAIL_OFF) {
                semaphores[semId].trail_angle += TRAIL_ANGLE_INC;       // Rotazione oraria
                if (semaphores[semId].trail_angle == TRAIL_UP_BIN_IN_SWITCH_OFF) {
                    semaphores[semId].trail_state = TRAIL_OFF;
                }   
            }
            break;

        default:
            break;
    }
    pthread_mutex_unlock(&semaphores[semId].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_first_train_in_sem_queue
//-------------------------------------------------------------------------------------------------------------------------

void move_first_train_in_sem_queue (int semId) {
    
    int i;
    int trainId;
    int queue;

    pthread_mutex_lock(&semaphores[semId].mutex);
    semaphores[semId].status    = true;                                 // Sem può tornare verde         
    trainId                     = semaphores[semId].queue_list[0];      // Salvo l'Id del primo treno in coda
    queue                       = semaphores[semId].queue;              // Salvo il numero di treni in coda
    pthread_mutex_unlock(&semaphores[semId].mutex);

    pthread_mutex_lock(&train_par[trainId].mutex);
    train_par[trainId].sem_passed[semId]    = true;                     // Segnalo che il treno ha superato il semaforo
    train_par[trainId].queue                = false;                    // Segnalo che il treno non si trova più in coda
    pthread_mutex_unlock(&train_par[trainId].mutex);

    for (i = 1; i <= queue; i++) {
        
        // Traslo la coda dei treni e della lista di posizioni del binario richieste
        pthread_mutex_lock(&semaphores[semId].mutex);
        semaphores[semId].queue_list[i-1] = semaphores[semId].queue_list[i];
        semaphores[semId].trail_list[i-1] = semaphores[semId].trail_list[i];
        trainId = semaphores[semId].queue_list[i-1];
        pthread_mutex_unlock(&semaphores[semId].mutex);

        // trainId = 0 si ottiene quando quella posizione della coda è vuota
        if (trainId != 0) {

            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].ready_to_go_from_queue = true;       // Segnalo che tutti i treni della coda si possono muovere
            pthread_mutex_unlock(&train_par[trainId].mutex);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_station
//-------------------------------------------------------------------------------------------------------------------------

void check_station(int trainId) {

    bool    checked;
    bool    station_passed;
    bool    semaphore_flag;
    bool    train_in_station;
    bool    init_red_time;
    int     stationId;
    
    // Leggo i dati necessari dalla struttura dati del treno
    pthread_mutex_lock(&train_par[trainId].mutex);      
    stationId       = train_par[trainId].binary;                        // Id della stazione a cui è diretto
    checked         = train_par[trainId].checked;                       // Flag per controllare se il treno ha già trovato il prossimo stop
    station_passed  = train_par[trainId].station_passed[stationId];     // Flag per controllare se il treno ha già passato la stazione
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // Se non è ancora stato trovato il prossimo stop del treno, controllo se è nel range della stazione
    if (checked == false) {
        
        // Check se il treno si trova nel range della stazione
        train_in_station    = check_station_range(trainId);
        
        // Se il treno si trova in stazione e non l'ha ancora superata, deve fermarsi e il sem deve diventare rosso
        if (train_in_station == true && station_passed == false) {
            
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].checked  = true;                                 // Flag per evitare di controllare tutti i semafori successivi
            semaphore_flag              = train_par[trainId].semaphore_flag;    // Flag per indicare la prima volta che il treno entra in stazione
            pthread_mutex_unlock(&train_par[trainId].mutex);

            // Eseguito solo la prima volta che il treno entra nel range della stazione
            if (semaphore_flag == false){
                
                // Procedura di fermata alla stazione
                stop_at_station(trainId);
                
                // Check se il timer per il semaforo rosso è già stato inizializzato
                init_red_time = check_init_red_time(trainId);

                // Eventuale inizializzazzione del timer per il semaforo rosso
                if (init_red_time == false)     red_time_initialize(trainId);
            } 
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_station_range
//-------------------------------------------------------------------------------------------------------------------------

bool check_station_range (int trainId) {
    
    bool    train_in_station;
    int     posx;
    int     stationId;
    int     xPointIn;
    int     xPointOut;
    int     direction;

    pthread_mutex_lock(&train_par[trainId].mutex);
    stationId       = train_par[trainId].binary;            // Id della stazione            
    posx            = train_par[trainId].posx;              // Posizione attuale del treno
    direction       = train_par[trainId].direction;         // Direzione del treno
    pthread_mutex_unlock(&train_par[trainId].mutex);

    pthread_mutex_lock(&station[stationId].mutex);
    xPointIn    = station[stationId].xPointIn;              // Inizio range stazione
    xPointOut   = station[stationId].xPointOut;             // Fine range stazine
    pthread_mutex_unlock(&station[stationId].mutex);

    // Flag inizializzato al valore false
    train_in_station = false;
    
    // Controllo se il treno si trova nel range della stazione a seconda della sua direzione
    switch (direction) {

    case FROM_SX:

        if (posx > xPointIn && posx < xPointOut)    train_in_station = true;
        break;

    case FROM_DX:
        
        if (posx < xPointIn && posx > xPointOut)    train_in_station = true;
        break;
    default:
        break;
    }

    return train_in_station;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_init_red_time
//-------------------------------------------------------------------------------------------------------------------------

bool check_init_red_time (int trainId) {
    
    bool    init_red_time;
    int     direction;
    
    pthread_mutex_lock(&train_par[trainId].mutex);
    direction = train_par[trainId].direction;           // Direzione del treno
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // Ci sono due timer, uno per i treni da SX e uno per i treni da DX, controllo se quello 
    // della direzione del treno trainId è stato inizializzato
    switch (direction) {
    case FROM_SX:

        pthread_mutex_lock(&INIT_RED_TIME_SX_MUTEX);
        init_red_time = INIT_RED_TIME_SX;               // Variabile globale bool
        pthread_mutex_unlock(&INIT_RED_TIME_SX_MUTEX);
        break;

    case FROM_DX:
        
        pthread_mutex_lock(&INIT_RED_TIME_DX_MUTEX);
        init_red_time = INIT_RED_TIME_DX;               // Variabile globale bool
        pthread_mutex_unlock(&INIT_RED_TIME_DX_MUTEX);
        break;

    default:
        break;
    }
    return init_red_time;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE stop_at_station
//-------------------------------------------------------------------------------------------------------------------------

void stop_at_station(int trainId){
    int     stationId;
    int     direction;

    pthread_mutex_lock(&train_par[trainId].mutex);
    stationId       = train_par[trainId].binary;            // Id della stazione
    direction       = train_par[trainId].direction;         // Direzione del treno

    // Flag per comunicare che il treno è in stazione
    train_par[trainId].semaphore_flag   = true;

    // Eseguito la prima volta e ogni volta che la coda si muove
    if (train_par[trainId].queue == false) {
        
        pthread_mutex_lock(&station[stationId].mutex);
        train_par[trainId].pos_in_queue = station[stationId].queue;         // Salvo la posizione in coda del treno
        train_par[trainId].stop_x       = station[stationId].xPointStop;    // Salvo la posizione in cui mi dovrò fermare
        train_par[trainId].stop_id      = stationId;                        // Id della fermata
        train_par[trainId].stop_type    = STATION;                          // Tipologia di fermata

        station[stationId].queue_list[station[stationId].queue] = trainId;  // Inserisco il treno in coda
        station[stationId].queue ++;                                        // Incremento il numero di treni in coda
        
        // Shift del range della stazione in modo coerente con la direzione del treno
        switch (direction) {

        case FROM_DX:
            station[stationId].xPointIn     += TRAIN_SPACE;
            station[stationId].xPointStop   += TRAIN_SPACE;
            break;

        case FROM_SX:
            station[stationId].xPointIn     -= TRAIN_SPACE;
            station[stationId].xPointStop   -= TRAIN_SPACE;
            break;

        default:
            break;
        }
        pthread_mutex_unlock(&station[stationId].mutex);
    }
    pthread_mutex_unlock(&train_par[trainId].mutex);               

    pthread_mutex_lock(&station[stationId].mutex);
    station[stationId].status = false;                      // Il semaforo della stazione diventa rosso
    pthread_mutex_unlock(&station[stationId].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE red_time_initialize
//-------------------------------------------------------------------------------------------------------------------------

void red_time_initialize (int trainId) {

    struct  timespec now;
    struct  timespec leave_time;
    int     direction;

    pthread_mutex_lock(&train_par[trainId].mutex);
    direction       = train_par[trainId].direction;         // Direzione del treno
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // Salvo l'istante corrente
    clock_gettime(CLOCK_MONOTONIC, &now);         

    switch (direction) {

        case FROM_SX:

            pthread_mutex_lock(&INIT_RED_TIME_SX_MUTEX);
            INIT_RED_TIME_SX = true;                        // Indico che il timer è stato inizializzato
            pthread_mutex_unlock(&INIT_RED_TIME_SX_MUTEX);

            pthread_mutex_lock(&last_red_time_sx_mutex);
            time_copy(&last_red_time_sx, now);              // Copio nella variabile globale l'istante in cui il semaforo è diventato rosso
            pthread_mutex_unlock(&last_red_time_sx_mutex);
            break;
        
        case FROM_DX:

            pthread_mutex_lock(&INIT_RED_TIME_DX_MUTEX);
            INIT_RED_TIME_DX = true;                        // Indico che il timer è stato inizializzato
            pthread_mutex_unlock(&INIT_RED_TIME_DX_MUTEX);

            pthread_mutex_lock(&last_red_time_dx_mutex);
            time_copy(&last_red_time_dx, now);              // Copio nella variabile globale l'istante in cui il semaforo è diventato rosso
            pthread_mutex_unlock(&last_red_time_dx_mutex);
            break;

        default:
            break;    
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE manage_station_out_SX
//-------------------------------------------------------------------------------------------------------------------------

void manage_station_out_SX() {
    
    bool    ready_to_go;
    bool    move_trails;
    int     trainId;
    int     binary;
    int     posx;
    int     direction;

    // Definisco la direzione dei treni da controllare
    direction = FROM_SX;

    // Check se è il momento di far diventare un semaforo verde
    ready_to_go = check_green_time(direction);

    if (ready_to_go == true) {

        // Id del treno a prio massima tra quelli in attesa
        trainId = search_for_max_prio_train(direction);

        // Se trovo un treno valido, segnalo di muovere i binari
        if (trainId != 0)   send_command_to_move_trails_out(direction, trainId);

        // Se ci sono altri semafori rossi a questo punto, aggiorno l'istante in cui il prossimo sem deve diventare verde
        check_other_station_waiting(direction);
    }
    
    // Leggo il comando di muovere i binari
    move_trails = read_command_to_move_trails(direction);

    if (move_trails == true) {

        // Import delle variabili globali
        pthread_mutex_lock(&max_prio_train_sx_mutex);
        trainId = max_prio_train_sx;
        pthread_mutex_unlock(&max_prio_train_sx_mutex);

        pthread_mutex_lock(&train_par[trainId].mutex);
        binary  = train_par[trainId].binary;
        posx    = train_par[trainId].posx;
        pthread_mutex_unlock(&train_par[trainId].mutex);

        // Spostamento dei binari in modo coerente col binario in cui si trova il treno
        switch (binary) {
            case 0:
                move_trails_out_bin_0(posx);
                break;
            
            case 1:
                move_trails_out_bin_1(posx);
                break;

            case 2:
                move_trails_out_bin_2(posx);
                break;

            case 3:
                move_trails_out_bin_3(posx);
                break;

            default:
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE manage_station_out_DX
//-------------------------------------------------------------------------------------------------------------------------
void manage_station_out_DX(){

    bool    ready_to_go;
    bool    move_trails;
    int     trainId;
    int     binary;
    int     posx;
    int     direction;

    // Definisco la direzione dei treni da controllare
    direction = FROM_DX;
    
    // Check se è il momento di far diventare un semaforo verde
    ready_to_go = check_green_time(direction);

    if (ready_to_go == true) {

        // Id del treno a prio massima tra quelli in attesa
        trainId = search_for_max_prio_train(direction);

        // Se trovo un treno valido, segnalo di muovere i binari
        if (trainId != 0)       send_command_to_move_trails_out(direction, trainId);

        // Se ci sono altri semafori rossi a questo punto, aggiorno l'istante in cui il prossimo sem deve diventare verde
        check_other_station_waiting(direction);
    }

    // Leggo il comando di muovere i binari
    move_trails = read_command_to_move_trails(direction);

    if (move_trails == true) {

        // Import delle variabili globali
        pthread_mutex_lock(&max_prio_train_dx_mutex);
        trainId = max_prio_train_dx;
        pthread_mutex_unlock(&max_prio_train_dx_mutex);

        pthread_mutex_lock(&train_par[trainId].mutex);
        binary  = train_par[trainId].binary;                    // Binario del treno che deve lasciare la stazione
        posx    = train_par[trainId].posx;                      // Posizione lungo X del treno che deve lasciare la stazione
        pthread_mutex_unlock(&train_par[trainId].mutex);

        // Spostamento dei binari in modo coerente col binario in cui si trova il treno
        switch (binary) {
                
            case 4:
                move_trails_out_bin_4(posx);
                break;

            case 5:
                move_trails_out_bin_5(posx);
                break;

            case 6:
                move_trails_out_bin_6(posx);
                break;

            case 7:
                move_trails_out_bin_7(posx);
                break;
            
            default:
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_green_time
//-------------------------------------------------------------------------------------------------------------------------

bool check_green_time (int direction) {

    bool    init_red_time;
    bool    ready_to_go;
    struct  timespec    now;
    struct  timespec    leave_time;

    ready_to_go = false;

    // Leggo l'istante attuale
    clock_gettime(CLOCK_MONOTONIC, &now);

    switch (direction) {
    
    case FROM_SX:

        // Check per vedere se il timer SX è stato inizializzato
        pthread_mutex_lock(&INIT_RED_TIME_SX_MUTEX);
        init_red_time = INIT_RED_TIME_SX;
        pthread_mutex_unlock(&INIT_RED_TIME_SX_MUTEX);

        if (init_red_time == true) {
            
            // Copio in locale l'istante in cui il primo semaforo SX è diventato rosso
            pthread_mutex_lock(&last_red_time_sx_mutex);
            time_copy(&leave_time, last_red_time_sx);
            pthread_mutex_unlock(&last_red_time_sx_mutex);

            // Ottengo l'istante in cui il sem deve diventare verde aggiungendo STOP_TIME all'istante copiato precedentemente
            time_add_ms(&leave_time, STOP_TIME);

            // Se "now" è maggiore di "leave_time" e la variabile "READY_TO_GO_SX" è settata al valore "true" segnalo che il treno è pronto a partire
            // READY_TO_GO_SX serve a evitare che il semaforo diventi verde prima che il treno precedente abbia lasciato la stazione
            pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);
            if (time_cmp(now, leave_time) > 0 && READY_TO_GO_SX == true)        ready_to_go = true;
            pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);
        }
        break;
    
    case FROM_DX:

        // Check per vedere se il timer DX è stato inizializzato
        pthread_mutex_lock(&INIT_RED_TIME_DX_MUTEX);
        init_red_time = INIT_RED_TIME_DX;
        pthread_mutex_unlock(&INIT_RED_TIME_DX_MUTEX);

        if (init_red_time == true) {

            // Copio in locale l'istante in cui il primo semaforo DX è diventato rosso
            pthread_mutex_lock(&last_red_time_dx_mutex);
            time_copy(&leave_time, last_red_time_dx);
            pthread_mutex_unlock(&last_red_time_dx_mutex);

            // Ottengo l'istante in cui il sem deve diventare verde aggiungendo STOP_TIME all'istante copiato precedentemente
            time_add_ms(&leave_time, STOP_TIME);

            // Se "now" è maggiore di "leave_time"  segnalo che il treno è pronto a partire
            pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);
            if (time_cmp(now, leave_time) > 0 && READY_TO_GO_DX == true)        ready_to_go = true;
            pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);
        }
        break;
    default:
        break;
    }
    return      ready_to_go;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE search_for_max_prio_train
//-------------------------------------------------------------------------------------------------------------------------
int search_for_max_prio_train (int direction) {

    int     stationId;
    int     train_id;
    int     priority;
    int     binary;
    int     max_priority_train_id;
    int     max_priority;
    int     max_priority_train_bin;
    int     first_station;

    // Init delle variabili
    max_priority_train_id   = 0;
    max_priority            = 0;

    if (direction == FROM_SX) {
        first_station           = 0;            // Se la direzione è SX controllo le stazioni 0, 1, 2, 3
        max_priority_train_bin  = 0;            // In caso di treni a priorità uguale, scelgo quello con binario maggiore quindi devo inizializzare a 0 questa variabile (valore minimo)
    }

    else {                        
        first_station           = 4;            // Se la direzione è DX controllo le stazioni 4, 5, 6, 7
        max_priority_train_bin  = 7;            // In caso di treni a priorità uguale, scelgo quello con binario maggiore quindi devo inizializzare a 7 questa variabile (valore massimo)
    }

    // Check delle 4 stazioni
    for (stationId = first_station; stationId <= first_station + 3; stationId ++) {
        
        pthread_mutex_lock(&station[stationId].mutex);
        train_id = station[stationId].queue_list[0];        // Id del primo treno in coda a ciascuna stazione
        pthread_mutex_unlock(&station[stationId].mutex);

        pthread_mutex_lock(&train_par[train_id].mutex);
        priority    = train_par[train_id].priority;         // Priorità di trainId
        binary      = train_par[train_id].binary;           // Binario di trainID
        pthread_mutex_unlock(&train_par[train_id].mutex);

        if (priority > max_priority ||                                                                      
            priority == max_priority && binary > max_priority_train_bin && direction == FROM_SX ||      // FROM_SX: In caso di priorità uguali, predilige quello con binario maggiore
            priority == max_priority && binary < max_priority_train_bin && direction == FROM_DX) {      // FROM_DX: In caso di priorità uguali, predilige quello con binario minore

            max_priority            = priority;
            max_priority_train_id   = train_id;
            max_priority_train_bin  = binary;
        }
    }
    return      max_priority_train_id;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_other_station_waiting
//-------------------------------------------------------------------------------------------------------------------------

void check_other_station_waiting(int direction){

    bool    another_station_waiting;
    bool    station_status;
    int     stationId;
    int     first_station;

    if (direction == FROM_SX)       first_station = 0;                  // Se la direzione è SX controllo le stazioni 0, 1, 2, 3
    else                            first_station = 4;                  // Se la direzione è DX controllo le stazioni 4, 5, 6, 7

    // Check delle 4 stazioni
    for (stationId = first_station; stationId <= first_station + 3; stationId ++) {

        pthread_mutex_lock(&station[stationId].mutex);
        station_status = station[stationId].status;         // Status della stazione 
        pthread_mutex_unlock(&station[stationId].mutex);
        
        // Se trovo almeno un altro semaforo rosso, esco dal ciclo e lo segnalo 
        if (station_status == false) {
            another_station_waiting = true;
            break;
        }
    }

    switch (direction) {

    case FROM_SX:
        
        // Se un'altra stazione è in attesa, segnalo che il prossimo sem SX deve diventare verde dopo STOP_TIME
        if (another_station_waiting == true) {
            pthread_mutex_lock(&last_red_time_sx_mutex);
            time_add_ms(&last_red_time_sx, STOP_TIME);
            pthread_mutex_unlock(&last_red_time_sx_mutex);
        }

        // Se nessuna stazione è in attesa, resetto il timer SX
        else {
            pthread_mutex_lock(&INIT_RED_TIME_SX_MUTEX);
            INIT_RED_TIME_SX = false;
            pthread_mutex_unlock(&INIT_RED_TIME_SX_MUTEX);
        }
        break;

    case FROM_DX:

        // Se un'altra stazione è in attesa, segnalo che il prossimo sem DX deve diventare verde dopo STOP_TIME
        if (another_station_waiting == true) {
            pthread_mutex_lock(&last_red_time_dx_mutex);
            time_add_ms(&last_red_time_dx, STOP_TIME);
            pthread_mutex_unlock(&last_red_time_dx_mutex);
        }

        // Se nessuna stazione è in attesa, resetto il timer DX
        else {
            pthread_mutex_lock(&INIT_RED_TIME_DX_MUTEX);
            INIT_RED_TIME_DX = false;
            pthread_mutex_unlock(&INIT_RED_TIME_DX_MUTEX);
        }
        break;

    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE read_command_to_move_trails
//-------------------------------------------------------------------------------------------------------------------------

bool read_command_to_move_trails (int direction){

    bool    move_trails;

    // A seconda della direzione, leggo il valore globale che sengala se devo muovere i binari
    switch (direction) {

    case FROM_SX:
        pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
        move_trails = MOVE_TRAILS_SX;
        pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);
        break;
    
    case FROM_DX:
        pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
        move_trails = MOVE_TRAILS_DX;
        pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);
        break;
    default:
        break;
    }
    return      move_trails;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE send_command_to_move_trails_out
//-------------------------------------------------------------------------------------------------------------------------

void send_command_to_move_trails_out (int direction, int trainId) {

    int stationId;
    
    pthread_mutex_lock(&train_par[trainId].mutex);
    stationId = train_par[trainId].binary;                      // Stazione a cui si trova il treno trainId
    pthread_mutex_unlock(&train_par[trainId].mutex);

    pthread_mutex_lock(&station[stationId].mutex);
    station[stationId].status = true;                           // Semaforo della stazione diventa verde
    pthread_mutex_unlock(&station[stationId].mutex);

    pthread_mutex_lock(&train_par[trainId].mutex);
    train_par[trainId].station_passed[stationId] = true;        // Segnalo che il treno ha superato la stazione
    pthread_mutex_unlock(&train_par[trainId].mutex);

    switch (direction) {

    case FROM_SX:    

        // Segnalo che i binari mobili in uscita dalla stazoni dei treni che vengono da SX si devono muovere
        pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
        MOVE_TRAILS_SX = true;
        pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);

        // Finchè i binari non hanno finito di muoversi, i treni da SX non possono muoversi
        pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);            
        READY_TO_GO_SX = false;
        pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);

        // Salvo in globale il valore del treno a priorità massima lato SX
        pthread_mutex_lock(&max_prio_train_sx_mutex);
        max_prio_train_sx = trainId;
        pthread_mutex_unlock(&max_prio_train_sx_mutex);
        break;

    case FROM_DX:

        // Segnalo che i binari mobili in uscita dalla stazoni dei treni che vengono da DX si devono muovere
        pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
        MOVE_TRAILS_DX = true;
        pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);

        // Finchè i binari non hanno finito di muoversi, i treni da DX non possono muoversi
        pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);            
        READY_TO_GO_DX = false;
        pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);

        // Salvo in globale il valore del treno a priorità massima lato DX
        pthread_mutex_lock(&max_prio_train_dx_mutex);
        max_prio_train_dx = trainId;
        pthread_mutex_unlock(&max_prio_train_dx_mutex);
        break;

    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin0
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_0(int posx){

    pthread_mutex_lock(&semaphores[5].mutex);
    semaphores[5].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[5].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {           // Se supero il valore massimo, reset a quel valore
        semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
    }

    // Quando il binario ha finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato SX
    if (semaphores[5].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_ON && 
        posx >= W - 1.5*SPACE) {

        pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
        MOVE_TRAILS_SX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);

        pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);
        READY_TO_GO_SX = true;
        pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);
    }
    pthread_mutex_unlock(&semaphores[5].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin1
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_1(int posx){

    pthread_mutex_lock(&semaphores[4].mutex);
    pthread_mutex_lock(&semaphores[5].mutex);

    semaphores[4].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[4].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {           // Se supero il valore massimo, reset a quel valore
        semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
    } 

    semaphores[5].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {          // Se supero il valore massimo, reset a quel valore
        semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
    }

    // Quando i binari hanno finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato SX
    if (semaphores[4].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_ON && 
        semaphores[5].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF &&
        posx >= W - 1.5*SPACE) {            
        
        pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
        MOVE_TRAILS_SX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);

        pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);
        READY_TO_GO_SX = true;
        pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);
    }

    pthread_mutex_unlock(&semaphores[5].mutex);
    pthread_mutex_unlock(&semaphores[4].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin2
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_2(int posx){

    pthread_mutex_lock(&semaphores[3].mutex);
    pthread_mutex_lock(&semaphores[4].mutex);
    pthread_mutex_lock(&semaphores[5].mutex);

    semaphores[3].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[3].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {           // Se supero il valore massimo, reset a quel valore
        semaphores[3].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
    }

    semaphores[4].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[4].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {          // Se supero il valore massimo, reset a quel valore
        semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
    }

    semaphores[5].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {          // Se supero il valore massimo, reset a quel valore
        semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
    }

    // Quando i binari hanno finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato SX
    if (semaphores[3].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_ON && 
        semaphores[4].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF &&
        semaphores[5].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF &&
        posx >= W - 1.5*SPACE) {
            
        pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
        MOVE_TRAILS_SX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);
        
        pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);
        READY_TO_GO_SX = true;
        pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);
    }
    pthread_mutex_unlock(&semaphores[3].mutex);
    pthread_mutex_unlock(&semaphores[4].mutex);
    pthread_mutex_unlock(&semaphores[5].mutex);  
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin3
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_3(int posx){

    pthread_mutex_lock(&semaphores[3].mutex);
    pthread_mutex_lock(&semaphores[4].mutex);
    pthread_mutex_lock(&semaphores[5].mutex);

    semaphores[3].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[3].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {          // Se supero il valore massimo, reset a quel valore
        semaphores[3].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
    }

    semaphores[4].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[4].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {          // Se supero il valore massimo, reset a quel valore
        semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
    } 

    semaphores[5].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {          // Se supero il valore massimo, reset a quel valore
        semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
    }

    // Quando i binari hanno finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato SX
    if (semaphores[3].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF && 
        semaphores[4].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF &&
        semaphores[5].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF &&
        posx >= W - 1.5*SPACE) {
            
            pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
            MOVE_TRAILS_SX = false;
            pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);

            pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);
            READY_TO_GO_SX = true;
            pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);
        }
        pthread_mutex_unlock(&semaphores[3].mutex);
        pthread_mutex_unlock(&semaphores[4].mutex);
        pthread_mutex_unlock(&semaphores[5].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin4
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_4(int posx){

    pthread_mutex_lock(&semaphores[6].mutex);
    pthread_mutex_lock(&semaphores[7].mutex);
    pthread_mutex_lock(&semaphores[8].mutex);

    semaphores[6].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[6].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {        // Se supero il valore massimo, reset a quel valore
        semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
    }

    semaphores[7].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[7].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {        // Se supero il valore massimo, reset a quel valore
        semaphores[7].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
    } 

    semaphores[8].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[8].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {        // Se supero il valore massimo, reset a quel valore
        semaphores[8].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
    }

    // Quando i binari hanno finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato DX
    if (semaphores[6].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_OFF && 
        semaphores[7].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_OFF &&
        semaphores[8].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_OFF &&
        posx <= 1.5*SPACE) {
        
        pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
        MOVE_TRAILS_DX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);

        pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);
        READY_TO_GO_DX = true;
        pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);
    }
    pthread_mutex_unlock(&semaphores[6].mutex);
    pthread_mutex_unlock(&semaphores[7].mutex);
    pthread_mutex_unlock(&semaphores[8].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin5
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_5(int posx){

    pthread_mutex_lock(&semaphores[6].mutex);
    pthread_mutex_lock(&semaphores[7].mutex);
    pthread_mutex_lock(&semaphores[8].mutex);

    semaphores[6].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[6].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {        // Se supero il valore massimo, reset a quel valore
        semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
    }
    semaphores[7].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[7].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {        // Se supero il valore massimo, reset a quel valore
        semaphores[7].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
    }
    semaphores[8].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[8].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_ON) {         // Se supero il valore massimo, reset a quel valore
        semaphores[8].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_ON;
    }

    // Quando i binari hanno finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato DX
    if (semaphores[6].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_OFF && 
        semaphores[7].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_OFF &&
        semaphores[8].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_ON &&
        posx < 1.5*SPACE) {
            
        pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
        MOVE_TRAILS_DX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);
        
        pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);
        READY_TO_GO_DX = true;
        pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);
    }
    pthread_mutex_unlock(&semaphores[6].mutex);
    pthread_mutex_unlock(&semaphores[7].mutex);
    pthread_mutex_unlock(&semaphores[8].mutex); 
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin6
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_6(int posx){

    pthread_mutex_lock(&semaphores[7].mutex);
    pthread_mutex_lock(&semaphores[6].mutex);

    semaphores[7].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[7].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_ON) {         // Se supero il valore massimo, reset a quel valore
        semaphores[7].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_ON;
    } 

    semaphores[6].trail_angle -= TRAIL_ANGLE_INC;                           // Rotazione anti-oraria
    if (semaphores[6].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {        // Se supero il valore massimo, reset a quel valore
        semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
    }

    // Quando i binari hanno finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato DX
    if (semaphores[7].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_ON && 
        semaphores[6].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_OFF &&
        posx < 1.5*SPACE) {
        
        pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
        MOVE_TRAILS_DX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);

        pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);
        READY_TO_GO_DX = true;
        pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);

    }

    pthread_mutex_unlock(&semaphores[7].mutex);
    pthread_mutex_unlock(&semaphores[6].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_trails_out_bin7
//-------------------------------------------------------------------------------------------------------------------------

void move_trails_out_bin_7(int posx){

    pthread_mutex_lock(&semaphores[6].mutex);

    semaphores[6].trail_angle += TRAIL_ANGLE_INC;                           // Rotazione oraria
    if (semaphores[6].trail_angle >= TRAIL_DOWN_BIN_OUT_SWITCH_ON) {        // Se supero il valore massimo, reset a quel valore
        semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_ON;
    }

    // Quando il binario ha finito di muoversi E il treno ha superato tutti i binari mobili, segnalo che posso gestire il prossimo treno lato DX
    if (semaphores[6].trail_angle == TRAIL_DOWN_BIN_OUT_SWITCH_ON && 
        posx < 1.5*SPACE) {

        pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
        MOVE_TRAILS_DX = false;
        pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);

        pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);
        READY_TO_GO_DX = true;
        pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);
    }
    pthread_mutex_unlock(&semaphores[6].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// TASK station_manager
//-------------------------------------------------------------------------------------------------------------------------

void *station_manager(void *p){
    int     id;

    id = get_task_id(p);
    set_activation(id);

    while (EXIT == false){
        
        // ASSEGNAZIONE BINARI
        binary_assignment();

        // RICERCA DELLA PROSSIMA FERMATA PER OGNI TRENO CHE SI TROVA PRIMA DELLE STAZIONI
        find_next_stop();

        //GESTIONE CODE DEI SEMAFORI E DELLE STAZIONI
        queue_manager();
        
        // FINZIONI CHE RICERCANO IL PRIMO TRENO DA FAR PARTIRE TRA QUELLI FERMI IN STAZIONE LATO SX E DX
        manage_station_out_SX();
        manage_station_out_DX();

        // CHECK DL MISS DEL TASK STATION MANAGER
        if(deadline_miss(id))           printf("Deadline miss of station manager task \n");

        // ATTESA PROSSIMA ATTIVAZIONE
        wait_for_activation(id);
    }
}