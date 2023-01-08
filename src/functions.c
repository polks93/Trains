#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "init.h"
#include "ptask.h"
#include "graphics.h"
#include "functions.h"

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE stationOutSx
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void stationOutSx() {
    bool    train_found;
    bool    ready_to_go;
    bool    init_red_time;
    bool    station_status;
    bool    another_station_waiting;
    bool    move_trails;
    int     train_id;
    int     max_priority_train_id;
    int     priority;
    int     binary;
    int     max_priority_train_bin;
    int     max_priority;
    int     i;
    int     bin;
    int     posx;
    struct  timespec    now;
    struct  timespec    leave_time;

    ready_to_go             = false;
    train_found             = false;
    another_station_waiting = false;

    max_priority            = 0;
    max_priority_train_bin  = 0;

    pthread_mutex_lock(&INIT_RED_TIME_SX_MUTEX);
    init_red_time = INIT_RED_TIME_SX;
    pthread_mutex_unlock(&INIT_RED_TIME_SX_MUTEX);

    if (init_red_time == true) {

        pthread_mutex_lock(&last_red_time_sx_mutex);
        time_copy(&leave_time, last_red_time_sx);
        pthread_mutex_unlock(&last_red_time_sx_mutex);

        time_add_ms(&leave_time, STOP_TIME);
        clock_gettime(CLOCK_MONOTONIC, &now);

        pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);
        if (time_cmp(now, leave_time) > 0 && READY_TO_GO_SX == true) {
            ready_to_go = true;
        }
        pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);
    }

    if (ready_to_go == true) {

        for (i = 0; i <= 3; i++) {
            
            pthread_mutex_lock(&station[i].mutex);
            train_id = station[i].queue_list[0];
            pthread_mutex_unlock(&station[i].mutex);

            pthread_mutex_lock(&train_par[train_id].mutex);
            priority    = train_par[train_id].priority;
            binary      = train_par[train_id].binary;
            pthread_mutex_unlock(&train_par[train_id].mutex);

            if (priority > max_priority) {

                max_priority            = priority;
                max_priority_train_id   = train_id;
                max_priority_train_bin  = binary;
                train_found             = true;
            }
            else if (priority == max_priority && binary > max_priority_train_bin) {

                max_priority            = priority;
                max_priority_train_id   = train_id;
                max_priority_train_bin  = binary;
                train_found             = true;
            }
        }

        if (train_found == true) {

            pthread_mutex_lock(&station[max_priority_train_bin].mutex);
            station[max_priority_train_bin].status = true;
            pthread_mutex_unlock(&station[max_priority_train_bin].mutex);

            pthread_mutex_lock(&train_par[max_priority_train_id].mutex);
            train_par[max_priority_train_id].station_passed[max_priority_train_bin] = true;
            pthread_mutex_unlock(&train_par[max_priority_train_id].mutex);

            pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
            MOVE_TRAILS_SX = true;
            pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);

            pthread_mutex_lock(&READY_TO_GO_SX_MUTEX);            
            READY_TO_GO_SX = false;
            pthread_mutex_unlock(&READY_TO_GO_SX_MUTEX);

            pthread_mutex_lock(&max_prio_train_sx_mutex);
            max_prio_train_sx = max_priority_train_id;
            pthread_mutex_unlock(&max_prio_train_sx_mutex);
        }

        // Se ci sono altri semafori rossi a questo punto, aggiorno l'istante in cui il prossimo sem deve diventare verde
        for (i = 0; i<= 3; i++) {

            pthread_mutex_lock(&station[i].mutex);
            station_status = station[i].status;
            pthread_mutex_unlock(&station[i].mutex);

            if (station_status == false) {
                another_station_waiting = true;
                break;
            }
        }

        if (another_station_waiting == true) {
            pthread_mutex_lock(&last_red_time_sx_mutex);
            time_add_ms(&last_red_time_sx, STOP_TIME);
            pthread_mutex_unlock(&last_red_time_sx_mutex);
        }

        else {
            pthread_mutex_lock(&INIT_RED_TIME_SX_MUTEX);
            INIT_RED_TIME_SX = false;
            pthread_mutex_unlock(&INIT_RED_TIME_SX_MUTEX);
        }
    }
    
    pthread_mutex_lock(&MOVE_TRAILS_SX_MUTEX);
    move_trails = MOVE_TRAILS_SX;
    pthread_mutex_unlock(&MOVE_TRAILS_SX_MUTEX);

    if (move_trails == true) {

        pthread_mutex_lock(&max_prio_train_sx_mutex);
        train_id = max_prio_train_sx;
        pthread_mutex_unlock(&max_prio_train_sx_mutex);

        pthread_mutex_lock(&train_par[train_id].mutex);
        bin     = train_par[train_id].binary;
        posx    = train_par[train_id].posx;
        pthread_mutex_unlock(&train_par[train_id].mutex);

        switch (bin) {
            case 0:

                pthread_mutex_lock(&semaphores[5].mutex);
                semaphores[5].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[5].trail_angle >= TRAIL_UP_BIN_OUT_SWITCH_ON) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
                }

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

            break;
            
            case 1:
                pthread_mutex_lock(&semaphores[4].mutex);
                pthread_mutex_lock(&semaphores[5].mutex);
                semaphores[4].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[4].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {
                    semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
                } 

                semaphores[5].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }

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

            break;

            case 2:

                pthread_mutex_lock(&semaphores[3].mutex);
                pthread_mutex_lock(&semaphores[4].mutex);
                pthread_mutex_lock(&semaphores[5].mutex);
                semaphores[3].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[3].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {
                    semaphores[3].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
                }
                semaphores[4].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[4].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }
                semaphores[5].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }

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
            break;

            case 3:

                pthread_mutex_lock(&semaphores[3].mutex);
                pthread_mutex_lock(&semaphores[4].mutex);
                pthread_mutex_lock(&semaphores[5].mutex);

                semaphores[3].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[3].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[3].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }

                semaphores[4].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[4].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                } 

                semaphores[5].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }

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
            break;

            default:
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE stationOutDx
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void stationOutDx(){
    bool    train_found;
    bool    ready_to_go;
    bool    init_red_time;
    bool    station_status;
    bool    another_station_waiting;
    bool    move_trails;
    int     train_id;
    int     max_priority_train_id;
    int     priority;
    int     binary;
    int     max_priority_train_bin;
    int     max_priority;
    int     i;
    int     bin;
    int     posx;
    struct  timespec    now;
    struct  timespec    leave_time;

    ready_to_go             = false;
    train_found             = false;
    another_station_waiting = false;

    max_priority            = 0;
    max_priority_train_bin  = 8;

    pthread_mutex_lock(&INIT_RED_TIME_DX_MUTEX);
    init_red_time = INIT_RED_TIME_DX;
    pthread_mutex_unlock(&INIT_RED_TIME_DX_MUTEX);

    if (init_red_time == true) {

        pthread_mutex_lock(&last_red_time_dx_mutex);
        time_copy(&leave_time, last_red_time_dx);
        pthread_mutex_unlock(&last_red_time_dx_mutex);

        time_add_ms(&leave_time, STOP_TIME);
        clock_gettime(CLOCK_MONOTONIC, &now);

        pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);
        if (time_cmp(now, leave_time) > 0 && READY_TO_GO_DX == true) {
            ready_to_go = true;
        }
        pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);
    }

    if (ready_to_go == true) {

        for (i = 4; i <= 7; i++) {
            
            pthread_mutex_lock(&station[i].mutex);
            train_id = station[i].queue_list[0];
            pthread_mutex_unlock(&station[i].mutex);

            pthread_mutex_lock(&train_par[train_id].mutex);
            priority    = train_par[train_id].priority;
            binary      = train_par[train_id].binary;
            pthread_mutex_unlock(&train_par[train_id].mutex);

            if (priority > max_priority) {

                max_priority            = priority;
                max_priority_train_id   = train_id;
                max_priority_train_bin  = binary;
                train_found             = true;
            }
            else if (priority == max_priority && binary < max_priority_train_bin) {

                max_priority            = priority;
                max_priority_train_id   = train_id;
                max_priority_train_bin  = binary;
                train_found             = true;
            }
        }

        if (train_found == true) {

            pthread_mutex_lock(&station[max_priority_train_bin].mutex);
            station[max_priority_train_bin].status = true;
            pthread_mutex_unlock(&station[max_priority_train_bin].mutex);

            pthread_mutex_lock(&train_par[max_priority_train_id].mutex);
            train_par[max_priority_train_id].station_passed[max_priority_train_bin] = true;
            pthread_mutex_unlock(&train_par[max_priority_train_id].mutex);

            pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
            MOVE_TRAILS_DX = true;
            pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);

            pthread_mutex_lock(&READY_TO_GO_DX_MUTEX);            
            READY_TO_GO_DX = false;
            pthread_mutex_unlock(&READY_TO_GO_DX_MUTEX);

            pthread_mutex_lock(&max_prio_train_dx_mutex);
            max_prio_train_dx = max_priority_train_id;
            pthread_mutex_unlock(&max_prio_train_dx_mutex);
        }

        // Se ci sono altri semafori rossi a questo punto, aggiorno l'istante in cui il prossimo sem deve diventare verde
        for (i = 4; i<= 7; i++) {

            pthread_mutex_lock(&station[i].mutex);
            station_status = station[i].status;
            pthread_mutex_unlock(&station[i].mutex);

            if (station_status == false) {
                another_station_waiting = true;
                break;
            }
        }

        if (another_station_waiting == true) {
            pthread_mutex_lock(&last_red_time_dx_mutex);
            time_add_ms(&last_red_time_dx, STOP_TIME);
            pthread_mutex_unlock(&last_red_time_dx_mutex);
        }

        else {
            pthread_mutex_lock(&INIT_RED_TIME_DX_MUTEX);
            INIT_RED_TIME_DX = false;
            pthread_mutex_unlock(&INIT_RED_TIME_DX_MUTEX);
        }
    }

    pthread_mutex_lock(&MOVE_TRAILS_DX_MUTEX);
    move_trails = MOVE_TRAILS_DX;
    pthread_mutex_unlock(&MOVE_TRAILS_DX_MUTEX);


    if (move_trails == true) {

        pthread_mutex_lock(&max_prio_train_dx_mutex);
        train_id = max_prio_train_dx;
        pthread_mutex_unlock(&max_prio_train_dx_mutex);

        pthread_mutex_lock(&train_par[train_id].mutex);
        bin     = train_par[train_id].binary;
        posx    = train_par[train_id].posx;
        pthread_mutex_unlock(&train_par[train_id].mutex);

        switch (bin) {
                
            case 4:

                pthread_mutex_lock(&semaphores[6].mutex);
                pthread_mutex_lock(&semaphores[7].mutex);
                pthread_mutex_lock(&semaphores[8].mutex);

                semaphores[6].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[6].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {
                    semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
                }

                semaphores[7].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[7].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {
                    semaphores[7].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
                } 

                semaphores[8].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[8].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {
                    semaphores[8].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
                }

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
            break;

            case 5:
                pthread_mutex_lock(&semaphores[6].mutex);
                pthread_mutex_lock(&semaphores[7].mutex);
                pthread_mutex_lock(&semaphores[8].mutex);
                semaphores[6].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[6].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {
                    semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
                }
                semaphores[7].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[7].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {
                    semaphores[7].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
                }
                semaphores[8].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[8].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_ON) {
                    semaphores[8].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_ON;
                }

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
            break;

            case 6:
                pthread_mutex_lock(&semaphores[7].mutex);
                pthread_mutex_lock(&semaphores[6].mutex);
                semaphores[7].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[7].trail_angle > TRAIL_DOWN_BIN_OUT_SWITCH_ON) {
                    semaphores[7].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_ON;
                } 

                semaphores[6].trail_angle -= TRAIL_ANGLE_INC;
                if (semaphores[6].trail_angle < TRAIL_DOWN_BIN_OUT_SWITCH_OFF) {
                    semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
                }

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
            break;

            case 7:
                pthread_mutex_lock(&semaphores[6].mutex);
                semaphores[6].trail_angle += TRAIL_ANGLE_INC;
                if (semaphores[6].trail_angle >= TRAIL_DOWN_BIN_OUT_SWITCH_ON) {
                    semaphores[6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_ON;
                }

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

            break;
            
            default:
            break;
        }
    }

}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_station_queue
//
// Muove la coda di tutte le stazioni
//-------------------------------------------------------------------------------------------------------------------------
void move_station_queue(int stationId){

    struct  timespec now;
    int     i;
    int     trainId;
    int     queue;
    int     direction;

    if (stationId < 4)      direction = FROM_SX;
    else                    direction = FROM_DX;

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
// TASK train
//
// Crea un nuovo task treno e ne gestisce il movimento
//-------------------------------------------------------------------------------------------------------------------------

void *train(void *p) {
    struct  timespec    now;
    struct  timespec    ready_time;
    bool    first_of_queue;
    bool    semaphore_flag;
    bool    binary_assigned;
    bool    stop_status;
    int     id;
    int     k;
    int     j;
    int     direction;
    int     binary;
    int     stop_id;
    int     stop_type;
    int     posx;
    int     stop_x;
    int     previous_train_pos_x;
    int     curr_state;
    int     prev_state;
    int     next_state;
    int     acc;
    float   vel;

    // INIT TASK
    id = get_task_id(p);
    set_activation(id);
    set_train_parameters(id);

    binary_assigned = false;

    // ATTENDO ASSEGNAZIONE BINARIO
    while(binary_assigned == false && EXIT == false){

        pthread_mutex_lock(&train_par[id].mutex);
        binary_assigned = train_par[id].binary_assigned;
        pthread_mutex_unlock(&train_par[id].mutex);
        if (deadline_miss(id)) {
            printf("Deadline miss of train task %d \n", id);
            total_train_dl ++;
        }
        wait_for_activation(id);
    }
    
    // CHECK COLLISIONI IN INGRESSO ALLA STAZIONE
    pthread_mutex_lock(&train_par[id].mutex);
    direction   = train_par[id].direction;
    pthread_mutex_unlock(&train_par[id].mutex);

    k = 0;
    while (k < 1 && EXIT == false) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        switch(direction) {

            case FROM_DX:
            pthread_mutex_lock(&last_assigned_train_from_dx_mutex);
            time_copy(&ready_time, last_assigned_train_from_dx);
            time_add_ms(&ready_time, MIN_MS_BETWEEN_TRAINS);
            k = time_cmp(now, ready_time);
            if ( k == 1)    time_copy(&last_assigned_train_from_dx, now);
            pthread_mutex_unlock(&last_assigned_train_from_dx_mutex);
                break;
            
            case FROM_SX:
            pthread_mutex_lock(&last_assigned_train_from_sx_mutex);
            time_copy(&ready_time, last_assigned_train_from_sx);
            time_add_ms(&ready_time, MIN_MS_BETWEEN_TRAINS);
            k = time_cmp(now, ready_time);
            if ( k == 1)    time_copy(&last_assigned_train_from_sx, now);
            pthread_mutex_unlock(&last_assigned_train_from_sx_mutex);
                break;
            
            default:
                break;
        }

        if (deadline_miss(id)) {
            printf("Deadline miss of train task %d \n", id);
            total_train_dl ++;
        }
        wait_for_activation(id);
    }


    // TRENO PRONTO 
    // Salvo il locale i parametri iniziali del treno
    pthread_mutex_lock(&train_par[id].mutex);
    train_par[id].run   = true;
    posx                = train_par[id].posx;
    stop_x              = train_par[id].stop_x;
    direction           = train_par[id].direction;
    binary              = train_par[id].binary;
    pthread_mutex_unlock(&train_par[id].mutex);

    // INIT MACCHINA A STATI
    curr_state = GO_FAST;
    prev_state = curr_state;
    next_state = curr_state;

    // TRENO IN MOVIMENTO
    while (EXIT == false){

        // Aggiorno in locale i parametri del treno
        pthread_mutex_lock(&train_par[id].mutex);
        posx                = train_par[id].posx;
        stop_x              = train_par[id].stop_x;
        semaphore_flag      = train_par[id].semaphore_flag;
        if (train_par[id].pos_in_queue == 0)    first_of_queue = true;
        else                                    first_of_queue = false;
        pthread_mutex_unlock(&train_par[id].mutex);

        // MACCHINA A STATI
        switch (curr_state){

            case(GO_FAST):
                // MOVIMENTO NORMALE DEL TRENO
                acc = 0;
                vel = MAX_VEL;
                move(id, vel, acc);

                // Check fermata
                if (semaphore_flag == true){

                    pthread_mutex_lock(&train_par[id].mutex);
                    stop_id     = train_par[id].stop_id;
                    stop_type   = train_par[id].stop_type;
                    pthread_mutex_unlock(&train_par[id].mutex);

                    next_state  = SLOW_DOWN;
                }
                break;

            case(SLOW_DOWN):
                // RALLENTO PRIMA DI UN SEMAFORO
                acc = - MAX_ACC;

                pthread_mutex_lock(&train_par[id].mutex);
                vel = train_par[id].currentVel;
                pthread_mutex_unlock(&train_par[id].mutex);

                move(id, vel, acc);

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

                break;

            case(QUEUE):  
                // MOVIMENTO DELLA CODA DEL SEMAFORO
                pthread_mutex_lock(&train_par[id].mutex);
                if (train_par[id].ready_to_go_from_queue == true) {

                    train_par[id].ready_to_go_from_queue  = false;
                    train_par[id].queue             = true;
                    train_par[id].currentVel        = MAX_VEL;
                    train_par[id].semaphore_flag    = false;
                    train_par[id].pos_in_queue --;

                    switch (direction){

                        case FROM_DX:
                            train_par[id].stop_x -= TRAIN_SPACE;
                            break;

                        case FROM_SX:
                            train_par[id].stop_x += TRAIN_SPACE;
                            break;

                        default:
                            break;
                    }
                    next_state = SLOW_DOWN;
                }
                pthread_mutex_unlock(&train_par[id].mutex);

                break;

            case(STOP):                
                if (stop_type == STATION) {

                    pthread_mutex_lock(&station[stop_id].mutex);
                    stop_status = station[stop_id].status;
                    pthread_mutex_unlock(&station[stop_id].mutex);

                    if (stop_status == true) {
                        next_state = SPEED_UP;
                        
                        pthread_mutex_lock(&train_par[id].mutex);
                        train_par[id].queue = false;
                        pthread_mutex_unlock(&train_par[id].mutex);

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

                        pthread_mutex_lock(&train_par[id].mutex);
                        train_par[id].queue = false;
                        pthread_mutex_unlock(&train_par[id].mutex);

                        pthread_mutex_lock(&semaphores[stop_id].mutex);
                        semaphores[stop_id].move_queue = true;
                        pthread_mutex_unlock(&semaphores[stop_id].mutex); 
                    }
                }
                break;

            case(SPEED_UP):
                // USCITA DAL SEMAFORO
                acc = MAX_ACC;

                pthread_mutex_lock(&train_par[id].mutex);
                vel = train_par[id].currentVel;
                train_par[id].semaphore_flag = false;
                pthread_mutex_unlock(&train_par[id].mutex);

                move(id, vel, acc);

                if (vel == MAX_VEL)     next_state = GO_FAST;
                break;

            default:
                break;
        }

        // AGGIORNO GLI STATI
        prev_state = curr_state;
        curr_state = next_state;

        // DECREMENTO IL NUMERO DI TRENI CHE STA OCCUPANDO IL BINARIO QUANDO SUPERO LA STAZIONE
        pthread_mutex_lock(&train_par[id].mutex);
        if (train_par[id].station_passed[binary] && train_par[id].binary_occupied == true) {

            pthread_mutex_lock(&trains_in_binary_mutex);
            trains_in_binary[train_par[id].binary]--;
            pthread_mutex_unlock(&trains_in_binary_mutex);
            
            train_par[id].binary_occupied = false;
        }
        pthread_mutex_unlock(&train_par[id].mutex);

        // CONDIZIONE DI USCITA DAL CICLO
        if      (direction == FROM_DX && posx < - TRAIN_SPACE)          break;
        else if (direction == FROM_SX && posx > W + TRAIN_SPACE)        break;

        // ATTENDO IL PROSSIMO CICLO
        if (deadline_miss(id)) {
            printf("Deadline miss of train task %d \n", id);
            total_train_dl ++;
        }

        wait_for_activation(id);
    }

    // USCITA DAL TASK
    train_par[id].run = false;
    pthread_mutex_destroy(&train_par[id].mutex);
    pthread_exit(NULL);

    return 0;
}

