#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "ptask.h"
#include "functions.h"
#include "init.h"
#include "user.h"
#include "station.h"
#include "graphics.h"

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE initialize()
//--------------------------------------------------------------------------------------------------------------------------------------------

void initialize() {
    
    printf("Loading ... \n");

    // INIT MUTEX
    mutex_init();

    // INIT VARIABILI GLOBALI
    global_variables_init();

    // INIT GRAFICA
    graphics_init();

    // INIT PULSANTI
    buttons_init();

    // CREAZIONE TASK
    task_create(graphics,           GRAPHIC_TASK_ID,            GRAPHIC_TASK_PERIOD,            GRAPHIC_TASK_DL,            GRAPHIC_TASK_PRIO);               
    task_create(station_manager,    STATION_MANAGER_TASK_ID,    STATION_MANAGER_TASK_PERIOD,    STATION_MANAGER_TASK_DL,    STATION_MANAGER_TASK_PRIO);   
    task_create(user_task,          USER_TASK_ID,               USER_TASK_PERIOD,               USER_TASK_DL,               USER_TASK_PRIO);                  

    printf("Initialization completed!\n");
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE mutex_init()
//
// Funzione che inizializza i mutex
//--------------------------------------------------------------------------------------------------------------------------------------------

void mutex_init() {
    int i;

    pthread_mutex_init(&ready_trains_num_mutex,             NULL);
    pthread_mutex_init(&last_assigned_train_id_mutex,       NULL);
    pthread_mutex_init(&trains_in_binary_mutex,             NULL);
    pthread_mutex_init(&ASSIGNED_DIRECTION_MUTEX,           NULL);
    pthread_mutex_init(&user_direction_mutex,               NULL);
    pthread_mutex_init(&AUTO_MODE_ON_MUTEX,                 NULL);
    pthread_mutex_init(&last_assigned_train_from_dx_mutex,  NULL);
    pthread_mutex_init(&last_assigned_train_from_sx_mutex,  NULL);
    pthread_mutex_init(&INIT_RED_TIME_SX_MUTEX,             NULL);
    pthread_mutex_init(&INIT_RED_TIME_DX_MUTEX,             NULL);
    pthread_mutex_init(&last_red_time_sx_mutex,             NULL);
    pthread_mutex_init(&last_red_time_dx_mutex,             NULL);
    pthread_mutex_init(&MOVE_TRAILS_SX_MUTEX,               NULL);
    pthread_mutex_init(&MOVE_TRAILS_DX_MUTEX,               NULL);
    pthread_mutex_init(&READY_TO_GO_SX_MUTEX,               NULL);
    pthread_mutex_init(&READY_TO_GO_DX_MUTEX,               NULL);
    pthread_mutex_init(&max_prio_train_sx_mutex,            NULL);
    pthread_mutex_init(&max_prio_train_dx_mutex,            NULL);

    for (i = 0; i < STATIONS_NUM; i++)      pthread_mutex_init(&station[i].mutex,       NULL);
    for (i = 0; i < SEMAPHORES_NUM; i++)    pthread_mutex_init(&semaphores[i].mutex,    NULL);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE global_variables_init()
//--------------------------------------------------------------------------------------------------------------------------------------------

void global_variables_init(){
    int i;

    EXIT                                    = false;
    EXIT_COMMAND                            = false;
    ASSIGNED_DIRECTION                      = false;
    AUTO_MODE_ON                            = false;
    INIT_RED_TIME_SX                        = false;
    INIT_RED_TIME_DX                        = false;
    MOVE_TRAILS_SX                          = false;
    MOVE_TRAILS_DX                          = false;
    READY_TO_GO_SX                          = true;
    READY_TO_GO_DX                          = true;
    next_trainId                            = 1;
    ready_trains_num                        = 0;
    last_assigned_train_id                  = 0;
    total_train_dl                          = 0;
    user_direction                          = FROM_DX;
    last_assigned_train_from_dx.tv_sec      = 0;
    last_assigned_train_from_dx.tv_nsec     = 0;
    last_assigned_train_from_sx.tv_sec      = 0;
    last_assigned_train_from_sx.tv_nsec     = 0;
    max_prio_train_sx                       = 0;
    max_prio_train_dx                       = 0;

    for (i = 0; i < STATIONS_NUM; i++)      trains_in_binary[i] = 0;
    
    // POSIZIONE DELLE INTERSEZIONI TRA I BINARI
    // binario 1
    trail1_xPoints[0] = W/2 - 7*SPACE;
    trail1_xPoints[1] = W/2 - 4*SPACE;
    trail1_xPoints[2] = W/2 + 4*SPACE;
    trail1_xPoints[3] = W/2 + 7*SPACE;

    // binario 2
    trail2_xPoints[0] = W/2 - 5*SPACE;
    trail2_xPoints[1] = W/2 - 3*SPACE;
    trail2_xPoints[2] = W/2 + 3*SPACE; 
    trail2_xPoints[3] = W/2 + 5*SPACE;

    // binario 3
    trail3_xPoints[0] = W/2 - 3*SPACE; 
    trail3_xPoints[1] = W/2 - 2*SPACE; 
    trail3_xPoints[2] = W/2 + 2*SPACE; 
    trail3_xPoints[3] = W/2 + 3*SPACE;

    // binario 4 
    trail4_xPoints[0] = W; 
    trail4_xPoints[1] = W; 
    trail4_xPoints[2] = W; 
    trail4_xPoints[3] = W;

    // intersezioni raggruppate
    trails_xPoints[0] = trail1_xPoints;
    trails_xPoints[1] = trail2_xPoints; 
    trails_xPoints[2] = trail3_xPoints; 
    trails_xPoints[3] = trail4_xPoints;
    trails_xPoints[4] = trail4_xPoints;
    trails_xPoints[5] = trail3_xPoints;
    trails_xPoints[6] = trail2_xPoints;
    trails_xPoints[7] = trail1_xPoints;

    semaphores_struct_init();
    stations_struct_init();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE semaphores_struct_init()
//--------------------------------------------------------------------------------------------------------------------------------------------

void semaphores_struct_init() {
    int i;

    // STRUTTURA SEMAFORI
    for (i = 0; i<=2; i++){
        semaphores[i].xPointStop    = W/2 + (2*i - 7)*SPACE - 5;
        semaphores[i].xPointDraw    = semaphores[i].xPointStop + 5;
        semaphores[i].yPointDraw    = H/2 - SPACE/2;
        semaphores[i].xPointTrail   = semaphores[i].xPointDraw;
        semaphores[i].yPointTrail   = semaphores[i].yPointDraw - TRAIL_H/2;
        semaphores[i].trail_angle   = TRAIL_UP_BIN_IN_SWITCH_OFF;
        semaphores[i].xPointIn      = semaphores[i].xPointStop - SLOW_DOWN_SPACE;
        semaphores[i].xPointOut     = semaphores[i].xPointDraw + TRAIL_W;
        
        semaphores[i+6].xPointStop  = W/2 + (i - 4)*SPACE + 5;
        semaphores[i+6].xPointDraw  = W/2 - (i+2)*SPACE;
        semaphores[i+6].yPointDraw  = (6+i)*SPACE;
        semaphores[i+6].xPointTrail = W/2 + (2*i - 7)*SPACE;
        semaphores[i+6].yPointTrail = H/2 + SPACE/2 - TRAIL_H/2;
        semaphores[i+6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
        semaphores[i+6].xPointIn    = semaphores[i+6].xPointStop + SLOW_DOWN_SPACE;
        semaphores[i+6].xPointOut   = trails_xPoints[0][0];
    }

    for (i = 3; i<=5; i++){
        semaphores[i].xPointStop    = W/2 +(i-1)*SPACE - 5;
        semaphores[i].xPointDraw    = semaphores[i].xPointStop + 5;
        semaphores[i].yPointDraw    = (6-i)*SPACE;
        semaphores[i].xPointTrail   = W/2 + (2*i - 3)*SPACE; 
        semaphores[i].yPointTrail   = H/2 - SPACE/2 + TRAIL_H/2;
        semaphores[i].trail_angle   = TRAIL_UP_BIN_OUT_SWITCH_OFF;
        semaphores[i].xPointIn      = semaphores[i].xPointStop - SLOW_DOWN_SPACE;
        semaphores[i].xPointOut     = trails_xPoints[0][3];
           
        semaphores[i+6].xPointStop  = W/2 + (2*i - 3)*SPACE + 5;
        semaphores[i+6].xPointDraw  = semaphores[i+6].xPointStop - 5;
        semaphores[i+6].yPointDraw  = H/2 + SPACE/2;
        semaphores[i+6].xPointTrail = semaphores[i+6].xPointDraw;
        semaphores[i+6].yPointTrail = semaphores[i+6].yPointDraw + TRAIL_H/2;
        semaphores[i+6].trail_angle = TRAIL_DOWN_BIN_IN_SWITCH_OFF;
        semaphores[i+6].xPointIn    = semaphores[i+6].xPointStop + SLOW_DOWN_SPACE;
        semaphores[i+6].xPointOut   = semaphores[i+6].xPointDraw - TRAIL_W;
    }

    for (i = 0; i < SEMAPHORES_NUM; i++){
        semaphores[i].queue             = 0;
        semaphores[i].status            = true;
        semaphores[i].trail_state       = TRAIL_OFF;
        semaphores[i].move_queue        = false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE stations_struct_init()
//--------------------------------------------------------------------------------------------------------------------------------------------

void stations_struct_init() {
    int i;

    // STRUTTURA STAZIONE
    for (i = 0; i < STATIONS_NUM/2; i++){
        station[i].xPointStop = (W/2) + TRAIN_W;
        station[i].xPointDraw = station[i].xPointStop + 5;
        station[i].yPointDraw = (i + 1) * SPACE;
        station[i].xPointIn   = station[i].xPointStop - SLOW_DOWN_SPACE;
        station[i].xPointOut  = station[i].xPointDraw + TRAIN_W;
        station[i].status     = true;
        station[i].queue      = 0; 
        station[i].move_queue = false;
    }
        for (i = STATIONS_NUM/2; i < STATIONS_NUM; i++){
        station[i].xPointStop = (W/2) - TRAIN_W;
        station[i].xPointDraw = station[i].xPointStop - 5;
        station[i].yPointDraw = (i + 1) * SPACE;
        station[i].xPointIn   = station[i].xPointStop + SLOW_DOWN_SPACE;
        station[i].xPointOut  = station[i].xPointDraw - TRAIN_W;
        station[i].status     = true;
        station[i].queue      = 0; 
        station[i].move_queue = false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE buttons_init()
//--------------------------------------------------------------------------------------------------------------------------------------------

void buttons_init() {

    // STRUTTURA PULSANTI
    // new random train
    button[0].button_off    = random_train_off;
    button[0].button_on     = random_train_on;
    button[0].state         = false;
    button[0].x_min         = 2*SPACE_BUTTONS;
    button[0].x_max         = 2*SPACE_BUTTONS + L_BUTTONS;
    button[0].y_min         = SPACE_BUTTONS;
    button[0].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // new hp train
    button[1].button_off    = hp_train_off;
    button[1].button_on     = hp_train_on;
    button[1].state         = false;
    button[1].x_min         = button[0].x_max + 2*SPACE_BUTTONS;
    button[1].x_max         = button[1].x_min + L_BUTTONS;
    button[1].y_min         = SPACE_BUTTONS;
    button[1].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // new mp train
    button[2].button_off    = mp_train_off;
    button[2].button_on     = mp_train_on;
    button[2].state         = false;
    button[2].x_min         = button[1].x_max + 2*SPACE_BUTTONS;
    button[2].x_max         = button[2].x_min + L_BUTTONS;
    button[2].y_min         = SPACE_BUTTONS;
    button[2].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // new lp train
    button[3].button_off    = lp_train_off;
    button[3].button_on     = lp_train_on;    
    button[3].state         = false;
    button[3].x_min         = button[2].x_max + 2*SPACE_BUTTONS;
    button[3].x_max         = button[3].x_min + L_BUTTONS;
    button[3].y_min         = SPACE_BUTTONS;
    button[3].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // train from dx
    button[4].button_off    = from_dx_off;
    button[4].button_on     = from_dx_on;    
    button[4].state         = false;
    button[4].x_min         = button[3].x_max + 2*SPACE_BUTTONS;
    button[4].x_max         = button[4].x_min + L_BUTTONS;
    button[4].y_min         = SPACE_BUTTONS;
    button[4].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // train from sx
    button[5].button_off    = from_sx_off;
    button[5].button_on     = from_sx_on;    
    button[5].state         = false;
    button[5].x_min         = button[4].x_max + 2*SPACE_BUTTONS;
    button[5].x_max         = button[5].x_min + L_BUTTONS;
    button[5].y_min         = SPACE_BUTTONS;
    button[5].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // random direction
    button[6].button_off    = rnd_direction_off;
    button[6].button_on     = rnd_direction_on;    
    button[6].state         = false;
    button[6].x_min         = button[5].x_max + 2*SPACE_BUTTONS;
    button[6].x_max         = button[6].x_min + L_BUTTONS;
    button[6].y_min         = SPACE_BUTTONS;
    button[6].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // auto mode
    button[7].button_off    = auto_mode_off;
    button[7].button_on     = auto_mode_on;    
    button[7].state         = false;
    button[7].x_min         = button[6].x_max + 2*SPACE_BUTTONS;
    button[7].x_max         = button[7].x_min + L_BUTTONS;
    button[7].y_min         = SPACE_BUTTONS;
    button[7].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // close program
    button[8].button_off    = close_program_off;
    button[8].button_on     = close_program_on;
    button[8].state         = false;
    button[8].x_min         = INTERFACE_W - 2*SPACE_BUTTONS - L_BUTTONS;
    button[8].x_max         = button[8].x_min + L_BUTTONS;
    button[8].y_min         = SPACE_BUTTONS;
    button[8].y_max         = SPACE_BUTTONS + L_BUTTONS;
}