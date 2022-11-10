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
    
    int i;
    int x0, x1, x2, x3, y0, y1, y2, y3;
    int col, black, red, blue, green, orange, white, grey;
    int trail_diag;

    printf("Loading ... \n");
    // INIT MUTEX
    pthread_mutex_t     ready_trains_num_mutex          = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t     last_assigned_train_id_mutex    = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t     trains_in_binary_mutex          = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t     station_mutex                   = PTHREAD_MUTEX_INITIALIZER;

    // INIZIALIZZAZIONE VARIABILI GLOBALI
    EXIT                    = false;
    EXIT_COMMAND            = false;
    ready_trains_num        = 0;
    last_assigned_train_id  = 0;
    for (i = 0; i < STATIONS_NUM; i++)      trains_in_binary[i] = 0;
    for (i = 0; i < STATIONS_NUM; i++)      station[i].mutex    = station_mutex;
    for (i = 0; i < SEMAPHORES_NUM; i++)    semaphores[i].mutex = station_mutex;

    // INIZIALIZZAZIONE ALLEGRO
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(15);
    black   = makecol(0, 0, 0);
    red     = makecol(0, 0, 255);
    blue    = makecol(165, 165, 0);
    green   = makecol(0, 255, 0);
    orange  = makecol(0, 160, 255);
    white   = makecol(255, 255,255);
    grey    = makecol(50, 50, 50);

    // BITMAPS DEI BINARI
    sem_g = load_bitmap("img/sem/sem_green.bmp", NULL);
    sem_r = load_bitmap("img/sem/sem_red.bmp", NULL);

    // BITMAPS DEI TRENI
    // orange
    train_bmp[0].train1 = load_bitmap("img/trains/orange.bmp", NULL);
    train_bmp[0].train2 = load_bitmap("img/trains/orange_diag.bmp", NULL);
    train_bmp[0].train3 = load_bitmap("img/trains/orange_diag2.bmp", NULL);

    // green
    train_bmp[1].train1 = load_bitmap("img/trains/green.bmp", NULL);
    train_bmp[1].train2 = load_bitmap("img/trains/green_diag.bmp", NULL);
    train_bmp[1].train3 = load_bitmap("img/trains/green_diag2.bmp", NULL);    
    
    // blue
    train_bmp[2].train1 = load_bitmap("img/trains/blue.bmp", NULL);
    train_bmp[2].train2 = load_bitmap("img/trains/blue_diag.bmp", NULL);
    train_bmp[2].train3 = load_bitmap("img/trains/blue_diag2.bmp", NULL);

    // red
    train_bmp[3].train1 = load_bitmap("img/trains/red.bmp", NULL);
    train_bmp[3].train2 = load_bitmap("img/trains/red_diag.bmp", NULL);
    train_bmp[3].train3 = load_bitmap("img/trains/red_diag2.bmp", NULL);

    // BITMAPS DEI BINARI
    trail = load_bitmap("img/trail.bmp", NULL);

    // BITMAPS DEI PULSANTI
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
    
    // INIZIALIZZAZIONE GRAFICA
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, WINDOW_H, 0, 0);
    background = create_bitmap(W, H);
    buffer = create_bitmap(W, H);
    interface = create_bitmap(INTERFACE_W, INTERFACE_H);
    interface_buffer = create_bitmap(INTERFACE_W, INTERFACE_H);
    clear_to_color(background, grey);
    clear_to_color(interface, grey);
    rectfill(background, 0, H-1, W-1, H-10, black);

    // STRUTTURA PULSANTI
    // random train
    button[0].button_off = random_train_off;
    button[0].button_on = random_train_on;
    button[0].state = false;
    button[0].x_min = 2*SPACE_BUTTONS;
    button[0].x_max = 2*SPACE_BUTTONS + L_BUTTONS;
    button[0].y_min = SPACE_BUTTONS;
    button[0].y_max = SPACE_BUTTONS + L_BUTTONS;

    // hp train
    button[1].button_off = hp_train_off;
    button[1].button_on = hp_train_on;
    button[1].state = false;
    button[1].x_min = button[0].x_max + 2*SPACE_BUTTONS;
    button[1].x_max = button[1].x_min + L_BUTTONS;
    button[1].y_min = SPACE_BUTTONS;
    button[1].y_max = SPACE_BUTTONS + L_BUTTONS;

    // mp train
    button[2].button_off = mp_train_off;
    button[2].button_on = mp_train_on;
    button[2].state = false;
    button[2].x_min = button[1].x_max + 2*SPACE_BUTTONS;
    button[2].x_max = button[2].x_min + L_BUTTONS;
    button[2].y_min = SPACE_BUTTONS;
    button[2].y_max = SPACE_BUTTONS + L_BUTTONS;

    // lp train
    button[3].button_off = lp_train_off;
    button[3].button_on = lp_train_on;    
    button[3].state = false;
    button[3].x_min = button[2].x_max + 2*SPACE_BUTTONS;
    button[3].x_max = button[3].x_min + L_BUTTONS;
    button[3].y_min = SPACE_BUTTONS;
    button[3].y_max = SPACE_BUTTONS + L_BUTTONS;

    // close program
    button[4].button_off = close_program_off;
    button[4].button_on = close_program_on;
    button[4].state = false;
    button[4].x_min = INTERFACE_W - 2*SPACE_BUTTONS - L_BUTTONS;
    button[4].x_max = button[4].x_min + L_BUTTONS;
    button[4].y_min = SPACE_BUTTONS;
    button[4].y_max = SPACE_BUTTONS + L_BUTTONS;

    // Disegno i pulsanti sul buffer
    for (i = 0; i <= 4; i++)        blit(button[i].button_off, interface, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);

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
    
    // STRUTTURA SEMAFORI
    for (i = 0; i<=2; i++){
        semaphores[i].xPointStop    = W/2 + (2*i - 7)*SPACE - (TRAIN_W + 5);
        semaphores[i].xPointDraw    = semaphores[i].xPointStop + (TRAIN_W + 5);
        semaphores[i].yPointDraw    = H/2 - SPACE/2;
        semaphores[i].xPointTrail   = semaphores[i].xPointDraw;
        semaphores[i].yPointTrail   = semaphores[i].yPointDraw;
        semaphores[i].trail_angle   = TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE;
        semaphores[i].xPointIn      = semaphores[i].xPointStop - SLOW_DOWN_SPACE;
        semaphores[i].xPointOut     = semaphores[i].xPointDraw + TRAIL_W;
        
        semaphores[i+6].xPointStop  = W/2 + (i - 4)*SPACE + (TRAIN_W + 5);
        semaphores[i+6].xPointDraw  = W/2 - (i+2)*SPACE;
        semaphores[i+6].yPointDraw  = (6+i)*SPACE;
        semaphores[i+6].xPointTrail = W/2 + (2*i - 7)*SPACE;
        semaphores[i+6].yPointTrail = H/2 + SPACE/2;
        semaphores[i+6].trail_angle = TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE;
        semaphores[i+6].xPointIn    = semaphores[i+6].xPointStop + SLOW_DOWN_SPACE;
        semaphores[i+6].xPointOut   = trails_xPoints[0][0];
    }

    for (i = 3; i<=5; i++){
        semaphores[i].xPointStop    = W/2 +(i-1)*SPACE - (TRAIN_W + 5);
        semaphores[i].xPointDraw    = semaphores[i].xPointStop + (TRAIN_W + 5);
        semaphores[i].yPointDraw    = (6-i)*SPACE;
        semaphores[i].xPointTrail   = W/2 + (2*i - 3)*SPACE; 
        semaphores[i].yPointTrail   = H/2 - SPACE/2;
        semaphores[i].trail_angle   = TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE;
        semaphores[i].xPointIn      = semaphores[i].xPointStop - SLOW_DOWN_SPACE;
        semaphores[i].xPointOut     = trails_xPoints[0][3];
           
        semaphores[i+6].xPointStop  = W/2 + (2*i - 3)*SPACE + (TRAIN_W + 5);
        semaphores[i+6].xPointDraw  = semaphores[i+6].xPointStop - (TRAIN_W + 5);
        semaphores[i+6].yPointDraw  = H/2 + SPACE/2;
        semaphores[i+6].xPointTrail = semaphores[i+6].xPointDraw;
        semaphores[i+6].yPointTrail = semaphores[i+6].yPointDraw;
        semaphores[i+6].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE;
        semaphores[i+6].xPointIn    = semaphores[i+6].xPointStop + SLOW_DOWN_SPACE;
        semaphores[i+6].xPointOut   = semaphores[i+6].xPointDraw - TRAIL_W;
    }

    for (i = 0; i < SEMAPHORES_NUM; i++){
        semaphores[i].queue             = 0;
        semaphores[i].status            = true;
        semaphores[i].sem               = create_bitmap(sem_g->w, sem_g->h);
        semaphores[i].trail_state       = TRAIL_OFF;
        semaphores[i].trail_angle_cnt   = 0;
        semaphores[i].trail_angle_inc   = 0.01;
        semaphores[i].move_queue        = false;
        blit(sem_g, semaphores[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
    }

    // STRUTTURA STAZIONE
    for (i = 0; i < STATIONS_NUM/2; i++){
        station[i].xPointStop = (W/2);
        station[i].xPointDraw = station[i].xPointStop + (TRAIN_W + 5);
        station[i].yPointDraw = (i + 1) * SPACE;
        station[i].xPointIn   = station[i].xPointStop - SLOW_DOWN_SPACE;
        station[i].xPointOut  = station[i].xPointDraw + TRAIN_W;
        station[i].status     = true;
        station[i].queue      = 0; 
        station[i].sem        = create_bitmap(sem_g->w, sem_g->h);
        station[i].move_queue = false;
        blit(sem_g, station[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
    }
        for (i = STATIONS_NUM/2; i < STATIONS_NUM; i++){
        station[i].xPointStop = (W/2);
        station[i].xPointDraw = station[i].xPointStop - 1.5*STOP_SPACE;
        station[i].yPointDraw = (i + 1) * SPACE;
        station[i].xPointIn   = station[i].xPointStop + SLOW_DOWN_SPACE;
        station[i].xPointOut  = station[i].xPointDraw - TRAIN_W;
        station[i].status     = true;
        station[i].queue      = 0; 
        station[i].sem        = create_bitmap(sem_g->w, sem_g->h);
        station[i].move_queue = false;
        blit(sem_g, station[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
    }

    // DISEGNO I BINARI
    y1 = 4*SPACE;
    y2 = 5*SPACE;
    line(background, 0, 4*SPACE, W, 4*SPACE, red);
    line(background, 0, 5*SPACE, W, 5*SPACE, red);
    trail_diag = (int) TRAIL_W*sqrt(2)/2;

    for (i = 0; i < 3; i++) { 
        y0 = (i+1)*SPACE;
        y3 = (8-i)*SPACE;
        x0 = trails_xPoints[i][0];
        x1 = trails_xPoints[i][1];
        x2 = trails_xPoints[i][2];
        x3 = trails_xPoints[i][3];
        line(background, x1, y0, x2, y0, red);                  
        line(background, x0 + trail_diag, y1 - trail_diag, x1, y0, red);        
        line(background, x2, y0, x3 - trail_diag, y1 - trail_diag, red);        
        line(background, x1, y3, x2, y3, red);                  
        line(background, x0 + trail_diag, y2 + trail_diag, x1, y3, red);        
        line(background, x2, y3, x3 - trail_diag, y2 + trail_diag, red);        
        line(background, x0, y1, x0 + TRAIL_W, y1, grey);
        line(background, x0, y2, x0 + TRAIL_W, y2, grey);
        line(background, x3, y1, x3 - TRAIL_W, y1, grey);
        line(background, x3, y2, x3 - TRAIL_W, y2, grey);
    }
    
    // CREAZIONE TASK
    task_create(graphics,           GRAPHIC_TASK_ID,            GRAPHIC_TASK_PERIOD,            GRAPHIC_TASK_DL,            GRAPHIC_TASK_PRIO);               
    task_create(station_manager,    STATION_MANAGER_TASK_ID,    STATION_MANAGER_TASK_PERIOD,    STATION_MANAGER_TASK_DL,    STATION_MANAGER_TASK_PRIO);   
    task_create(user_task,          USER_TASK_ID,               USER_TASK_PERIOD,               USER_TASK_DL,               USER_TASK_PRIO);                  

    printf("Initialization completed!\n");
    printf("Press SPACE to create a new random train \n");
}

//-------------------------------------------------------------------------------------------------------------------------
// TASK user_task
// 
// Gestisce mouse e tastiera 
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
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    i++;
                    break;
                
                case 1:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    train_par[i].priority = 3;
                    i++;
                    break;
                    
                case 2:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    train_par[i].priority = 2;
                    i++;
                    break;
                
                case 3:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    train_par[i].priority = 1;
                    i++;
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
        else    for (j = 0; j<= N_BUTTONS; j++) button[j].state = false;
        
        // PARTE 2: legge i comandi da tastiera
        scan = get_scancode();
        switch(scan) {

            case KEY_SPACE:
                if (i == TMAX ) i = 1;
                task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
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
//
// individua il pulsante premuto
//-------------------------------------------------------------------------------------------------------------------------
int check_button(int x, int y){
    int i;

    for (i = 0; i < N_BUTTONS; i++) {
        if (x < button[i].x_max && x > button[i].x_min &&
            y < button[i].y_max && y > button[i].y_min)         break;   
    }
    return i;
}

//-------------------------------------------------------------------------------------------------------------------------
// TASK graphics
// 
// Gestisce tutta la parte grafica
//-------------------------------------------------------------------------------------------------------------------------

void *graphics(void *p){

    int id;
    int sem_w;
    int sem_h;
    int i;
    int j;
    char str[50];
    char station_number[2];
    char sem_number[2];
    char queue_list[100];

    id = get_task_id(p);
    set_activation(id);

    sem_w = sem_r->w*SEM_SIZE_FACTOR ;
    sem_h = sem_r->h*SEM_SIZE_FACTOR ;
    
    while(EXIT == false) {

        blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
        blit(interface, interface_buffer, 0, 0, 0, 0, interface->w, interface->h);

        // SEMAFORI DEGLI INCROCI
        for (i = 0; i < SEMAPHORES_NUM; i++){
            if (semaphores[i].status == false)      semaphores[i].sem = sem_r;
            else                                    semaphores[i].sem = sem_g;
            
            sprintf(sem_number,"%d",i);
            textout_ex(buffer, font, sem_number, semaphores[i].xPointDraw, semaphores[i].yPointDraw + 10, (0,25,0), -1);
            stretch_sprite(buffer, semaphores[i].sem, semaphores[i].xPointDraw - sem_w/2, semaphores[i].yPointDraw - sem_h, sem_w, sem_h);
        }
        
        // SEMAFORI DELLA STAZIONE
        for (i = 0; i < STATIONS_NUM; i++){
            if (station[i].status == false)     station[i].sem = sem_r;
            else                                station[i].sem = sem_g;
            
            stretch_sprite(buffer, station[i].sem, station[i].xPointDraw - sem_w/2, station[i].yPointDraw - sem_h, sem_w, sem_h);
            sprintf(station_number,"%d",i);
            textout_ex(buffer, font, station_number, station[i].xPointDraw, station[i].yPointDraw + 10, (0,0,0), -1);
        }

        // BINARI MOBILI
        for (i = 0; i <= 2; i++){
            pivot_sprite(buffer,trail, semaphores[i].xPointTrail, semaphores[i].yPointTrail - TRAIL_H/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointTrail, semaphores[i+6].yPointTrail - TRAIL_H/2, 0, 0, itofix(semaphores[i+6].trail_angle));
        }
        for (i = 3; i <= 5; i++){
            pivot_sprite(buffer,trail, semaphores[i].xPointTrail, semaphores[i].yPointTrail + TRAIL_H/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointTrail, semaphores[i+6].yPointTrail + TRAIL_H/2, 0, 0, itofix(semaphores[i+6].trail_angle));
        }

        // TRENI
        for (i = 1; i < TMAX; i++){
            if (train_par[i].run == true){
                for (int j = 0; j < WAGONS; j++){
                    draw_sprite(buffer, train_par[i].wagons[j].bmp, train_par[i].wagons[j].posx, train_par[i].wagons[j].posy);
                }
            }
        }

        // INTERFACCIA
        for (j = 0; j < N_BUTTONS ; j++) {
            if (button[j].state == true)    blit(button[j].button_on, interface_buffer, 0, 0, button[j].x_min, button[j].y_min, L_BUTTONS, L_BUTTONS);
        }

        // DEBUG GRAFICO
        sprintf(str, "Queue: %d", trains_in_binary[0]);
        textout_ex(buffer, font, str, 550, 1.2*SPACE, (0,0,0),-1);
        sprintf(str, "Queue: %d", trains_in_binary[1]);
        textout_ex(buffer, font, str, 550, 2.2*SPACE, (0,0,0),-1);
        sprintf(str, "Queue: %d", trains_in_binary[2]);
        textout_ex(buffer, font, str, 550, 3.2*SPACE, (0,0,0),-1);
        sprintf(str, "Queue: %d", trains_in_binary[3]);
        textout_ex(buffer, font, str, 550, 4.2*SPACE, (0,0,0),-1);
        sprintf(str, "ready trains number %d", ready_trains_num);
        textout_ex(interface_buffer, font, str, 400, 20, (0,0,0), -1);

        // int k = 4;
        // line(buffer, station[k].xPointIn, 0, station[k].xPointIn, H, (0,0,0));
        // line(buffer, station[k+1].xPointIn, 0, station[k+1].xPointIn, H, (0,0,0));
        // line(buffer, station[k+2].xPointIn, 0, station[k+2].xPointIn, H, (0,0,0));
        // line(buffer, station[k+3].xPointIn, 0, station[k+3].xPointIn, H, (0,0,0));



        // DISEGNO IL BUFFER SULLO SCHERMO
        blit(buffer, screen, 0, 0, 0, 0, background->w, background->h);
        blit(interface_buffer, screen, 0, 0, 0, H, interface->w, interface->h);
        show_mouse(screen);

        // DEADLINE MISS
        if (deadline_miss(id))      printf("deadline miss of graphic task\n");
              
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
//-------------------------------------------------------------------------------------------------------------------------
// TASK station_manager
//
// Assegna i binari ai nuovi treni, gestisce semafori e incroci
//-------------------------------------------------------------------------------------------------------------------------
void *station_manager(void *p){
    int     id;
    int     i;
    int     j;
    struct  timespec now;

    id = get_task_id(p);
    set_activation(id);

    while (EXIT == false){
        
        // ASSEGNAZIONE BINARI
        binary_assignment();

        // GESTIONE TRENI
        // Controlla la prossima fermata di ogni treno in base al binario
        for (i = 1; i < TMAX; i++){

            if (train_par[i].run == true){

                train_par[i].checked = false;

                switch(train_par[i].binary){
                case(0):

                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_ON);
                    // check stazione
                    checkStation(i);
                    // check semaforo
                    //checkSemaphoreOut(i, 5, TRAIL_ON);

                    break;
                case(1):

                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_ON);
                    // check stazione
                    checkStation(i);
                    // check semaforo
                    //checkSemaphoreOut(i, 4, TRAIL_ON); // il semaforo 4 in realtà dovrebbe controllare da solo anche il 5....

                    break;
                case(2):
                    // check semaforo
                    //checkSemaphoreIn(i, 0, TRAIL_OFF);
                    //checkSemaphoreIn(i, 1, TRAIL_OFF);
                    checkSemaphoreIn(i, 2, TRAIL_ON);
                    // check stazione
                    checkStation(i);
                    // check semaforo
                    //checkSemaphoreOut(i, 3, TRAIL_ON); // il semaforo 3 in realtà dovrebbe controllare da solo anche il 4 e 5....
                    break;
                case(3):
                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_OFF);
                    checkSemaphoreIn(i, 2, TRAIL_OFF);
                    // check stazione
                    checkStation(i);                        // La stazione centrale deve controllare i sem successivi
                    // }
                    break;
                }
            }
        }

        // GESTIONE CODE DEI SEMAFORI E DELLE STAZIONI
        for (i = 0; i < STATIONS_NUM; i++){
            if (station[i].move_queue == true)     move_station_queue(i);
        }

        move_semaphore_queue();

        wait_for_activation(id);
    }
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE binary_assignment
// 
// Assegna il binario corretto ad ogni nuovo treno
//-------------------------------------------------------------------------------------------------------------------------
void binary_assignment(){

    bool    run;
    bool    binary_assigned;
    int     i;
    int     bin;
    int     trainId;
    int     priority;
    int     ready_trains_num_local;

        
    pthread_mutex_lock(&ready_trains_num_mutex);
    ready_trains_num_local = ready_trains_num;
    pthread_mutex_unlock(&ready_trains_num_mutex);


    if(ready_trains_num_local > 0){
            for (i = 0; i < ready_trains_num_local; i++){
                
                pthread_mutex_lock(&last_assigned_train_id_mutex);
                trainId = last_assigned_train_id + 1;
                pthread_mutex_unlock(&last_assigned_train_id_mutex);

                pthread_mutex_lock(&train_par[trainId].mutex);
                binary_assigned = train_par[trainId].binary_assigned;
                priority        = train_par[trainId].priority;
                pthread_mutex_unlock(&train_par[trainId].mutex);

                if(binary_assigned == false) {

                    pthread_mutex_lock(&trains_in_binary_mutex);
                    switch (priority) {

                    case 3:
                        if (trains_in_binary[3] <= trains_in_binary[2])         bin = 3;
                        else if (trains_in_binary[2] <= trains_in_binary[1])    bin = 2;
                        else if (trains_in_binary[1] <= trains_in_binary[0])    bin = 1;
                        else                                                    bin = 0;
                        break;
                    
                    case 2:
                        if (trains_in_binary[2] <= trains_in_binary[1])         bin = 2;
                        else if (trains_in_binary[1] <= trains_in_binary[0])    bin = 1;
                        else                                                    bin = 0;
                        break;
                    
                    case 1:
                        if (trains_in_binary[1] <= trains_in_binary[0])         bin = 1;
                        else                                                    bin = 0;
                        break;

                    default:
                        break;
                    }
                    pthread_mutex_unlock(&trains_in_binary_mutex);

                    pthread_mutex_lock(&train_par[trainId].mutex);
                    train_par[trainId].binary             = bin;
                    train_par[trainId].binary_occupied    = true;
                    train_par[trainId].binary_assigned    = true;
                    pthread_mutex_unlock(&train_par[trainId].mutex);

                    pthread_mutex_lock(&last_assigned_train_id_mutex);
                    last_assigned_train_id = trainId;
                    pthread_mutex_unlock(&last_assigned_train_id_mutex);

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
// FUNZIONE checkSemaphoreIn
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void checkSemaphoreIn(int trainId, int semId, int SemRequiredState){

}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_semaphore_queue
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void move_semaphore_queue(){
    int i;

        for (i = 0; i < SEMAPHORES_NUM; i++){
            if (semaphores[i].move_queue ==  true){
                semaphores[i].move_queue = false;
                semaphores[i].queue --;
            }
        }
}
//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE CheckStation
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void checkStation(int trainId) {
    bool    checked;
    bool    station_passed;
    bool    semaphore_flag;
    struct  timespec    now;
    struct  timespec    leave_time;
    int     stationId;
    int     posx;
    
    pthread_mutex_lock(&train_par[trainId].mutex);
    stationId       = train_par[trainId].binary;
    posx            = train_par[trainId].posx;
    checked         = train_par[trainId].checked;
    station_passed  = train_par[trainId].station_passed[stationId];
    pthread_mutex_unlock(&train_par[trainId].mutex);

    if (checked == false) {
        
        // Controllo se mi trovo nel range della stazione
        if (posx > station[stationId].xPointIn && posx < station[stationId].xPointOut && station_passed == false ){

            // Flag per evitare di controllare tutti i semafori successivi
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].checked = true;
            semaphore_flag  = train_par[trainId].semaphore_flag;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            // Eseguito solo una volta quando il semaforo diventa verde
            if (semaphore_flag == false){
                
                pthread_mutex_lock(&train_par[trainId].mutex);

                // Flag per comunicare che il treno è in stazione
                train_par[trainId].semaphore_flag = true;

                // Eseguito solo la prima volta che il treno entra in stazione
                if (train_par[trainId].queue == false) {

                    // Salvo la posizione in coda del treno
                    train_par[trainId].pos_in_queue = station[stationId].queue;
                    
                    // Salvo la posizione in cui mi dovrò fermare
                    train_par[trainId].stop_x = station[stationId].xPointStop;
                    train_par[trainId].stop_id = stationId;
                    train_par[trainId].stop_type = STATION;

                    // Inserisco il treno in coda
                    station[stationId].queue_list[station[stationId].queue] = trainId;
                    station[stationId].queue        += 1;
                    station[stationId].xPointIn     -= TRAIN_SPACE;
                    station[stationId].xPointStop   -= TRAIN_SPACE;
                }

                pthread_mutex_unlock(&train_par[trainId].mutex);

                // Aggiorno la struttura della stazione, il semaforo diventa rosso e salvo l'istante deve tornare verde
                clock_gettime(CLOCK_MONOTONIC, &now);                
                time_copy(&leave_time, now);
                time_add_ms(&leave_time, STOP_TIME);
                time_copy(&station[stationId].t, leave_time);

                // Aggiorno la struttura della stazione
                station[stationId].status = false;
            }

            // SEMAFORO ROSSO
            if (station[stationId].status == false) {
                // Timer per far tornare il semaforo della stazione verde
                clock_gettime(CLOCK_MONOTONIC, &now);
                if (time_cmp(now, station[stationId].t) == 1) {
                    time_add_ms(&now, 200);
                    time_copy(&station[stationId].t, now);
                    station[stationId].status = true;

                    pthread_mutex_lock(&train_par[trainId].mutex);
                    train_par[trainId].station_passed[stationId] = true;
                    pthread_mutex_unlock(&train_par[trainId].mutex);
                }     
            }   
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

    // Legge l'istante iniziale
    clock_gettime(CLOCK_MONOTONIC, &now);

    // Muove la coda 
    for (i = 1; i <= station[stationId].queue; i++) {
        station[stationId].queue_list[i-1] = station[stationId].queue_list[i];

        trainId = station[stationId].queue_list[i-1];
        pthread_mutex_lock(&train_par[trainId].mutex);
        train_par[trainId].ready_to_go_flag = true;
        pthread_mutex_unlock(&train_par[trainId].mutex);
    }

    station[stationId].move_queue   = false;
    station[stationId].xPointIn     += TRAIN_SPACE;
    station[stationId].xPointStop   += TRAIN_SPACE;
    station[stationId].queue --;

    // Dopo il tempo di attesa il semaforo torna rosso
    if (station[i].queue > 0 && time_cmp(now, station[i].t) == 1)   station[i].status = false;
      
}

//-------------------------------------------------------------------------------------------------------------------------
// TASK train
//
// Crea un nuovo task treno e ne gestisce il movimento
//-------------------------------------------------------------------------------------------------------------------------

void *train(void *p) {
    bool    first_of_queue;
    bool    semaphore_flag;
    bool    binary_assigned;
    int     id;
    int     stop_id;
    int     stop_type;
    int     j;
    int     posx;
    int     stop_x;
    int     previous_train_pos_x;
    int     curr_state;
    int     prev_state;
    int     next_state;
    int     K;
    int     acc;
    float   vel;

    // INIT TASK
    id = get_task_id(p);
    set_activation(id);
    set_train_parameters(id);

    binary_assigned = false;

    // ATTENDO ASSEGNAZIONE BINARIO
    while(binary_assigned == false){

        pthread_mutex_lock(&train_par[id].mutex);
        binary_assigned = train_par[id].binary_assigned;
        pthread_mutex_unlock(&train_par[id].mutex);

        wait_for_activation(id);
        }
    
    // CHECK COLLISIONI IN INGRESSO ALLA STAZIONE
    if(id != 1) {
        pthread_mutex_lock(&train_par[id-1].mutex);
        previous_train_pos_x = train_par[id-1].posx;
        pthread_mutex_unlock(&train_par[id-1].mutex);

        while (previous_train_pos_x < 2*TRAIN_W*WAGONS) {

            pthread_mutex_lock(&train_par[id-1].mutex);
            previous_train_pos_x = train_par[id-1].posx;
            pthread_mutex_unlock(&train_par[id-1].mutex);

            wait_for_activation(id);
        }
    }

    // TRENO PRONTO 
    
    // Salvo il locale i parametri iniziali del treno
    pthread_mutex_lock(&train_par[id].mutex);
    train_par[id].run = true;
    posx    = train_par[id].posx;
    stop_x  = train_par[id].stop_x;
    pthread_mutex_unlock(&train_par[id].mutex);

    // INIT MACCHINA A STATI
    curr_state = GO_FAST;
    prev_state = curr_state;
    next_state = curr_state;

    // TRENO IN MOVIMENTO
    while (posx < W      &&      EXIT == false){

        // Aggiorno in locale i parametri del treno
        pthread_mutex_lock(&train_par[id].mutex);
        train_par[id].posx  = train_par[id].wagons[0].posx;
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
                if (posx >= stop_x ) {
                    if (first_of_queue == true)     next_state = STOP; 
                    else                            next_state = QUEUE;
                }
                break;

            case(QUEUE):            
                // MOVIMENTO DELLA CODA DEL SEMAFORO
                pthread_mutex_lock(&train_par[id].mutex);
                if (train_par[id].ready_to_go_flag == true) {
                    train_par[id].ready_to_go_flag = false;
                    train_par[id].queue = true;
                    train_par[id].stop_x += TRAIN_SPACE;
                    train_par[id].currentVel = MAX_VEL;
                    train_par[id].semaphore_flag = false;
                    train_par[id].pos_in_queue --;
                    next_state = SLOW_DOWN;
                }
                pthread_mutex_unlock(&train_par[id].mutex);

                break;

            case(STOP):
                // FERMATA AL SEMAFORO
                // In base a tipo e ID controllo quando il sem diventa verde per ripartire
                if (stop_type == STATION && station[stop_id].status == true) {
                    next_state = SPEED_UP;
                    pthread_mutex_lock(&train_par[id].mutex);
                    train_par[id].queue = false;
                    pthread_mutex_unlock(&train_par[id].mutex);

                    station[stop_id].move_queue = true;
                }
                if (stop_type == SEMAPHORE && semaphores[stop_id].status == true) {
                    next_state = SPEED_UP;
                    pthread_mutex_lock(&train_par[id].mutex);
                    train_par[id].queue = false;
                    pthread_mutex_unlock(&train_par[id].mutex);

                    semaphores[stop_id].move_queue = true;
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
        if (posx > W/2 && train_par[id].binary_occupied == true) {

            pthread_mutex_lock(&trains_in_binary_mutex);
            trains_in_binary[train_par[id].binary]--;
            pthread_mutex_unlock(&trains_in_binary_mutex);
            
            train_par[id].binary_occupied = false;
        }
        pthread_mutex_unlock(&train_par[id].mutex);

        // ATTENDO IL PROSSIMO CICLO
        wait_for_activation(id);
    }

    // USCITA DAL TASK
    train_par[id].run = false;
    pthread_mutex_destroy(&train_par[id].mutex);
    ptask_exit(id);
    return 0;
}

//----------------------------------------------------------------------------
// FUNZIONE set_train_parameters
//
// inizializza tutti i parametri del treno
//---------------------------------------------------------------------------
void set_train_parameters(int i) {
    
    int random_num;
    int j;

    pthread_mutex_t     train_mux = PTHREAD_MUTEX_INITIALIZER;

    // Incremento il numero di treni in attesa del binario (GLOBALE)
    pthread_mutex_lock(&ready_trains_num_mutex);
    ready_trains_num ++;
    pthread_mutex_unlock(&ready_trains_num_mutex);

    // Seme per la funzione rand()
    srand(time(NULL)+ i);             
    random_num = rand();

    // Inizializzo la struttura del treno
    train_par[i].run                = false;     
    train_par[i].binary_assigned    = false; 
    train_par[i].ready_to_go_flag   = false; 
    train_par[i].semaphore_flag     = false; 
    train_par[i].checked            = false;
    train_par[i].queue              = false;
    train_par[i].binary_occupied    = false;

    // Struttura per indicare se il treno è già passato da una stazione
    for (j = 0; j < STATIONS_NUM; j++)      train_par[i].station_passed[j] = false;
    // Struttura per indicare se il treno è già passato da un semaforo
    for (j = 0; j < SEMAPHORES_NUM; j++)    train_par[i].sem_passed[j] = false;

    train_par[i].direction = 0;
//  train_par[i].direction              = (random_num)%2; // direzione random da 0 a 1
//  train_par[i].binary                 = 0;
    
    // Se non viene indicata una priorità, ne scelgo una random da 1 a 3
    if (train_par[i].priority == 0)     train_par[i].priority = 1 + (random_num)%3;
    if (train_par[i].direction == 0)    train_par[i].posx = - TRAIN_W;
    else                                train_par[i].posx = W;
    
    train_par[i].count                  = 0;
    train_par[i].stop_x                 = W;
    train_par[i].stop_type              = SEMAPHORE;
    train_par[i].stop_id                = 11*train_par[i].direction;
    train_par[i].pos_in_queue           = 0;
    train_par[i].first_diagonal_wagon   = 0;
    train_par[i].currentVel             = MAX_VEL;
    train_par[i].mutex                  = train_mux;
    // Parametri dei vari vagoni
    for (j = 0; j < WAGONS; j++){

        train_par[i].wagons[j].bmp              = train_bmp[1].train1;

        if (train_par[i].direction == 0){
            train_par[i].wagons[j].posx = - (j*(TRAIN_W + WAGONS_SPACE));
            train_par[i].wagons[j].posy = H/2 - SPACE/2 - TRAIN_H/2;
        }
        else {
            // DA CONTROLLARE
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
    int     priority;
    int     bin;
    int     j;
    int     wagon_posx;
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
    pthread_mutex_unlock(&train_par[i].mutex);

    deltaSpace  = newVel * periodS;
    step        = round(deltaSpace);

    pthread_mutex_lock(&train_par[i].mutex);
    priority    = train_par[i].priority; 
    bin         = train_par[i].binary;
    pthread_mutex_unlock(&train_par[i].mutex);
    
    for (j = 0; j < WAGONS; j++) {

        pthread_mutex_lock(&train_par[i].mutex);
        wagon_posx = train_par[i].wagons[j].posx + TRAIN_W;
        pthread_mutex_unlock(&train_par[i].mutex);

        if      (wagon_posx > trails_xPoints[bin][0]   &&   wagon_posx < trails_xPoints[bin][1]){
            move_diag_up(i, j, priority, step);
        }
        else if (wagon_posx > trails_xPoints[bin][2] + round(TRAIN_W*sqrt(2)/2)  &&   wagon_posx < trails_xPoints[bin][3]){
            
            move_diag_down(i, j, priority, step);
        }
        else{
            // i due if qua sotto servono a riallineare i vagoni dopo essersi mossi in diagonale visto che si disallineavano
            if (wagon_posx < trails_xPoints[bin][2] - TRAIN_H   &&   wagon_posx > trails_xPoints[bin][1] - TRAIN_W) {
                train_par[i].wagons[j].posy = (bin + 1)*SPACE - TRAIN_H/2;
                
            }
            else if (wagon_posx > trails_xPoints[bin][3] - TRAIN_H) {
                train_par[i].wagons[j].posy = (H - SPACE - TRAIN_H)/2;
            }
            move_forward(i, j, priority, step);
        }
    }
}
/*-------------------------------------------------------------------------*/
void move_diag_up(int i, int j ,int priority, int inc){
//     int k;
//     int first_wagon;

//     inc = round(inc*sqrt(2)/2);

//     if (j == first_wagon) {
//         train_par[i].wagons[j].posx += inc;
//         train_par[i].wagons[j].posy -= inc;
//         train_par[i].wagons[j].bmp = train_bmp[priority].train2;
//     }
//     else {
//         train_par[i].wagons[j].posx = train_par[i].wagons[j-1].posx - round((TRAIN_W + WAGONS_SPACE)*sqrt(2)/2);
//         train_par[i].wagons[j].posy = train_par[i].wagons[j-1].posy + round((TRAIN_W + WAGONS_SPACE)*sqrt(2)/2);
//         train_par[i].wagons[j].bmp = train_bmp[priority].train2;
//     }
// }

    if (j == 0){
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += inc;
        //train_par[i].posy -= inc;
    }
    train_par[i].wagons[j].bmp = train_bmp[priority].train2;
    train_par[i].wagons[j].posx += inc;
    train_par[i].wagons[j].posy -= inc;
    // train_par[i].wagons[j].posx = train_par[i].wagons[j-1].posx - (j*(TRAIN_W*1.1)); 
    // train_par[i].wagons[j].posy = train_par[i].wagons[j-1].posy - (j*(TRAIN_W*1.1));
}
/*-------------------------------------------------------------------------*/
void move_diag_down(int i, int j, int priority, int inc){
    
    if (j == 0){
        // aggiorna la posizione di riferimento del treno come quella del primo vagone, 
        // così che continuino a funzionare tutti i check fatti nel codice
        train_par[i].posx += (int)(((float)inc)*cosf(M_PI_4));
        //train_par[i].posy += (int)(((float)inc)*sinf(M_PI_4));    
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
    train_par[i].wagons[j].posx = train_par[i].posx - (j*(TRAIN_W*1.1));

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