//----------------------------------------------------------------------------
// FUNZIONE set_train_parameters
//
// inizializza tutti i parametri del treno
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
    train_par[i].run                = false;     
    train_par[i].binary_assigned    = false; 
    train_par[i].ready_to_go_from_queue   = false; 
    train_par[i].semaphore_flag     = false; 
    train_par[i].checked            = false;
    train_par[i].queue              = false;
    train_par[i].binary_occupied    = false;

    // Struttura per indicare se il treno è già passato da una stazione
    for (j = 0; j < STATIONS_NUM; j++)      train_par[i].station_passed[j] = false;
    // Struttura per indicare se il treno è già passato da un semaforo
    for (j = 0; j < SEMAPHORES_NUM; j++)    train_par[i].sem_passed[j] = false;

    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
    pthread_mutex_lock(&user_direction_mutex);
    if (ASSIGNED_DIRECTION == true)     train_par[i].direction = user_direction;
    else                                train_par[i].direction = (random_num)%2; // direzione random da 0 a 1
    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);    
    pthread_mutex_unlock(&user_direction_mutex);
    
    // Se non viene indicata una priorità, ne scelgo una random da 1 a 3
    if (train_par[i].priority == 0)             train_par[i].priority = 1 + (random_num)%3;
    if (train_par[i].direction == FROM_DX)      train_par[i].posx = - TRAIN_W;
    else                                        train_par[i].posx = W;
    
    train_par[i].stop_x                 = W;
    train_par[i].stop_type              = SEMAPHORE;
    train_par[i].stop_id                = 11*train_par[i].direction;
    train_par[i].pos_in_queue           = 0;
    train_par[i].first_diagonal_wagon   = 0;
    train_par[i].currentVel             = MAX_VEL;
    train_par[i].mutex                  = train_mux;
    train_par[i].bmp[0]                 = train_bmp[train_par[i].priority - 1].train1;
    train_par[i].bmp[1]                 = train_bmp[train_par[i].priority - 1].train2;
    train_par[i].bmp[2]                 = train_bmp[train_par[i].priority - 1].train3;

    // Parametri dei vari vagoni
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