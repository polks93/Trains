#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "init.h"
#include "ptask.h"
#include "graphics.h"
#include "trains.h"

//----------------------------------------------------------------------------
// FUNZIONE set_train_parameters
//---------------------------------------------------------------------------
void set_train_parameters(int i) {
    
    int                 random_num;
    int                 j;
    struct timespec     now;

    pthread_mutex_t     train_mux = PTHREAD_MUTEX_INITIALIZER;

    // Incremento il numero di treni in attesa del binario (GLOBALE)
    pthread_mutex_lock(&ready_trains_num_mutex);
    ready_trains_num ++;
    pthread_mutex_unlock(&ready_trains_num_mutex);

    // Seme per la funzione rand()
    clock_gettime(CLOCK_MONOTONIC, &now);
    srand(now.tv_nsec);             
    random_num = rand();

    // Inizializzo la struttura del treno
    train_par[i].run                        = false;     
    train_par[i].binary_assigned            = false; 
    train_par[i].ready_to_go_from_queue     = false; 
    train_par[i].semaphore_flag             = false; 
    train_par[i].checked                    = false;
    train_par[i].queue                      = false;
    train_par[i].binary_occupied            = false;

    // Init della struttura per indicare se il treno è già passato da una stazione
    for (j = 0; j < STATIONS_NUM; j++)      train_par[i].station_passed[j] = false;
    // Init della struttura per indicare se il treno è già passato da un semaforo
    for (j = 0; j < SEMAPHORES_NUM; j++)    train_par[i].sem_passed[j] = false;

    // Assegnamento direzione casuale, o definita da utente
    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
    pthread_mutex_lock(&user_direction_mutex);
    if (ASSIGNED_DIRECTION == true)     train_par[i].direction = user_direction;
    else                                train_par[i].direction = (random_num)%2; // direzione random da 0 a 1
    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);    
    pthread_mutex_unlock(&user_direction_mutex);
    
    // Se non viene indicata una priorità, ne scelgo una random da 1 a 3
    if (train_par[i].priority == 0)             train_par[i].priority = 1 + (random_num)%3;

    // Init della posizione del treno a seconda della direzione
    if (train_par[i].direction == FROM_DX)      train_par[i].posx = - TRAIN_W;
    else                                        train_par[i].posx = W;
    
    train_par[i].stop_x                 = W*(1 - train_par[i].direction);      // Prossimo stop inizializzato a fine corsa
    train_par[i].stop_type              = SEMAPHORE;
    train_par[i].stop_id                = 11*train_par[i].direction;
    train_par[i].pos_in_queue           = 0;
    train_par[i].first_diagonal_wagon   = 0;
    train_par[i].currentVel             = MAX_VEL;
    train_par[i].mutex                  = train_mux;
    train_par[i].bmp[0]                 = train_bmp[train_par[i].priority - 1].train1;   // Import dei bitmap
    train_par[i].bmp[1]                 = train_bmp[train_par[i].priority - 1].train2;
    train_par[i].bmp[2]                 = train_bmp[train_par[i].priority - 1].train3;

    // Init dei parametri dei vari vagoni
    for (j = 0; j < WAGONS; j++){

        train_par[i].wagons[j].bmp              = train_par[i].bmp[0];
        train_par[i].wagons[j].diag_passed      = false;

        if (train_par[i].direction == 0){
            train_par[i].wagons[j].posx = - (j*(TRAIN_W + WAGONS_SPACE));
            train_par[i].wagons[j].posy = H/2 - SPACE/2 - TRAIN_H/2;
        }
        else {
            train_par[i].wagons[j].posx = W + (j*(TRAIN_W + WAGONS_SPACE));
            train_par[i].wagons[j].posy = H/2 + SPACE/2 - TRAIN_H/2;    
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE wait_for_binary_assignement
//-------------------------------------------------------------------------------------------------------------------------

void wait_for_binary_assignement(int trainId) {

    bool binary_assigned;

    binary_assigned = false;

    // Controllo se il binario è stato assegnato e blocco il task in questo ciclo
    while(binary_assigned == false && EXIT == false){

        pthread_mutex_lock(&train_par[trainId].mutex);
        binary_assigned = train_par[trainId].binary_assigned;
        pthread_mutex_unlock(&train_par[trainId].mutex);

        // DL miss check
        if (deadline_miss(trainId)) {
            printf("Deadline miss of train task %d \n", trainId);
            total_train_dl ++;                                          // Incremento variabile globale che conta tutti i DL miss dei task treno
        }

        // Attesa prossima attivazione
        wait_for_activation(trainId);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE collision_check
//-------------------------------------------------------------------------------------------------------------------------

void collision_check (int trainId) {
    int     ready_flag;
    int     direction;
    struct  timespec    now;
    struct  timespec    ready_time;
    
    // Ottengo la direzione dalla struttura dati del treno
    pthread_mutex_lock(&train_par[trainId].mutex);
    direction   = train_par[trainId].direction;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    ready_flag = 0;                                                     // Flag che vale 1 se è il momento di partire, altrimenti vale 0 o -1

    while (ready_flag < 1 && EXIT == false) {

        // Leggo l'istante corrente
        clock_gettime(CLOCK_MONOTONIC, &now);
        
        switch(direction) {

            case FROM_DX:

                pthread_mutex_lock(&last_assigned_train_from_dx_mutex);
                time_copy(&ready_time, last_assigned_train_from_dx);                    // Copio in ready_time l'istante in cui è stato generato il treno precedente
                time_add_ms(&ready_time, MIN_MS_BETWEEN_TRAINS);                        // Aggiungo a ready_time il delta T minimo tra un treno e il successivo    
                ready_flag = time_cmp(now, ready_time);                                 // Confronto tra l'istante attuale e l'istante in cui il treno può partire
                if (ready_flag == 1)    time_copy(&last_assigned_train_from_dx, now);   // Se il treno può partire, aggiorno la variabile globale
                pthread_mutex_unlock(&last_assigned_train_from_dx_mutex);
                break;
            
            case FROM_SX:           // Stessa procedura per la direzione opposta

                pthread_mutex_lock(&last_assigned_train_from_sx_mutex); 
                time_copy(&ready_time, last_assigned_train_from_sx);
                time_add_ms(&ready_time, MIN_MS_BETWEEN_TRAINS);
                ready_flag = time_cmp(now, ready_time);
                if (ready_flag == 1)    time_copy(&last_assigned_train_from_sx, now);
                pthread_mutex_unlock(&last_assigned_train_from_sx_mutex);
                break;
            
            default:
                break;
        }

        // DL miss check
        if (deadline_miss(trainId)) {
            printf("Deadline miss of train task %d \n", trainId);
            total_train_dl ++;                                          // Incremento variabile globale che conta tutti i DL miss dei task treno
        }

        // Attesa prossima attivazione
        wait_for_activation(trainId);
    }
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE state_machine
//-------------------------------------------------------------------------------------------------------------------------

int state_machine (int trainId, int direction, int curr_state) {
    int     next_state;

    switch (curr_state){

        case(GO_FAST):
            // MOVIMENTO A MASSIMA VELOCITA'
            next_state = state_go_fast(trainId);
            break;

        case(SLOW_DOWN):
            // RALLENTO PRIMA DI UN SEMAFORO
            next_state = state_slow_down(trainId, direction);
            break;

        case(QUEUE):  
            // MOVIMENTO DELLA CODA DEL SEMAFORO
            next_state = state_queue(trainId, direction);
            break;

        case(STOP):  

            next_state = state_stop(trainId);
            break;

        case(SPEED_UP):
            // USCITA DAL SEMAFORO
            next_state = state_speed_up(trainId);
            break;

        default:
            break;
    }
    return      next_state;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE state_go_fast
//-------------------------------------------------------------------------------------------------------------------------

int state_go_fast(int trainId) {

    bool    semaphore_flag;
    int     next_state;
    int     acc;
    float   vel;

    pthread_mutex_lock(&train_par[trainId].mutex);
    semaphore_flag = train_par[trainId].semaphore_flag;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // MOVIMENTO NORMALE DEL TRENO
    acc = 0;
    vel = MAX_VEL;
    move(trainId, vel, acc);

    // Check fermata
    if (semaphore_flag == true)     next_state  = SLOW_DOWN;
    else                            next_state  = GO_FAST;

    return      next_state;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE state_slow_down
//-------------------------------------------------------------------------------------------------------------------------

int state_slow_down (int trainId, int direction) {

    bool    first_of_queue;
    int     next_state;
    int     posx;
    int     stop_x;
    int     acc;
    float   vel;

    next_state = SLOW_DOWN;
    acc = - MAX_ACC;

    pthread_mutex_lock(&train_par[trainId].mutex);
    vel         = train_par[trainId].currentVel;
    posx        = train_par[trainId].posx;
    stop_x      = train_par[trainId].stop_x;
    if (train_par[trainId].pos_in_queue == 0)       first_of_queue = true;
    else                                            first_of_queue = false;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    move(trainId, vel, acc);

    // Check fermata
    switch (direction) {

        case FROM_DX:
            if (posx <= stop_x) {
                if (first_of_queue == true)      next_state = STOP;
                else                             next_state = QUEUE;
            }
            break;

        case FROM_SX:
            if (posx >= stop_x ) {
                if (first_of_queue == true)     next_state = STOP; 
                else                            next_state = QUEUE;
            }
            break;

        default:
            break;
    }   
    return      next_state;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE state_queue
//-------------------------------------------------------------------------------------------------------------------------

int state_queue(int trainId, int direction){
    int     next_state;

    next_state = QUEUE;

    pthread_mutex_lock(&train_par[trainId].mutex);
    if (train_par[trainId].ready_to_go_from_queue == true) {

        train_par[trainId].ready_to_go_from_queue   = false;
        train_par[trainId].queue                    = true;
        train_par[trainId].currentVel               = MAX_VEL;
        train_par[trainId].semaphore_flag           = false;
        train_par[trainId].pos_in_queue --;

        switch (direction){

            case FROM_DX:
                train_par[trainId].stop_x -= TRAIN_SPACE;
                break;

            case FROM_SX:
                train_par[trainId].stop_x += TRAIN_SPACE;
                break;

            default:
                break;
        }
        next_state = SLOW_DOWN;
    }
    pthread_mutex_unlock(&train_par[trainId].mutex);

    return      next_state;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE state_stop
//-------------------------------------------------------------------------------------------------------------------------

int state_stop (int trainId) {   

    bool    stop_status;
    int     next_state;
    int     stop_id;
    int     stop_type;

    next_state = STOP;

    pthread_mutex_lock(&train_par[trainId].mutex);
    stop_id     = train_par[trainId].stop_id;
    stop_type   = train_par[trainId].stop_type;
    pthread_mutex_unlock(&train_par[trainId].mutex);      

    if (stop_type == STATION) {

        pthread_mutex_lock(&station[stop_id].mutex);
        stop_status = station[stop_id].status;
        pthread_mutex_unlock(&station[stop_id].mutex);

        if (stop_status == true) {
            next_state = SPEED_UP;
            
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].queue = false;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            pthread_mutex_lock(&station[stop_id].mutex);
            station[stop_id].move_queue = true;
            pthread_mutex_unlock(&station[stop_id].mutex);
        }
    }

    else if (stop_type == SEMAPHORE) {

        pthread_mutex_lock(&semaphores[stop_id].mutex);
        stop_status = semaphores[stop_id].status;
        pthread_mutex_unlock(&semaphores[stop_id].mutex); 
        
        if (stop_status == true) {

            next_state = SPEED_UP;

            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].queue = false;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            pthread_mutex_lock(&semaphores[stop_id].mutex);
            semaphores[stop_id].move_queue = true;
            pthread_mutex_unlock(&semaphores[stop_id].mutex); 
        }
    }

    return      next_state;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE state_speed_up
//-------------------------------------------------------------------------------------------------------------------------

int state_speed_up (int trainId) { 
    int     next_state;
    int     vel;
    float   acc;

    next_state = SPEED_UP;
    acc = MAX_ACC;

    pthread_mutex_lock(&train_par[trainId].mutex);
    vel = train_par[trainId].currentVel;
    train_par[trainId].semaphore_flag = false;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    move(trainId, vel, acc);

    if (vel == MAX_VEL)     next_state = GO_FAST;

    return      next_state;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE clear_binary
//-------------------------------------------------------------------------------------------------------------------------

void clear_binary (int trainId, int binary) {

    pthread_mutex_lock(&train_par[trainId].mutex);
    if (train_par[trainId].station_passed[binary] && train_par[trainId].binary_occupied == true) {

        pthread_mutex_lock(&trains_in_binary_mutex);
        trains_in_binary[train_par[trainId].binary]--;
        pthread_mutex_unlock(&trains_in_binary_mutex);
        
        train_par[trainId].binary_occupied = false;
    }
    pthread_mutex_unlock(&train_par[trainId].mutex);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE out_of_bound_check;
//-------------------------------------------------------------------------------------------------------------------------

bool out_of_bound_check(int trainId, int direction){

    bool    train_in_bound;
    int     posx;

    train_in_bound = true;

    pthread_mutex_lock(&train_par[trainId].mutex);
    posx = train_par[trainId].posx;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    if      (direction == FROM_DX && posx < - TRAIN_SPACE)         train_in_bound = false;
    else if (direction == FROM_SX && posx > W + TRAIN_SPACE)       train_in_bound = false;

    return      train_in_bound;
    
}
//-------------------------------------------------------------------------------------------------------------------------
// TASK train
//
// Crea un nuovo task treno e ne gestisce il movimento
//-------------------------------------------------------------------------------------------------------------------------

void *train(void *p) {

    bool    train_in_bound;
    int     trainId;
    int     direction;
    int     binary;
    int     state;

    // INIT TASK
    trainId = get_task_id(p);
    set_activation(trainId);

    // INIT PARAMETRI DEL TRENO
    set_train_parameters(trainId);

    // ATTENDO ASSENGAZIONE BINARIO
    wait_for_binary_assignement(trainId);
    
    // CHECK COLLISIONI IN INGRESSO ALLA STAZIONE
    collision_check(trainId);

    // TRENO PRONTO 
    // Salvo il locale i parametri iniziali del treno
    pthread_mutex_lock(&train_par[trainId].mutex);
    train_par[trainId].run      = true;                         // Flag per segnalare che il treno è attivo
    direction                   = train_par[trainId].direction; // Direzione del treno
    binary                      = train_par[trainId].binary;    // Binario del treno
    pthread_mutex_unlock(&train_par[trainId].mutex);

    // INIT MACCHINA A STATI
    state = GO_FAST;
    train_in_bound = true;

    // TRENO IN MOVIMENTO
    while (EXIT == false && train_in_bound == true){
        
        // MACCHINA A STATI
        state = state_machine(trainId, direction, state);

        // DECREMENTO IL NUMERO DI TRENI CHE STA OCCUPANDO IL BINARIO SE SUPERO LA STAZIONE
        clear_binary(trainId, binary);

        // CONDIZIONE DI USCITA DAL CICLO
        train_in_bound = out_of_bound_check(trainId, direction);

        // DL MISS CHECK
        if (deadline_miss(trainId)) {
            printf("Deadline miss of train task %d \n", trainId);
            total_train_dl ++;                                          // Incremento variabile globale che conta tutti i DL miss dei task treno                              
        }

        wait_for_activation(trainId);
    }

    // USCITA DAL TASK
    train_par[trainId].run = false;
    pthread_mutex_destroy(&train_par[trainId].mutex);
    pthread_exit(NULL);

    return 0;
}


//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move
//
// muove il treno in orizzontale
//-------------------------------------------------------------------------------------------------------------------------
void move(int i, float vel, float acc){
    
    int     step;
    int     bin;
    int     j;
    int     wagon_posx;
    int     direction;
    float   periodS;
    float   newVel;
    float   deltaSpace;
    float   newStep;
    float   deltaVel;

    periodS     = (float) TRAIN_TASK_PERIOD / 1000.0;

    deltaVel    = acc * periodS;
    newVel      = vel + deltaVel;

    if (newVel > MAX_VEL)   newVel = MAX_VEL;
    if (newVel < 0)         newVel = 0;
    
    pthread_mutex_lock(&train_par[i].mutex);
    train_par[i].currentVel = newVel;
    direction = train_par[i].direction;
    bin         = train_par[i].binary;
    pthread_mutex_unlock(&train_par[i].mutex);

    deltaSpace  = newVel * periodS;
    step        = round(deltaSpace);

    switch (direction)
    {
    case FROM_SX:
        for (j = 0; j < WAGONS; j++) {

            pthread_mutex_lock(&train_par[i].mutex);
            wagon_posx = train_par[i].wagons[j].posx + TRAIN_W;
            pthread_mutex_unlock(&train_par[i].mutex);

            if      (wagon_posx > trails_xPoints[bin][0]   &&   wagon_posx < trails_xPoints[bin][1]){
                move_diag_up(i, j, step);
            }
            else if (wagon_posx > trails_xPoints[bin][2] + round(TRAIN_W*sqrt(2)/2)  &&   wagon_posx < trails_xPoints[bin][3]){
                move_diag_down(i, j, step);
            }
            else {
            
                // i due if qua sotto servono a riallineare i vagoni dopo essersi mossi in diagonale visto che si disallineavano
                if (wagon_posx < trails_xPoints[bin][2] + round(TRAIN_W*sqrt(2)/2)   &&   wagon_posx > trails_xPoints[bin][1]) {
                    train_par[i].wagons[j].posy = (bin + 1)*SPACE - TRAIN_H/2;

                }
                else if (wagon_posx > trails_xPoints[bin][3]) {
                    train_par[i].wagons[j].posy = (H - SPACE - TRAIN_H)/2;
                }
                move_forward(i, j, step);
            }
        }
        train_par[i].posx  = train_par[i].wagons[0].posx + TRAIN_W;
        break;

    case FROM_DX:
        for (j = 0; j < WAGONS; j++) {

            pthread_mutex_lock(&train_par[i].mutex);
            wagon_posx = train_par[i].wagons[j].posx;
            pthread_mutex_unlock(&train_par[i].mutex);

            if (wagon_posx > trails_xPoints[bin][2] - 18 &&   wagon_posx < trails_xPoints[bin][3] - 18){
                move_diag_up(i, j, -step);
            }
            else if (wagon_posx > trails_xPoints[bin][0] - 6  &&   wagon_posx < trails_xPoints[bin][1] - 5){
                move_diag_down(i, j, -step);
            }
            else {
                
                // i due if qua sotto servono a riallineare i vagoni dopo essersi mossi in diagonale visto che si disallineavano
                if (wagon_posx < trails_xPoints[bin][2] - 18   &&   wagon_posx > trails_xPoints[bin][1] - 5) {
                    train_par[i].wagons[j].posy = (bin + 1)*SPACE - TRAIN_H/2;

                }
                else if (wagon_posx < trails_xPoints[bin][0] - 6) {
                    train_par[i].wagons[j].posy = (H + SPACE - TRAIN_H)/2;
                }

                move_forward(i, j, -step);
            }
             
        }
        train_par[i].posx  = train_par[i].wagons[0].posx;
        break;
    default:
        break;
    }


}
/*-------------------------------------------------------------------------*/

void move_diag_up(int i, int j, int inc){
    int first_wagon;

    // Incremento diviso nelle due direzioni
    inc = round(inc*sqrt(2)/2);


    pthread_mutex_lock(&train_par[i].mutex);
    first_wagon = train_par[i].first_diagonal_wagon;

    // Se è la prima volta che il vagone si muove in diagonale, aggiorno flag e bmp
    if(train_par[i].wagons[j].diag_passed == false) {
        train_par[i].wagons[j].diag_passed = true;
        train_par[i].wagons[j].bmp = train_par[i].bmp[1];
    }
    
    // Il primo vagone incrementa la sua posizione, gli altri seguono il precedente
    if (j == first_wagon) {
        train_par[i].wagons[j].posx += inc;
        train_par[i].wagons[j].posy -= inc;
    }
    else {
        train_par[i].wagons[j].posx = train_par[i].wagons[j-1].posx - (sign(inc)) * round(sqrt(2)/2*(TRAIN_W + WAGONS_SPACE));
        train_par[i].wagons[j].posy = train_par[i].wagons[j-1].posy + (sign(inc)) * round(sqrt(2)/2*(TRAIN_W + WAGONS_SPACE));
    }
    pthread_mutex_unlock(&train_par[i].mutex);

}
/*-------------------------------------------------------------------------*/
void move_diag_down(int i, int j, int inc){
    int first_wagon;
    
    // Incremento diviso nelle due direzioni
    inc = round(inc*sqrt(2)/2);

    pthread_mutex_lock(&train_par[i].mutex);
    first_wagon = train_par[i].first_diagonal_wagon;

    // Se è la prima volta che il vagone si muove in diagonale, aggiorno flag e bmp
    if(train_par[i].wagons[j].diag_passed == false) {
        train_par[i].wagons[j].diag_passed = true;
        train_par[i].wagons[j].bmp = train_par[i].bmp[2];
    }
    
    // Il primo vagone incrementa la sua posizione, gli altri seguono il precedente
    if (j == first_wagon) {
        train_par[i].wagons[j].posx += inc;
        train_par[i].wagons[j].posy += inc;
    }
    else {
        train_par[i].wagons[j].posx = train_par[i].wagons[j-1].posx - (sign(inc)) * round(sqrt(2)/2*(TRAIN_W + WAGONS_SPACE));
        train_par[i].wagons[j].posy = train_par[i].wagons[j-1].posy - (sign(inc)) * round(sqrt(2)/2*(TRAIN_W + WAGONS_SPACE));
    }

    pthread_mutex_unlock(&train_par[i].mutex);

}
/*-------------------------------------------------------------------------*/
void move_forward(int i, int j, int inc) {

    // Se è appena uscito da un movimento diagonale, aggiorno la flag, il bitmap e 
    // il numero del primo vagone che si sta ancora muovendo in diagonale
    pthread_mutex_lock(&train_par[i].mutex);
    if (train_par[i].wagons[j].diag_passed == true) {
        train_par[i].wagons[j].diag_passed = false;
        train_par[i].wagons[j].bmp = train_par[i].bmp[0];
        if ( j == WAGONS - 1)   train_par[i].first_diagonal_wagon = 0;
        else                train_par[i].first_diagonal_wagon ++;   
    }
    if (j == 0)     train_par[i].wagons[j].posx += inc;
    else            train_par[i].wagons[j].posx = train_par[i].wagons[j-1].posx - (sign(inc)) * (TRAIN_W + WAGONS_SPACE);
    pthread_mutex_unlock(&train_par[i].mutex);
}

/*-------------------------------------------------------------------------*/
void exit_all(){
    int i;
    EXIT = true;
    for (i = 0; i <= TMAX + 2; i++) {
        pthread_join(tid[i],NULL);
    }
    allegro_exit(); 
    printf("All tasks closed!\n");
}


//----------------------------------------------------------------------------
// FUNZIONE sing
//
// restituisce il segno dell'intero
//----------------------------------------------------------------------------
int sign(int x) {
    if (x >= 0)     return 1;
    else            return -1;
}