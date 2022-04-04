#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "ptask.h"
#include "functions.h"

//----------------------------------------------------------------------
// FUNZIONE initialize()
//
// Funzione che inizializza alcune variabili, allegro e disegna il background
// eventualmente sarebbe più facile caricare direttamente uno sfondo
//----------------------------------------------------------------------

void initialize() {
    int x0, x1, x2, x3, y0, y1, y2, y3;
    int i;
    int col, black, red, blue, green, orange, white, grey;

    px2m = 100.0/W;
    prova = 0;
    EXIT = false;
    EXIT_COMMAND = false;

    allegro_init();
    set_color_depth(15);
    black   = makecol(0, 0, 0);
    red     = makecol(0, 0, 255);
    blue    = makecol(165, 165, 0);
    green   = makecol(0, 255, 0);
    orange  = makecol(0, 160, 255);
    white   = makecol(255, 255,255);
    grey    = makecol(50, 50, 50);
    ready_trains_num = 0;
    going_trains_num = 0;
    last_assigned_train_id = 0;

    for (i=0; i < STATIONS_NUM; i++) trains_in_binary[i]=0;
    for (i = 1; i<TMAX; i++) train_par[i].run = false;
    
    sem_g = load_bitmap("img/sem/sem_green.bmp", NULL);
    sem_r = load_bitmap("img/sem/sem_red.bmp", NULL);

    // loading bitmaps of the different trains
    // orange binario 1
    train_bmp[0].train1 = load_bitmap("img/trains/orange.bmp", NULL);
    train_bmp[0].train2 = load_bitmap("img/trains/orange_diag.bmp", NULL);
    train_bmp[0].train3 = load_bitmap("img/trains/orange_diag2.bmp", NULL);

    // green binario 2
    train_bmp[1].train1 = load_bitmap("img/trains/green.bmp", NULL);
    train_bmp[1].train2 = load_bitmap("img/trains/green_diag.bmp", NULL);
    train_bmp[1].train3 = load_bitmap("img/trains/green_diag2.bmp", NULL);    
    
    // blue binario 3
    train_bmp[2].train1 = load_bitmap("img/trains/blue.bmp", NULL);
    train_bmp[2].train2 = load_bitmap("img/trains/blue_diag.bmp", NULL);
    train_bmp[2].train3 = load_bitmap("img/trains/blue_diag2.bmp", NULL);

    // red binario 4
    train_bmp[3].train1 = load_bitmap("img/trains/red.bmp", NULL);
    train_bmp[3].train2 = load_bitmap("img/trains/red_diag.bmp", NULL);
    train_bmp[3].train3 = load_bitmap("img/trains/red_diag2.bmp", NULL);

    // bitmap dei binari mobili
    trail = load_bitmap("img/trail.bmp", NULL);

    // bitmaps dei bottoni
    random_train_on = load_bitmap("img/buttons/random_on.bmp", NULL);
    random_train_off = load_bitmap("img/buttons/random_off.bmp", NULL);

    hp_train_on = load_bitmap("img/buttons/hp_on.bmp", NULL);
    hp_train_off = load_bitmap("img/buttons/hp_off.bmp", NULL);

    mp_train_off = load_bitmap("img/buttons/mp_off.bmp", NULL);
    mp_train_on = load_bitmap("img/buttons/mp_on.bmp", NULL);

    lp_train_off = load_bitmap("img/buttons/lp_off.bmp", NULL);
    lp_train_on = load_bitmap("img/buttons/lp_on.bmp", NULL);

    close_program_off = load_bitmap("img/buttons/exit_off.bmp", NULL);
    close_program_on = load_bitmap("img/buttons/exit_on.bmp", NULL);
  
    install_keyboard();
    install_mouse();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H_window, 0, 0);
    background = create_bitmap(W, H);
    buffer = create_bitmap(W, H);
    interface = create_bitmap(INTERFACE_W, INTERFACE_H);
    interface_buffer = create_bitmap(INTERFACE_W, INTERFACE_H);
    clear_to_color(background, grey);
    clear_to_color(interface, grey);
    rectfill(background, 0, H-1, W-1, H-10, black);

    // struttra dei pulsanti
    button[0].button_off = random_train_off;
    button[0].button_on = random_train_on;
    button[0].state = false;
    button[0].x_min = 2*SPACE_BUTTONS;
    button[0].x_max = 2*SPACE_BUTTONS + L_BUTTONS;
    button[0].y_min = SPACE_BUTTONS;
    button[0].y_max = SPACE_BUTTONS + L_BUTTONS;

    button[1].button_off = hp_train_off;
    button[1].button_on = hp_train_on;
    button[1].state = false;
    button[1].x_min = button[0].x_max + 2*SPACE_BUTTONS;
    button[1].x_max = button[1].x_min + L_BUTTONS;
    button[1].y_min = SPACE_BUTTONS;
    button[1].y_max = SPACE_BUTTONS + L_BUTTONS;

    button[2].button_off = mp_train_off;
    button[2].button_on = mp_train_on;
    button[2].state = false;
    button[2].x_min = button[1].x_max + 2*SPACE_BUTTONS;
    button[2].x_max = button[2].x_min + L_BUTTONS;
    button[2].y_min = SPACE_BUTTONS;
    button[2].y_max = SPACE_BUTTONS + L_BUTTONS;

    button[3].button_off = lp_train_off;
    button[3].button_on = lp_train_on;    
    button[3].state = false;
    button[3].x_min = button[2].x_max + 2*SPACE_BUTTONS;
    button[3].x_max = button[3].x_min + L_BUTTONS;
    button[3].y_min = SPACE_BUTTONS;
    button[3].y_max = SPACE_BUTTONS + L_BUTTONS;

    button[4].button_off = close_program_off;
    button[4].button_on = close_program_on;
    button[4].state = false;
    button[4].x_min = INTERFACE_W - 2*SPACE_BUTTONS - L_BUTTONS;
    button[4].x_max = button[4].x_min + L_BUTTONS;
    button[4].y_min = SPACE_BUTTONS;
    button[4].y_max = SPACE_BUTTONS + L_BUTTONS;

    for (i = 0; i <= 4; i++) {
        blit(button[i].button_off, interface, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);
    }

    //binario 1
    trail1_xPoints[0] = W/2-7*space;
    trail1_xPoints[1] = W/2-4*space;
    trail1_xPoints[2] = W/2+4*space;
    trail1_xPoints[3] = W/2+7*space;

    //binario 2
    trail2_xPoints[0] = W/2-5*space;
    trail2_xPoints[1] = W/2-3*space;
    trail2_xPoints[2] = W/2+3*space; 
    trail2_xPoints[3] = W/2+5*space;

    //binario 3
    trail3_xPoints[0] = W/2-3*space; 
    trail3_xPoints[1] = W/2-2*space; 
    trail3_xPoints[2] = W/2+2*space; 
    trail3_xPoints[3] = W/2+3*space;

    //binario 4 è W così non gira mai il bitmap
    trail4_xPoints[0] = W; 
    trail4_xPoints[1] = W; 
    trail4_xPoints[2] = W; 
    trail4_xPoints[3] = W;

    //vettore dei binari
    trails_xPoints[0] = trail1_xPoints;
    trails_xPoints[1] = trail2_xPoints; 
    trails_xPoints[2] = trail3_xPoints; 
    trails_xPoints[3] = trail4_xPoints;
  
    //semaphores struct
    for (i = 0; i<=2; i++){
        semaphores[i].xPointStop    = W/2 + (2*i - 7)*space;
        semaphores[i].yPointStop    = H/2 - space/2;
        semaphores[i].trail_angle   = 0;
        semaphores[i].xPointIn   = semaphores[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i].xPointOut  = semaphores[i].xPointStop + (train_w + stop_space) * 2;
        
        semaphores[i+6].xPointStop  = W/2 + (2*i - 7)*space;
        semaphores[i+6].yPointStop  = H/2 + space/2;
        semaphores[i+6].trail_angle = 0;
        semaphores[i].xPointIn   = semaphores[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i].xPointOut  = semaphores[i].xPointStop + (train_w + stop_space) * 2;
    }

    for (i = 3; i<=5; i++){
        semaphores[i].xPointStop = W/2 + (2*i - 3)*space;
        semaphores[i].yPointStop = H/2 - space/2;
        semaphores[i].trail_angle = 128;
        semaphores[i].xPointIn   = semaphores[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i].xPointOut  = semaphores[i].xPointStop + (train_w + stop_space) * 2;
        
        semaphores[i+6].xPointStop = W/2 + (2*i - 3)*space;
        semaphores[i+6].yPointStop = H/2 + space/2;
        semaphores[i+6].trail_angle = 128;
        semaphores[i].xPointIn   = semaphores[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i].xPointOut  = semaphores[i].xPointStop + (train_w + stop_space) * 2;
    }

    for (i = 0; i<=11; i++){
        semaphores[i].queue = 0;
        semaphores[i].status = true;
        semaphores[i].sem = create_bitmap(sem_g->w, sem_g->h);
        blit(sem_g, semaphores[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
    }

    station_stop = W/2 + (train_w + stop_space) * 2;
    station_in   = station_stop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
    station_out  = station_stop + (train_w + stop_space) * 2;

    //stazion struct
    for (i = 0; i<STATIONS_NUM; i++){
        station[i].xPointStop = (W/2) + (train_w + stop_space);// * 2) - (train_w - stop_space);
        station[i].yPointStop = (i + 1) * space; 
        station[i].xPointIn   = station[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        station[i].xPointOut  = station[i].xPointStop + (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        station[i].status     = true;
        station[i].queue      = 0; 
        station[i].sem        = create_bitmap(sem_g->w, sem_g->h);
        blit(sem_g, station[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
        station[i].move_queue = false;
    }

    // Disegno i vari binari
    y1 = 4*space;
    y2 = 5*space;
    line(background, 0, 4*space, W, 4*space, red);
    line(background, 0, 5*space, W, 5*space, red);
    col = orange;

    for (i=0; i<3; i++) { 
        
        y0 = (i+1)*space;
        y3 = (8-i)*space;
        x0 = trails_xPoints[i][0];
        x1 = trails_xPoints[i][1];
        x2 = trails_xPoints[i][2];
        x3 = trails_xPoints[i][3];

        line(background, x1, y0, x2, y0, col);                  // linea dritta up
        line(background, x0 + 50, y1 - 50, x1, y0, col);        // salita up
        line(background, x2, y0, x3 - 50, y1 - 50, col);        // discesa up
        line(background, x1, y3, x2, y3, col);                  // linea dritta down
        line(background, x0 + 50, y2 + 50, x1, y3, col);        // salita down
        line(background, x2, y3, x3 - 50, y2 + 50, col);        // discesa down
        line(background, x0, y1, x0 + space, y1, grey);
        line(background, x0, y2, x0 + space, y2, grey);
        line(background, x3, y1, x3 - space, y1, grey);
        line(background, x3, y2, x3 - space, y2, grey);

        if (col == orange) col = green;
        else col = blue;
    }

    // creazione dei vari tasks
    task_create(graphics, graphicTaskId, 30, 30, 255);
    task_create(station_manager, stationsManagerTaskId, 50, 30, 255);
    task_create(user_task, userTaskId, 50, 50, 255);

    printf("Initialization completed!\n");
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE *user_task
// task utente
//-------------------------------------------------------------------------------------------------------------------------
void    *user_task(void *p) {
    char scan;
    int id;
    int i, j;
    int mbutton;
    int x;
    int y;
    int pressed_button;
    
    id = get_task_id(p);
    set_activation(id);
    i = 1;

    while(EXIT == false){
        
        // PARTE 1: legge i comandi del mouse 
        mbutton = mouse_b & 1;
        
        if (mbutton) {
            x = mouse_x;
            y = mouse_y - H;
            pressed_button = check_button(x, y);

            if (button[pressed_button].state == false) {
                switch (pressed_button) {

                case 0:
                    if (i == TMAX ) i = 1;
                    task_create(new_train, i, 20, 20, 255);
                    i++;
                    break;
                
                case 1:
                    break;
                    
                case 2:
                    break;
                
                case 3:
                    break;
                
                case 4:
                    EXIT_COMMAND = true;
                    break;
                
                default:
                    break;
                }
                button[pressed_button].state = true;
            }
        }
        else {
            for (j = 0; j<= N_BUTTONS; j++) button[j].state = false;
        }
        
        // PARTE 2: legge i comandi da tastiera
        scan = get_scancode();
        switch(scan) {

            case KEY_SPACE:
                if (i == TMAX ) i = 1;
                task_create(new_train, i, 20, 20, 255);
                i++;
                break;

            case KEY_ENTER:
                printf("zero \n");
                break;

            case KEY_ESC:
                EXIT_COMMAND = true;
                break;

            default:
                break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE *check button
// individua il pulsante premuto
//-------------------------------------------------------------------------------------------------------------------------
int check_button(int x, int y){
    int i;

    for (i = 0; i < N_BUTTONS; i++) {
        if (x < button[i].x_max && x > button[i].x_min &&
                y < button[i].y_max && y > button[i].y_min) break;   
    }
    return i;
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE *graphics
// task grafico
//-------------------------------------------------------------------------------------------------------------------------

void *graphics(void *p){

    int id;
    int sem_w;
    int sem_h;
    int i;
    int j;
    
    id = get_task_id(p);
    set_activation(id);

    sem_w = sem_r->w*sem_size_factor;
    sem_h = sem_r->h*sem_size_factor;
    
    while(EXIT == false) {

        blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
        blit(interface, interface_buffer, 0, 0, 0, 0, interface->w, interface->h);

        // disegno i semafori degli scambi sul buffer
        for (i=0; i<=11; i++){
            stretch_sprite(buffer, semaphores[i].sem, semaphores[i].xPointStop - sem_w/2, semaphores[i].yPointStop - sem_h, sem_w, sem_h);
        }
        
        // disegno i binari mobili sul buffer
        for (i = 0; i<=2; i++){
            pivot_sprite(buffer,trail, semaphores[i].xPointStop, semaphores[i].yPointStop - trail_h/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointStop, semaphores[i+6].yPointStop - trail_h/2, 0, 0, itofix(semaphores[i+6].trail_angle));
        }
        for (i = 3; i<=5; i++){
            pivot_sprite(buffer,trail, semaphores[i].xPointStop, semaphores[i].yPointStop + trail_h/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointStop, semaphores[i+6].yPointStop + trail_h/2, 0, 0, itofix(semaphores[i+6].trail_angle));
        }
        // disegno i semafori della stazione
        for (i=0; i<STATIONS_NUM; i++){
            stretch_sprite(buffer, sem_g, W/2, station[i].yPointStop - sem_h, sem_w, sem_h);
        }
        // disegna ogni treno attivo sul buffer, in base alla posizione associata al treno
        for (i = 1; i < TMAX; i++){
            if (train_par[i].run == true){
                // draw_sprite(buffer, train_par[i].bmp, train_par[i].posx, train_par[i].posy);
                for (int j = 0; j < train_par[i].wagonsNumber; j++){
                    draw_sprite(buffer, train_par[i].wagons[j].bmp,
                                        train_par[i].wagons[j].posx, 
                                        train_par[i].wagons[j].posy);
                }
            }
        }

        // disegno l'interfaccia
        for (j = 0; j<N_BUTTONS ; j++) {
            if (button[j].state == true) {
                blit(button[j].button_on, interface_buffer, 0, 0, button[j].x_min, button[j].y_min, L_BUTTONS, L_BUTTONS);
            }
        }

        blit(buffer, screen, 0, 0, 0, 0, background->w, background->h);
        blit(interface_buffer, screen, 0, 0, 0, H, interface->w, interface->h);
        show_mouse(screen);

        if (deadline_miss(id)) {
            printf("deadline miss of graphic task\n");
        }        
        wait_for_activation(id);
    }

    destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(sem_r);
    destroy_bitmap(sem_y);
    destroy_bitmap(sem_g);
    destroy_bitmap(trail);
    destroy_bitmap(platform);
        for (i = 0; i <= 3; i ++) {
        destroy_bitmap(train_bmp[i].train1);
        destroy_bitmap(train_bmp[i].train2);
        destroy_bitmap(train_bmp[i].train3);
    }
    printf("Closing graphics...\n");
    ptask_exit(id);
}
/*-------------------------------------------------------------------------*/
void *station_manager(void *p){
    int id;
    int assigned_trains;
    int i;
    int s;
    int bin;

    id = get_task_id(p);
    set_activation(id);

    while (EXIT == false){

        if(ready_trains_num > 0){
            assigned_trains=0;
            for (i = 0; i < ready_trains_num; i++){

                if(train_par[last_assigned_train_id + ready_trains_num - i].run == false) {
                    bin = 1 + rand()%4;
                    if (trains_in_binary[bin-1] < MAX_TRAINS_IN_QUEUE + 1){
                    train_par[last_assigned_train_id + ready_trains_num - i].binary = bin;
                    assigned_trains++;
                    }
                }
            }
            last_assigned_train_id += assigned_trains;
        }

        for (i = 1; i < TMAX; i++){

            if (train_par[i].run == true){
                if ((train_par[i].posx > station[train_par[i].binary-1].xPointIn) &&
                    (train_par[i].posx < station[train_par[i].binary-1].xPointStop)){

                    if (train_par[i].station_flag == false){
                        if (station[train_par[i].binary-1].status == true){
                            station[train_par[i].binary-1].status = false;
                        }
                        train_par[i].station_flag = true;
                        train_par[i].stopx = station[train_par[i].binary-1].xPointStop - (train_w + stop_space)*(station[train_par[i].binary-1].queue);
                        train_par[i].pos_in_queue = station[train_par[i].binary-1].queue;
                        station[train_par[i].binary-1].queue_list[station[train_par[i].binary-1].queue] = i;
                        station[train_par[i].binary-1].queue++;
                    }
                }
                if (train_par[i].binary != 4) {
                    if(train_par[i].posx >= trails_xPoints[train_par[i].binary -1][0] - train_w) {
                        semaphores[train_par[i].binary -1].trail_angle = TRAIL_UP_BIN_IN_SWITCH_ON_ANGLE;
                    }
                     if(train_par[i].posx >= trails_xPoints[train_par[i].binary -1][0] + 2*space) {
                        semaphores[train_par[i].binary -1].trail_angle = TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE;
                    }
                    
                    if(train_par[i].posx >= trails_xPoints[train_par[i].binary - 1][3] - 1.5*space) {
                        semaphores[6 - train_par[i].binary].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON_ANGLE;
                    }
                    if(train_par[i].posx >= trails_xPoints[train_par[i].binary - 1][3] + 2*space) {
                        semaphores[6 - train_par[i].binary].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE;
                    }
                }
            }
        }

        for (s = 0; s < STATIONS_NUM; s++) {
            if (station[s].queue >= 2) {
                station[s].move_queue = true;
                manage_queue(s);
                if (station[s].queue == 0) {
                    // station[s].status = true;
                }                
            }
        }

        wait_for_activation(id);
    }
}
//----------------------------------------------------------------------
// FUNZIONE new_train
//
// Crea un nuovo task treno e ne gestisce il movimento.
//----------------------------------------------------------------------
void *new_train(void *p) {
    int id;
    int binary_num;
    int j;
    int next_state, curr_state, prev_state, last_stop;
    float vel;
    float acc;
    float refVel;
    float accDistancePixel;
    float accDistanceMeters;
    float stopDistancePixel;
    float stopDistanceMeters;
    bool run;

    struct station_struct stationToCheck;

    id = get_task_id(p);
    set_activation(id);

    ready_trains_num++;
    set_train_parameters(id);    

    while(train_par[id].binary == 0){
        printf("Train number %d is waiting for binary\n", id);
        wait_for_activation(id);
    }

    binary_num = train_par[id].binary; // assigned by the station manager
    train_par[id].wagonsNumber = 3; // deve essere massimo MAX_WAGONS_PER_TRAIN

    if (train_par[id].binary == 4){
        train_par[id].wagonsNumber = 5;
    }
    
    for (j = 0; j < train_par[id].wagonsNumber; j++){
        train_par[id].wagons[j].posx = 0 - (j*(train_w*1.1));
        train_par[id].wagons[j].posy = H/2 - space/2 - train_h/2;
        train_par[id].wagons[j].bmp = train_bmp[binary_num-1].train1;
    }


    train_par[id].run = true;
    ready_trains_num--;
    going_trains_num++;
    trains_in_binary[train_par[id].binary-1]++;
    
    curr_state = GO_FAST;
    prev_state = curr_state;
    next_state = curr_state;
    last_stop  = train_par[id].stopx;

    stationToCheck = station[binary_num - 1];

    while((train_par[id].posx < W) && (EXIT == false) ){

        switch (curr_state){
            case(GO_FAST):
                if(curr_state != prev_state){
                    printf("GO FAST\n");
                }

                vel = train_par[id].currentVel;
                acc = 0;
                move(id, vel, acc);

                // check
                if ((train_par[id].posx > stationToCheck.xPointIn) &&
                    (train_par[id].posx < stationToCheck.xPointOut)){
                    next_state = GO_SLOW;
                }
                if (curr_state != next_state){
                }
                break;

            case(GO_SLOW):
                if(curr_state != prev_state){
                    printf("GO SLOW\n");            
                }
                // step
                vel = train_par[id].currentVel;
                
                if ((prev_state == STOP)||(train_par[id].posx >= stationToCheck.xPointStop)){
                    accDistancePixel = fabs(stationToCheck.xPointOut - stationToCheck.xPointStop);
                    accDistanceMeters = accDistancePixel*px2m;
                    refVel = train_par[id].maxVel;
                    acc = (refVel*refVel)/(2*accDistanceMeters);
                }
                else if ((prev_state == GO_FAST)||(train_par[id].posx < stationToCheck.xPointStop)){
                    stopDistancePixel = fabs(stationToCheck.xPointStop - stationToCheck.xPointIn)*1.5;
                    stopDistanceMeters = stopDistancePixel*px2m;
                    refVel = train_par[id].maxVel;
                    acc = -(refVel*refVel)/(2*stopDistanceMeters);
                }                
                
                move(id, vel, acc);

                // check
                if ((train_par[id].alreadyStopped == false)){
                    if (train_par[id].stopx != 0){
                        last_stop = train_par[id].stopx;
                        if ((train_par[id].posx >= train_par[id].stopx)) {
                            if (train_par[id].pos_in_queue == 0){
                                next_state = STOP;  
                            }
                            else{
                                next_state = WAIT;
                            }
                        }
                    }
                }
                if (train_par[id].posx > stationToCheck.xPointOut){
                    next_state = GO_FAST;
                }
                if (curr_state != next_state){
                }
                break;

            case(WAIT):
                if(curr_state != prev_state){
                    //on entry
                }
                // step
                //do not move
                // check
                if (train_par[id].stopx != last_stop){
                    next_state = GO_SLOW;  
                }
                if (curr_state != next_state){
                    // on leave();
                }
                break;

            case(STOP):
                if(curr_state != prev_state){
                    printf("STOP STATE \n");
                    train_par[id].currentVel = 0;
                }
                // step
                stopAtStation(id);
                // check
                if (train_par[id].ready_to_go_flag == true){
                    next_state = GO_SLOW;
                }
                if (curr_state != next_state){
                    // on leave();
                    train_par[id].alreadyStopped = true;
                }
                break;
        }

        prev_state = curr_state;
        curr_state = next_state;
         
        wait_for_activation(id);
    }
    trains_in_binary[train_par[id].binary-1]--;
    if(trains_in_binary[train_par[id].binary-1] < 0){
        trains_in_binary[train_par[id].binary-1] = 0;
    }
    train_par[id].run = false;
    ptask_exit(id);
    return 0;
}

//----------------------------------------------------------------------------
// FUNZIONE set_train_parameters
//
//---------------------------------------------------------------------------
void set_train_parameters(int i) {
    
    /* seme per la funzione random, scrivendo solo srand(time(NULL)) 
    ottengo lo stesso seme se i treni appaiono troppo velocemente */
    srand(time(NULL)+ i);             

    // variabili della struttura settate dalla funzione new train
    train_par[i].run              = false;                         
    train_par[i].posx             = 0;
    train_par[i].posy             = H/2 - space/2 - train_h/2;
    train_par[i].currentVel       = 15; //velocità di partenza
    train_par[i].maxVel           = 15; //velocità di partenza
    train_par[i].count            = 0;
    train_par[i].direction        = rand()%2; // direzione random da 0 a 1
    train_par[i].binary           = 0; 
    train_par[i].priority         = 1 + rand()%3;  // priorità random da 1 a 3 
    train_par[i].station_flag     = false;
    train_par[i].ready_to_go_flag = false; //indica che il treno è pronto a ripartire dopo la fermata
    train_par[i].stopx            = 0;
    train_par[i].pos_in_queue     = 0;
    train_par[i].alreadyStopped   = false;
}
/*-------------------------------------------------------------------------*/
void manage_queue(int stationId){
    int i;
    // se posso muovere la coda eseguo questo ciclo, altrimenti resto in attesa
    if (station[stationId].move_queue == true) {
        if (train_par[station[stationId].queue_list[0]].ready_to_go_flag == true){
            station[stationId].queue--;
            // definisco la nuova posizione di stop intermedia per tutti i treni in coda
            for (i=1; i<=station[stationId].queue; i++) {
                train_par[station[stationId].queue_list[i]].stopx += train_w + stop_space;
                train_par[station[stationId].queue_list[i]].pos_in_queue--;
                station[stationId].queue_list[i-1] = station[stationId].queue_list[i];
            }
        }
        station[stationId].move_queue = false; // attendo che il prossimo treno esca dalla stazione
    }
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move
// muove il treno in orizzontale
//-------------------------------------------------------------------------------------------------------------------------
// void move(int i, int step){
void move(int i, float vel, float acc){
    
    float newVel;
    float deltaSpace;
    float newStep;
    int step;
    int bin;
    int j;

    /** Lo step minimo deve essere di 2 pixel, altrimenti nelle funzioni
     *  move_diag_up/move_diag_down che ne calcolano il seno e il coseno,
     *  si rischia di avere un incremento nullo.
     */

    newVel = vel + acc * 0.020;

    if (newVel > train_par[i].maxVel) {
        newVel = train_par[i].maxVel;
    }
    if (newVel < 0) {
        newVel = 0;
    }

    train_par[i].currentVel = newVel;
    deltaSpace = newVel * 0.020;
    newStep = deltaSpace / px2m;
    step = (int)newStep;
    bin = train_par[i].binary; 
    
    for (j = 0; j < train_par[i].wagonsNumber; j++) {

        if ((train_par[i].wagons[j].posx > trails_xPoints[bin - 1][0] - train_w) && 
                (train_par[i].wagons[j].posx <= trails_xPoints[bin - 1][1] - train_w)){
            move_diag_up(i, j, bin, step);
        }
        else if ((train_par[i].wagons[j].posx > trails_xPoints[bin -1][2] - train_h) && 
                (train_par[i].wagons[j].posx <= trails_xPoints[bin -1][3] - train_h)){
            move_diag_down(i, j, bin, step);
        }
        else{
            
            // i due if qua sotto servono a riallineare i vagoni dopo essersi mossi in diagonale visto che si disallineavano
            if (train_par[i].wagons[j].posx > trails_xPoints[bin - 1][3] - train_h) {
                train_par[i].wagons[j].posy = (H - space - train_h)/2;
            }
            else if ((train_par[i].wagons[j].posx <= trails_xPoints[bin - 1][2] - train_h) && 
                    (train_par[i].wagons[j].posx >trails_xPoints[bin - 1][1] - train_w)) {
                train_par[i].wagons[j].posy = bin*space - train_h/2;
            }  
            move_forward(i, j, bin, step);
        }
    }
}
/*-------------------------------------------------------------------------*/
void move_diag_up(int i, int j ,int bin, int inc){
    if (j == 0){
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += (int)(((float)inc)*cosf(M_PI_4));
        train_par[i].posy -= (int)(((float)inc)*sinf(M_PI_4));    
    }
    train_par[i].wagons[j].bmp = train_bmp[bin-1].train2;
    train_par[i].wagons[j].posx += (int)(((float)inc)*cosf(M_PI_4));
    train_par[i].wagons[j].posy -= (int)(((float)inc)*sinf(M_PI_4));
}
/*-------------------------------------------------------------------------*/
void move_diag_down(int i, int j, int bin, int inc){
    if (j == 0){
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += (int)(((float)inc)*cosf(M_PI_4));
        train_par[i].posy += (int)(((float)inc)*sinf(M_PI_4));    
    }
    train_par[i].wagons[j].bmp = train_bmp[bin-1].train3;
    train_par[i].wagons[j].posx += (int)(((float)inc)*cosf(M_PI_4));
    train_par[i].wagons[j].posy += (int)(((float)inc)*sinf(M_PI_4));
}
/*-------------------------------------------------------------------------*/
void move_forward(int i, int j, int bin, int inc) {
    if (j == 0) {
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += inc;    
    }
    train_par[i].wagons[j].bmp = train_bmp[bin-1].train1;
    train_par[i].wagons[j].posx += inc;
}
/*-------------------------------------------------------------------------*/
void stopAtStation(int i){ 

    if (train_par[i].ready_to_go_flag == false) {
        if (train_par[i].count == 100) {
            train_par[i].ready_to_go_flag = true;
            printf("Train %d is ready to go \n", i);
        }
        else {
            train_par[i].count += 1;
        }
    }
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
// FUNZIONE get_scancode
//
// funzione che da come risultato il tasto premuto, se ne è stato 
// premuto uno, altrimenti da come risultato 0
//---------------------------------------------------------------------------
char get_scancode() {
    
    if(keypressed()) {
        return readkey() >> 8;
    }
    else{
        return 0;
    }    
}