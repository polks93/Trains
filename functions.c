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
        semaphores[i].xPointDraw = semaphores[i].xPointStop;
        semaphores[i].yPointDraw = semaphores[i].yPointStop;
        semaphores[i].trail_angle = TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE;
        semaphores[i].xPointIn   = semaphores[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i].xPointOut  = semaphores[i].xPointStop + (train_w + stop_space) * 2;
        
        semaphores[i+6].xPointStop  = W/2 + (2*i - 7)*space;
        semaphores[i+6].yPointStop  = H/2 + space/2;
        semaphores[i+6].xPointDraw = semaphores[i+6].xPointStop;
        semaphores[i+6].yPointDraw = semaphores[i+6].yPointStop;
        semaphores[i+6].trail_angle = TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE;
        semaphores[i+6].xPointIn   = semaphores[i+6].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i+6].xPointOut  = semaphores[i+6].xPointStop + (train_w + stop_space) * 2;
    }

    for (i = 3; i<=5; i++){
        semaphores[i].xPointStop = W/2 +(i-1)*space;
        semaphores[i].yPointStop = (6-i)*space;
        semaphores[i].xPointDraw = W/2 + (2*i - 3)*space;
        semaphores[i].yPointDraw = H/2 - space/2;
        semaphores[i].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE;
        semaphores[i].xPointIn   = semaphores[i].xPointStop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
        semaphores[i].xPointOut  = semaphores[i].xPointStop + (train_w + stop_space) * 2;
        
        semaphores[i+6].xPointStop = W/2 +(i-1)*space;
        semaphores[i+6].yPointStop = (3+i)*space;
        semaphores[i+6].xPointDraw = W/2 + (2*i - 3)*space;
        semaphores[i+6].yPointDraw = H/2 + space/2;
        semaphores[i+6].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE;
        semaphores[i+6].xPointIn   = semaphores[i+6].xPointStop - (train_w + stop_space) * 2;//MAX_TRAINS_IN_QUEUE;
        semaphores[i+6].xPointOut  = semaphores[i+6].xPointStop + (train_w + stop_space) * 2;
    }

    for (i = 0; i<=11; i++){

        semaphores[i].queue = 0;
        semaphores[i].status = true;
        semaphores[i].sem = create_bitmap(sem_g->w, sem_g->h);
        semaphores[i].trail_state = TRAIL_OFF;
        semaphores[i].trail_angle_cnt = 0;
        semaphores[i].trail_angle_inc = 0.01;
        blit(sem_g, semaphores[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
    }

    station_stop = W/2 + (train_w + stop_space) * 2;
    station_in   = station_stop - (train_w + stop_space) * MAX_TRAINS_IN_QUEUE;
    station_out  = station_stop + (train_w + stop_space) * 2;

    //stazion struct
    for (i = 0; i<STATIONS_NUM; i++){
        station[i].xPointStop = (W/2) + (train_w + stop_space);// * 2) - (train_w - stop_space);
        station[i].yPointStop = (i + 1) * space;
        station[i].xPointDraw = station[i].xPointStop;
        station[i].yPointDraw = station[i].yPointStop;
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

    for (i=0; i<3; i++) { 
        
        y0 = (i+1)*space;
        y3 = (8-i)*space;
        x0 = trails_xPoints[i][0];
        x1 = trails_xPoints[i][1];
        x2 = trails_xPoints[i][2];
        x3 = trails_xPoints[i][3];

        line(background, x1, y0, x2, y0, red);                  // linea dritta up
        line(background, x0 + 50, y1 - 50, x1, y0, red);        // salita up
        line(background, x2, y0, x3 - 50, y1 - 50, red);        // discesa up
        line(background, x1, y3, x2, y3, red);                  // linea dritta down
        line(background, x0 + 50, y2 + 50, x1, y3, red);        // salita down
        line(background, x2, y3, x3 - 50, y2 + 50, red);        // discesa down
        line(background, x0, y1, x0 + space, y1, grey);
        line(background, x0, y2, x0 + space, y2, grey);
        line(background, x3, y1, x3 - space, y1, grey);
        line(background, x3, y2, x3 - space, y2, grey);
    }

    // creazione dei vari tasks
    task_create(graphics, graphicTaskId, 50, 50, 255);                  // 20 Hz
    task_create(station_manager, stationsManagerTaskId, 25, 25, 255);   // 40 Hz
    task_create(user_task, userTaskId, 100, 100, 255);                  // 10 Hz

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
                    task_create(new_train, i, 25, 25, 255); //40 Hz
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
                task_create(new_train, i, 25, 25, 255); //40 Hz
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
    int direction;
    int sem_w;
    int sem_h;
    int i;
    int j;
    char str[50];

    id = get_task_id(p);
    set_activation(id);

    direction = 0;
    sem_w = sem_r->w*sem_size_factor;
    sem_h = sem_r->h*sem_size_factor;
    
    while(EXIT == false) {

        blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
        blit(interface, interface_buffer, 0, 0, 0, 0, interface->w, interface->h);

        // disegno i semafori degli scambi sul buffer
        for (i=0; i<=11; i++){
            if (semaphores[i].status == false){
                semaphores[i].sem = sem_r;
            }
            else{
                semaphores[i].sem = sem_g;
            }
            stretch_sprite(buffer, semaphores[i].sem, semaphores[i].xPointStop - sem_w/2, semaphores[i].yPointStop - sem_h, sem_w, sem_h);
        }
        
        // disegno i binari mobili sul buffer
        for (i = 0; i<=2; i++){
            pivot_sprite(buffer,trail, semaphores[i].xPointDraw, semaphores[i].yPointDraw - trail_h/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointDraw, semaphores[i+6].yPointDraw - trail_h/2, 0, 0, itofix(semaphores[i+6].trail_angle));
        }
        for (i = 3; i<=5; i++){
            pivot_sprite(buffer,trail, semaphores[i].xPointDraw, semaphores[i].yPointDraw + trail_h/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointDraw, semaphores[i+6].yPointDraw + trail_h/2, 0, 0, itofix(semaphores[i+6].trail_angle));
        }
        // disegno i semafori della stazione
        for (i=0; i<STATIONS_NUM; i++){
            if (station[i].status == false){
                station[i].sem = sem_r;
            }
            else{
                station[i].sem = sem_g;
            }
            stretch_sprite(buffer, station[i].sem, station[i].xPointDraw - sem_w/2, station[i].yPointDraw - sem_h, sem_w, sem_h);
        }

        // disegna ogni treno attivo sul buffer, in base alla posizione associata al treno
        for (i = 1; i < TMAX; i++){
            if (train_par[i].run == true){
                for (int j = 0; j < WAGONS; j++){
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

        // DEBUG GRAFICO
        sprintf(str, "Queue in binary 4 is: %d", trains_in_binary[4]);
        textout_ex(interface_buffer, font, str,400, 30,(0,0,0),-1);
        sprintf(str, "Queue in binary 3 is: %d", trains_in_binary[3]);
        textout_ex(interface_buffer, font, str,400, 40,(0,0,0),-1);
        sprintf(str, "Queue in binary 2 is: %d", trains_in_binary[2]);
        textout_ex(interface_buffer, font, str,400, 50,(0,0,0),-1);
        sprintf(str, "Queue in binary 1 is: %d", trains_in_binary[1]);
        textout_ex(interface_buffer, font, str,400, 60,(0,0,0),-1);

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
        
        // ASSEGNAZIONE BINARI
        if(ready_trains_num > 0){
            assigned_trains = 0;
            for (i = 0; i < ready_trains_num; i++){

                if(train_par[last_assigned_train_id + ready_trains_num - i].run == false) {
                    switch (train_par[last_assigned_train_id + ready_trains_num - i].priority) {
                    case 3:
                        if (trains_in_binary[4] <= trains_in_binary[3]) bin = 4;
                        else if (trains_in_binary[3] <= trains_in_binary[2]) bin = 3;
                        else if (trains_in_binary[2] <= trains_in_binary[1]) bin = 2;
                        else bin = 1;
                        break;
                    
                    case 2:
                        if (trains_in_binary[3] <= trains_in_binary[2]) bin = 3;
                        else if (trains_in_binary[2] <= trains_in_binary[1]) bin = 2;
                        else bin = 1;
                        break;
                    
                    case 1:
                        if (trains_in_binary[2] <= trains_in_binary[1]) bin = 2;
                        else bin = 1;
                        break;

                    default:
                        break;
                    }
                    train_par[last_assigned_train_id + ready_trains_num - i].binary = bin;
                    train_par[last_assigned_train_id + ready_trains_num - i].binary_occupied = true;
                    ready_trains_num--;
                    assigned_trains++;                    
                }
            }
            last_assigned_train_id += assigned_trains;
        }

        // GESTIONE SEMAFORI
        for (i = 1; i < TMAX; i++){

            if (train_par[i].run == true){
               switch(train_par[i].binary){
                case(1):
                    // check and modify sem status
                    checkSemaphoreIn(i, 0, TRAIL_ON);
                    checkSemaphoreOut(i, 5, TRAIL_ON);
                    
                    // check train into station
                    checkStation(i);

                    break;
                case(2):
                    // check and modify sem status
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_ON);
                    checkSemaphoreOut(i, 4, TRAIL_ON);
                    checkSemaphoreOut(i, 5, TRAIL_OFF);
                    // check train into station
                    checkStation(i);
                    break;
                case(3):
                    // check and modify sem status
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_OFF);
                    checkSemaphoreIn(i, 2, TRAIL_ON);
                    checkSemaphoreOut(i, 3, TRAIL_ON);
                    checkSemaphoreOut(i, 4, TRAIL_OFF);
                    checkSemaphoreOut(i, 5, TRAIL_OFF);
                    // check train into station
                    checkStation(i);
                    break;
                case(4):
                    // check and modify sem status
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_OFF);
                    checkSemaphoreIn(i, 2, TRAIL_OFF);
                    checkSemaphoreOut(i, 3, TRAIL_OFF);
                    checkSemaphoreOut(i, 4, TRAIL_OFF);
                    checkSemaphoreOut(i, 5, TRAIL_OFF);
                    // check train into station
                    checkStation(i);
                    break;
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
    int posx;
    int previous_train_posx;
    int j;
    int next_state, curr_state, prev_state, last_stop;
    int periodMs;
    float periodS;
    float vTarget;
    float vel;
    float acc;
    float refVel;
    float accDistancePixel;
    float accDistanceMeters;
    float stopDistancePixel;
    float stopDistanceMeters;
    bool run;

    id = get_task_id(p);
    set_activation(id);

    periodMs = get_task_per(p);
    periodS = (float)periodMs / 1000.0; 

    ready_trains_num++;
    set_train_parameters(id);   

    pthread_mutex_lock(&train_par[id].mutex);
    binary_num = train_par[id].binary;
    pthread_mutex_unlock(&train_par[id].mutex);

    while(binary_num == 0){
        printf("Train number %d is waiting for binary\n", id);

        pthread_mutex_lock(&train_par[id].mutex);
        binary_num = train_par[id].binary;
        pthread_mutex_unlock(&train_par[id].mutex);

        wait_for_activation(id);
    }
    
    // Check per evitare le collisioni in ingresso alla stazione
    if(id != 1) {

        pthread_mutex_lock(&train_par[id-1].mutex);
        previous_train_posx = train_par[id-1].posx;
        pthread_mutex_unlock(&train_par[id-1].mutex);

        while (previous_train_posx < 2*train_w*WAGONS) {
            printf("Waiting for previous train to go away \n");

            pthread_mutex_lock(&train_par[id-1].mutex);
            previous_train_posx = train_par[id-1].posx;
            pthread_mutex_unlock(&train_par[id-1].mutex);

            wait_for_activation(id);
        }
    }

    pthread_mutex_lock(&train_par[id].mutex);
    for (j = 0; j < WAGONS; j++){
        train_par[id].wagons[j].posx = 0 - (j*(train_w*1.1));
        train_par[id].wagons[j].posy = H/2 - space/2 - train_h/2;
        train_par[id].wagons[j].bmp = train_bmp[1].train1;
    }
    train_par[id].run = true;
    posx = train_par[id].posx;
    pthread_mutex_unlock(&train_par[id].mutex);

    going_trains_num++;
    trains_in_binary[binary_num]++;
    
    curr_state = GO_FAST;
    prev_state = curr_state;
    next_state = curr_state;
    last_stop  = 0;

    pthread_mutex_lock(&train_par[id].mutex);
    train_par[id].stopToCheck =  &station[binary_num - 1];
    pthread_mutex_unlock(&train_par[id].mutex);

    while((posx < W) && (EXIT == false) ){

        pthread_mutex_lock(&train_par[id].mutex);
        posx = train_par[id].posx;
        pthread_mutex_unlock(&train_par[id].mutex);

        switch (curr_state){
            case(GO_FAST):
                if(curr_state != prev_state){
                    pthread_mutex_lock(&train_par[id].mutex);
                    train_par[id].alreadyStopped = false;
                    train_par[id].ready_to_go_flag = false;
                    train_par[id].count = 0;
                    pthread_mutex_unlock(&train_par[id].mutex);
                }

                pthread_mutex_lock(&train_par[id].mutex);
                vel = train_par[id].currentVel;
                pthread_mutex_unlock(&train_par[id].mutex);

                acc = 0;

                move(id, vel, acc);

                // check
                if ((posx > train_par[id].stopToCheck->xPointIn)  &&
                    (posx < train_par[id].stopToCheck->xPointOut)){
                    next_state = SLOW_DOWN;
                    printf("GO SLOW \n");
                }

                break;

            case(SLOW_DOWN):
                printf("GOING SLOW \n");
                // step
                vel = train_par[id].currentVel;
                refVel = train_par[id].maxVel;
                
                // LASCIO LA STAZIONE
                if (train_par[id].alreadyStopped == true){
                    accDistancePixel = fabs(train_par[id].stopToCheck->xPointOut - train_par[id].stopToCheck->xPointStop);
                    printf("acc: %f \n", accDistancePixel);
                    accDistanceMeters = accDistancePixel*px2m;
                    vTarget = refVel; //m/s
                    acc = ((vTarget*vTarget) - (vel*vel)) / (2*stopDistanceMeters);
                }
                // ARRIVO ALLA STAZIONE
                else{
                    stopDistancePixel = fabs(train_par[id].stopToCheck->xPointStop - train_par[id].stopToCheck->xPointIn)*0.6;
                    printf("stop distance: %f \n", stopDistancePixel);
                    stopDistanceMeters = stopDistancePixel*px2m;
                    
                    // Se il semaforo è rosso, decelera
                    if (train_par[id].stopToCheck->status == false){
                        acc = -(refVel*refVel)/(2*stopDistanceMeters);
                    }
                    else{
                        vTarget = 5; //m/s
                        acc = ((vTarget*vTarget) - (vel*vel)) / (2*stopDistanceMeters);
                    }
                }                
                
                move(id, vel, acc);

                // check
                if ((train_par[id].alreadyStopped == false)){
                    if (train_par[id].currentVel < 1.0) {
                        if(train_par[id].stopToCheck->status == false){
                            next_state = STOP;  
                        }
                    }
                    else{
                        if (train_par[id].posx >= train_par[id].stopToCheck->xPointStop){
                            train_par[id].alreadyStopped = true;    
                        }
                    }
                }

                if (train_par[id].posx > train_par[id].stopToCheck->xPointOut){
                    next_state = GO_FAST;
                }

                if (curr_state != next_state){
                }
                break;

            case(SPEED_UP):
                
                vel = train_par[id].currentVel;
                refVel = train_par[id].maxVel;

                accDistancePixel = fabs(train_par[id].stopToCheck->xPointOut - train_par[id].stopToCheck->xPointStop);
                printf("acc: %f \n", accDistancePixel);
                accDistanceMeters = accDistancePixel*px2m;
                vTarget = refVel; //m/s
                acc = ((vTarget*vTarget) - (vel*vel)) / (2*stopDistanceMeters);

                break;
            case(STOP):
                if(curr_state != prev_state){
                    train_par[id].currentVel = 0;
                }
                // step
                stopAtStation(id);
                // check
                if (train_par[id].ready_to_go_flag == true){
                    next_state = SLOW_DOWN;
                }
                if (curr_state != next_state){
                    // on leave();
                    train_par[id].alreadyStopped = true;
                }
                break;
        }
        if (train_par[id].posx > W/2 && train_par[id].binary_occupied == true) {
            trains_in_binary[train_par[id].binary]--;
            train_par[id].binary_occupied = false;

        }
        prev_state = curr_state;
        curr_state = next_state;
         
        wait_for_activation(id);
    }

    train_par[id].run = false;
    pthread_mutex_destroy(&train_par[id].mutex);
    ptask_exit(id);
    return 0;
}

//----------------------------------------------------------------------------
// FUNZIONE set_train_parameters
//
//---------------------------------------------------------------------------
void set_train_parameters(int i) {
    pthread_mutex_t     train_mux = PTHREAD_MUTEX_INITIALIZER;
    int random_num;

    /* seme per la funzione random */
    srand(time(NULL)+ i);             
    random_num = rand();

    // variabili della struttura settate dalla funzione new train
    train_par[i].run                = false;                         
    train_par[i].posx               = 0;
    train_par[i].posy               = H/2 - space/2 - train_h/2;
    train_par[i].currentVel         = 15; //velocità di partenza
    train_par[i].maxVel             = 15; //velocità massima 
    train_par[i].count              = 0;
    train_par[i].direction          = (random_num)%2; // direzione random da 0 a 1
    train_par[i].binary             = 0; 
    train_par[i].priority           = 3; //+ (random_num)%3;  // priorità random da 1 a 3 
    train_par[i].station_flag       = false;
    train_par[i].ready_to_go_flag   = false; //indica che il treno è pronto a ripartire dopo la fermata
    train_par[i].stopx              = 0;
    train_par[i].pos_in_queue       = 0;
    train_par[i].alreadyStopped     = false;
    train_par[i].binary_occupied    = false;
    train_par[i].mutex              = train_mux;
}
/*-------------------------------------------------------------------------*/
//  QUESTA FUNZIONE NON FUNZIONA

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
    int priority;
    int bin;
    int j;
    int periodMs;
    float periodS;

    periodMs = get_task_per(&tp[i]);
    periodS = (float)periodMs / 1000.0; 
    newVel = vel + acc * periodS;

    if (newVel > train_par[i].maxVel)   newVel = train_par[i].maxVel;
    
    if (newVel < 0)     newVel = 0;
    
    train_par[i].currentVel = newVel;
    deltaSpace = newVel * periodS;
    newStep = deltaSpace / px2m;
    step = (int)newStep;
    priority = train_par[i].priority; 
    bin = train_par[i].binary;
    
    for (j = 0; j < WAGONS; j++) {

        if ((train_par[i].wagons[j].posx > trails_xPoints[bin - 1][0] - train_w) && 
                (train_par[i].wagons[j].posx <= trails_xPoints[bin - 1][1] - train_w)){
            move_diag_up(i, j, priority, step);
        }
        else if ((train_par[i].wagons[j].posx > trails_xPoints[bin -1][2] - train_h) && 
                (train_par[i].wagons[j].posx <= trails_xPoints[bin -1][3] - train_h)){
            move_diag_down(i, j, priority, step);
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
            move_forward(i, j, priority, step);
        }
    }
}
/*-------------------------------------------------------------------------*/
void move_diag_up(int i, int j ,int priority, int inc){

    inc = (int) inc*cosf(M_PI_4);

    if (j == 0){
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        // train_par[i].posx += (int)(((float)inc)*cosf(M_PI_4));
        // train_par[i].posy -= (int)(((float)inc)*sinf(M_PI_4));    
        train_par[i].posx += inc;
        train_par[i].posy -= inc;
    }
    train_par[i].wagons[j].bmp = train_bmp[priority].train2;
    // train_par[i].wagons[j].posx += (int)(((float)inc)*cosf(M_PI_4));
    // train_par[i].wagons[j].posy -= (int)(((float)inc)*sinf(M_PI_4));
    train_par[i].wagons[j].posx += inc;
    train_par[i].wagons[j].posy -= inc;
}
/*-------------------------------------------------------------------------*/
void move_diag_down(int i, int j, int priority, int inc){
    
    if (j == 0){
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += (int)(((float)inc)*cosf(M_PI_4));
        train_par[i].posy += (int)(((float)inc)*sinf(M_PI_4));    
    }
    train_par[i].wagons[j].bmp = train_bmp[priority].train3;
    train_par[i].wagons[j].posx += (int)(((float)inc)*cosf(M_PI_4));
    train_par[i].wagons[j].posy += (int)(((float)inc)*sinf(M_PI_4));
}
/*-------------------------------------------------------------------------*/
void move_forward(int i, int j, int priority, int inc) {
    if (j == 0) {
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += inc;    
    }
    train_par[i].wagons[j].bmp = train_bmp[priority].train1;
    train_par[i].wagons[j].posx = train_par[i].posx - (j*(train_w*1.1));

}
/*-------------------------------------------------------------------------*/
void stopAtStation(int i){ 

    if (train_par[i].stopToCheck->status == true){
        train_par[i].ready_to_go_flag = true;
    }
}
/*-------------------------------------------------------------------------*/
void checkSemaphoreIn(int trainId, int semId, int semStateRequired) {
    
    if ((semaphores[semId].trail_state != semStateRequired) && 
        (train_par[trainId].posx <= semaphores[semId].xPointOut) &&
        (train_par[trainId].posx >= semaphores[semId].xPointIn)){
            semaphores[semId].status = false;
            train_par[trainId].stopToCheck = &semaphores[semId];

        if (semStateRequired == TRAIL_ON) {
            semaphores[semId].trail_angle -= 1;
            if (semaphores[semId].trail_angle == TRAIL_UP_BIN_IN_SWITCH_ON_ANGLE){
                semaphores[semId].trail_state = semStateRequired;
                semaphores[semId].status = true;
                semaphores[semId].trail_angle_cnt = 0;
            }
        }
        if (semStateRequired == TRAIL_OFF){
            semaphores[semId].trail_angle += 1;
            // printf("[TO OFF]semaphores[semId].trail_angle_cnt %.3f\n", semaphores[semId].trail_angle_cnt);
            // printf("[TO OFF]semaphores[semId].trail_angle %d\n", semaphores[semId].trail_angle);
            if (semaphores[semId].trail_angle == TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE){
                semaphores[semId].trail_state = semStateRequired;
                semaphores[semId].status = true;
                semaphores[semId].trail_angle_cnt = 0;
            }
        }
    }
}
/*-------------------------------------------------------------------------*/
void checkSemaphoreOut(int trainId, int semId, int semStateRequired) {
    
    if ((semaphores[semId].trail_state != semStateRequired) && 
        (train_par[trainId].posx <= semaphores[semId].xPointOut) &&
        (train_par[trainId].posx >= semaphores[semId].xPointIn)){
            semaphores[semId].status = false;
            train_par[trainId].stopToCheck = &semaphores[semId];

        if (semStateRequired == TRAIL_ON) {
            semaphores[semId].trail_angle +=1;
            if (semaphores[semId].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_ON_ANGLE){
                semaphores[semId].trail_state = semStateRequired;
                semaphores[semId].status = true;
                semaphores[semId].trail_angle_cnt = 0;
            }
        }
        if (semStateRequired == TRAIL_OFF){
            semaphores[semId].trail_angle -= 1;
            if (semaphores[semId].trail_angle == TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE){
                semaphores[semId].trail_state = semStateRequired;
                semaphores[semId].status = true;
                semaphores[semId].trail_angle_cnt = 0;
            }
        }
    }
}
/*-------------------------------------------------------------------------*/
void checkStation(int trainId) {

    if ((train_par[trainId].posx > station[train_par[trainId].binary-1].xPointIn) &&
        (train_par[trainId].posx < station[train_par[trainId].binary-1].xPointOut)){
        if (train_par[trainId].station_flag == false){
            train_par[trainId].station_flag = true;
            train_par[trainId].stopToCheck = &station[train_par[trainId].binary-1];
            station[train_par[trainId].binary-1].status = false;
        }
        // temporanea logica di uscita dalla stazione
        if (train_par[trainId].count == 100) {
            train_par[trainId].count = 0;
            station[train_par[trainId].binary-1].status = true;
        }
        else {
            train_par[trainId].count += 1;
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