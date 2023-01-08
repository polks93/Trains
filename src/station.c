#include <allegro.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>

#include "functions.h"
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

        // Check se il timer per il semaforo rosso è già stato inizializzato
        init_red_time       = check_init_red_time(trainId);
        
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
        
        // FINZIONI CHE RICERCANO IL PRIMO TRENO DA FAR PARTIRE TRA QUELLI FERMI IN STAZIONE
        stationOutSx();
        stationOutDx();

        // CHECK DL MISS DEL TASK STATION MANAGER
        if(deadline_miss(id))           printf("Deadline miss of station manager task \n");

        // ATTESA PROSSIMA ATTIVAZIONE
        wait_for_activation(id);
    }
}