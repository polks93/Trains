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
    
    int     i;
    int     x0, x1, x2, x3, y0, y1, y2, y3;
    int     col, black, red, blue, green, orange, white, grey;
    int     trail_diag;
    char    str[50];

    printf("Loading ... \n");

    // INIT MUTEX
    pthread_mutex_init(&ready_trains_num_mutex,             NULL);
    pthread_mutex_init(&last_assigned_train_id_mutex,       NULL);
    pthread_mutex_init(&trains_in_binary_mutex,             NULL);
    pthread_mutex_init(&ASSIGNED_DIRECTION_MUTEX,           NULL);
    pthread_mutex_init(&user_direction_mutex,               NULL);
    pthread_mutex_init(&last_assigned_train_from_dx_mutex,  NULL);
    pthread_mutex_init(&last_assigned_train_from_sx_mutex,  NULL);

    for (i = 0; i < STATIONS_NUM; i++)      pthread_mutex_init(&station[i].mutex,       NULL);
    for (i = 0; i < SEMAPHORES_NUM; i++)    pthread_mutex_init(&semaphores[i].mutex,    NULL);

    // INIZIALIZZAZIONE VARIABILI GLOBALI
    EXIT                                = false;
    EXIT_COMMAND                        = false;
    ASSIGNED_DIRECTION                  = false;
    max_prio_train_found                = false;
    ready_trains_num                    = 0;
    last_assigned_train_id              = 0;
    total_train_dl                      = 0;
    user_direction                      = FROM_DX;
    last_assigned_train_from_dx.tv_sec  = 0;
    last_assigned_train_from_dx.tv_nsec = 0;
    last_assigned_train_from_sx.tv_sec  = 0;
    last_assigned_train_from_sx.tv_nsec = 0;

    for (i = 0; i < STATIONS_NUM; i++)      trains_in_binary[i] = 0;

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
    // green
    train_bmp[0].train1 = load_bitmap("img/trains/green.bmp", NULL);
    train_bmp[0].train2 = load_bitmap("img/trains/green_diag.bmp", NULL);
    train_bmp[0].train3 = load_bitmap("img/trains/green_diag2.bmp", NULL);    
    
    // blue
    train_bmp[1].train1 = load_bitmap("img/trains/blue.bmp", NULL);
    train_bmp[1].train2 = load_bitmap("img/trains/blue_diag.bmp", NULL);
    train_bmp[1].train3 = load_bitmap("img/trains/blue_diag2.bmp", NULL);

    // red
    train_bmp[2].train1 = load_bitmap("img/trains/red.bmp", NULL);
    train_bmp[2].train2 = load_bitmap("img/trains/red_diag.bmp", NULL);
    train_bmp[2].train3 = load_bitmap("img/trains/red_diag2.bmp", NULL);

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
    
    from_dx_off = load_bitmap("img/buttons/from_dx_off.bmp", NULL);
    from_dx_on = load_bitmap("img/buttons/from_dx_on.bmp", NULL);

    from_sx_off = load_bitmap("img/buttons/from_sx_off.bmp", NULL);
    from_sx_on = load_bitmap("img/buttons/from_sx_on.bmp", NULL);

    rnd_direction_off = load_bitmap("img/buttons/rnd_direction_off.bmp", NULL);
    rnd_direction_on = load_bitmap("img/buttons/rnd_direction_on.bmp", NULL);
    
    close_program_off = load_bitmap("img/buttons/exit_off.bmp", NULL);
    close_program_on = load_bitmap("img/buttons/exit_on.bmp", NULL);

    // BITMAPS FRECCE
    green_arrow_dx = load_bitmap("img/arrows/green_arrow_dx.bmp", NULL);
    green_arrow_sx = load_bitmap("img/arrows/green_arrow_sx.bmp", NULL);
    red_arrow_dx = load_bitmap("img/arrows/red_arrow_dx.bmp", NULL);
    red_arrow_sx = load_bitmap("img/arrows/red_arrow_sx.bmp", NULL);

    // INIZIALIZZAZIONE GRAFICA
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, WINDOW_H, 0, 0);
    background          = create_bitmap(W, H);
    buffer              = create_bitmap(W, H);
    interface           = create_bitmap(INTERFACE_W, INTERFACE_H);
    interface_buffer    = create_bitmap(INTERFACE_W, INTERFACE_H);
    clear_to_color(background, grey);
    clear_to_color(interface, grey);
    rectfill(background, 0, H-1, W-1, H-10, black);

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

    // close program
    button[7].button_off    = close_program_off;
    button[7].button_on     = close_program_on;
    button[7].state         = false;
    button[7].x_min         = INTERFACE_W - 2*SPACE_BUTTONS - L_BUTTONS;
    button[7].x_max         = button[7].x_min + L_BUTTONS;
    button[7].y_min         = SPACE_BUTTONS;
    button[7].y_max         = SPACE_BUTTONS + L_BUTTONS;

    // Disegno i pulsanti sul buffer
    //for (i = 0; i < N_BUTTONS; i++)        blit(button[i].button_off, interface, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);

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

    // STRUTTURA SEMAFORI
    for (i = 0; i<=2; i++){
        semaphores[i].xPointStop    = W/2 + (2*i - 7)*SPACE - 5;
        semaphores[i].xPointDraw    = semaphores[i].xPointStop + 5;
        semaphores[i].yPointDraw    = H/2 - SPACE/2;
        semaphores[i].xPointTrail   = semaphores[i].xPointDraw;
        semaphores[i].yPointTrail   = semaphores[i].yPointDraw;
        semaphores[i].trail_angle   = TRAIL_UP_BIN_IN_SWITCH_OFF;
        semaphores[i].xPointIn      = semaphores[i].xPointStop - SLOW_DOWN_SPACE;
        semaphores[i].xPointOut     = semaphores[i].xPointDraw + TRAIL_W;
        
        semaphores[i+6].xPointStop  = W/2 + (i - 4)*SPACE + 5;
        semaphores[i+6].xPointDraw  = W/2 - (i+2)*SPACE;
        semaphores[i+6].yPointDraw  = (6+i)*SPACE;
        semaphores[i+6].xPointTrail = W/2 + (2*i - 7)*SPACE;
        semaphores[i+6].yPointTrail = H/2 + SPACE/2;
        semaphores[i+6].trail_angle = TRAIL_DOWN_BIN_OUT_SWITCH_OFF;
        semaphores[i+6].xPointIn    = semaphores[i+6].xPointStop + SLOW_DOWN_SPACE;
        semaphores[i+6].xPointOut   = trails_xPoints[0][0];
    }

    for (i = 3; i<=5; i++){
        semaphores[i].xPointStop    = W/2 +(i-1)*SPACE - 5;
        semaphores[i].xPointDraw    = semaphores[i].xPointStop + 5;
        semaphores[i].yPointDraw    = (6-i)*SPACE;
        semaphores[i].xPointTrail   = W/2 + (2*i - 3)*SPACE; 
        semaphores[i].yPointTrail   = H/2 - SPACE/2;
        semaphores[i].trail_angle   = TRAIL_UP_BIN_OUT_SWITCH_OFF;
        semaphores[i].xPointIn      = semaphores[i].xPointStop - SLOW_DOWN_SPACE;
        semaphores[i].xPointOut     = trails_xPoints[0][3];
           
        semaphores[i+6].xPointStop  = W/2 + (2*i - 3)*SPACE + 5;
        semaphores[i+6].xPointDraw  = semaphores[i+6].xPointStop - 5;
        semaphores[i+6].yPointDraw  = H/2 + SPACE/2;
        semaphores[i+6].xPointTrail = semaphores[i+6].xPointDraw;
        semaphores[i+6].yPointTrail = semaphores[i+6].yPointDraw;
        semaphores[i+6].trail_angle = TRAIL_DOWN_BIN_IN_SWITCH_OFF;
        semaphores[i+6].xPointIn    = semaphores[i+6].xPointStop + SLOW_DOWN_SPACE;
        semaphores[i+6].xPointOut   = semaphores[i+6].xPointDraw - TRAIL_W;
    }

    for (i = 0; i < SEMAPHORES_NUM; i++){
        semaphores[i].queue             = 0;
        semaphores[i].status            = true;
        semaphores[i].sem               = create_bitmap(sem_g->w, sem_g->h);
        semaphores[i].trail_state       = TRAIL_OFF;
        semaphores[i].trail_angle_cnt   = 0;
        semaphores[i].trail_angle_inc   = 1;
        semaphores[i].move_queue        = false;
        blit(sem_g, semaphores[i].sem, 0, 0, 0, 0, sem_g->w, sem_g->h);
    }

    // STRUTTURA STAZIONE
    for (i = 0; i < STATIONS_NUM/2; i++){
        station[i].xPointStop = (W/2) + TRAIN_W;
        station[i].xPointDraw = station[i].xPointStop + 5;
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
        station[i].xPointStop = (W/2) - TRAIN_W;
        station[i].xPointDraw = station[i].xPointStop - 5;
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

    // DISEGNO LEGENDA SU SCHERMO
    for (i = 0; i < WAGONS; i++) {
    draw_sprite(background, train_bmp[2].train1, (i + 1) * (TRAIN_W + WAGONS_SPACE), TRAIN_W);
    draw_sprite(background, train_bmp[1].train1, (i + 1) * (TRAIN_W + WAGONS_SPACE), 2*TRAIN_W);
    draw_sprite(background, train_bmp[0].train1, (i + 1) * (TRAIN_W + WAGONS_SPACE), 3*TRAIN_W);
    }
    sprintf(str, " : HIGH PRIORITY TRAIN");
    textout_ex(background, font, str, (i+1)*(TRAIN_W+WAGONS_SPACE), TRAIN_W + 1, red, -1);
    sprintf(str, " : MEDIUM PRIORITY TRAIN");
    textout_ex(background, font, str, (i+1)*(TRAIN_W+WAGONS_SPACE), 2*TRAIN_W + 1, blue, -1);
    sprintf(str, " : LOW PRIORITY TRAIN");
    textout_ex(background, font, str, (i+1)*(TRAIN_W+WAGONS_SPACE), 3*TRAIN_W + 1, green, -1);

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

                case NEW_RND_TRAIN:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    i++;
                    break;
                
                case NEW_HP_TRAIN:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    train_par[i].priority = 3;
                    i++;
                    break;
                    
                case NEW_MP_TRAIN:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    train_par[i].priority = 2;
                    i++;
                    break;
                
                case NEW_LP_TRAIN:
                    if (i == TMAX ) i = 1;
                    task_create(train, i, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PERIOD); 
                    train_par[i].priority = 1;
                    i++;
                    break;
                
                case TRAIN_FROM_DX:
                    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
                    ASSIGNED_DIRECTION = true;
                    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);

                    pthread_mutex_lock(&user_direction_mutex);
                    user_direction = FROM_DX;
                    pthread_mutex_unlock(&user_direction_mutex);

                    button[TRAIN_FROM_DX].button_off = from_dx_on;
                    button[TRAIN_FROM_SX].button_off = from_sx_off;
                    
                    printf("---------------------------------------------------------------------------- \n");
                    printf("Direction selected: new trains will appear from the right side \n");

                    break;

                case TRAIN_FROM_SX:
                    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
                    ASSIGNED_DIRECTION = true;
                    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);

                    pthread_mutex_lock(&user_direction_mutex);
                    user_direction = FROM_SX;
                    pthread_mutex_unlock(&user_direction_mutex);
                
                    button[TRAIN_FROM_SX].button_off = from_sx_on;
                    button[TRAIN_FROM_DX].button_off = from_dx_off;

                    printf("---------------------------------------------------------------------------- \n");
                    printf("Direction selected: new trains will appear from the left side \n");

                    break;

                case RANDOM_DIRECTION:
                    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
                    ASSIGNED_DIRECTION = false;
                    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);

                    button[TRAIN_FROM_SX].button_off = from_sx_off;
                    button[TRAIN_FROM_DX].button_off = from_dx_off;

                    printf("---------------------------------------------------------------------------- \n");                    
                    printf("Random direction restored \n");

                    break;

                case CLOSE_PROGRAM:
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

            case KEY_ESC:
                EXIT_COMMAND = true;
                break;

            default:
                break;
        }
        
        // PARTE 3: genera un treno random dopo un tot di tempo
        // DA FARE


    if(deadline_miss(id))           printf("Deadline miss of user task \n");
    wait_for_activation(id);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE *check button
//
// individua il pulsante premuto
//-------------------------------------------------------------------------------------------------------------------------
int check_button(int x, int y){
    bool    pressed;
    int     i;

    pressed = false;

    for (i = 0; i < N_BUTTONS; i++) {
        if (x < button[i].x_max && x > button[i].x_min &&
            y < button[i].y_max && y > button[i].y_min) {
                pressed = true;
                break;
            }  
    }
    if(pressed) return i;
    // DA SISTEMARE QUESTO RETURN
    else        return 12;
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
    int arrow_w;
    int arrow_h;
    int i;
    int j;
    char str[50];
    char station_number[2];
    char sem_number[2];
    char queue_list[100];

    id = get_task_id(p);
    set_activation(id);

    sem_w = sem_r->w*SEM_SIZE_FACTOR;
    sem_h = sem_r->h*SEM_SIZE_FACTOR;
    arrow_w = green_arrow_dx->w*ARROW_SIZE_FACTOR;
    arrow_h = green_arrow_dx->h*ARROW_SIZE_FACTOR;

    while(EXIT == false) {

        blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
        blit(interface, interface_buffer, 0, 0, 0, 0, interface->w, interface->h);

        // SEMAFORI DEGLI INCROCI
        for (i = 0; i < SEMAPHORES_NUM; i++){

            pthread_mutex_lock(&semaphores[i].mutex);
            if (semaphores[i].status == false)      semaphores[i].sem = sem_r;
            else                                    semaphores[i].sem = sem_g;
            stretch_sprite(buffer, semaphores[i].sem, semaphores[i].xPointDraw - sem_w/2, semaphores[i].yPointDraw - sem_h, sem_w, sem_h);
            pthread_mutex_unlock(&semaphores[i].mutex);

            sprintf(sem_number,"%d",i);
            textout_ex(buffer, font, sem_number, semaphores[i].xPointDraw, semaphores[i].yPointDraw + 10, (0,25,0), -1);
            
        }
        
        // SEMAFORI DELLA STAZIONE
        for (i = 0; i < STATIONS_NUM; i++){

            pthread_mutex_lock(&station[i].mutex);
            if (station[i].status == false)     station[i].sem = sem_r;
            else                                station[i].sem = sem_g;
            stretch_sprite(buffer, station[i].sem, station[i].xPointDraw - sem_w/2, station[i].yPointDraw - sem_h, sem_w, sem_h);
            pthread_mutex_unlock(&station[i].mutex);

            sprintf(station_number,"%d",i);
            textout_ex(buffer, font, station_number, station[i].xPointDraw - 10, station[i].yPointDraw + 10, (0,0,0), -1);
            
        }

        // BINARI MOBILI
        for (i = 0; i <= 2; i++){
            pthread_mutex_lock(&semaphores[i].mutex);
            pivot_sprite(buffer,trail, semaphores[i].xPointTrail, semaphores[i].yPointTrail - TRAIL_H/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointTrail, semaphores[i+6].yPointTrail - TRAIL_H/2, 0, 0, itofix(semaphores[i+6].trail_angle));
            pthread_mutex_unlock(&semaphores[i].mutex);
        }
        for (i = 3; i <= 5; i++){
            pthread_mutex_lock(&semaphores[i].mutex);
            pivot_sprite(buffer,trail, semaphores[i].xPointTrail, semaphores[i].yPointTrail + TRAIL_H/2, 0, 0, itofix(semaphores[i].trail_angle));
            pivot_sprite(buffer,trail, semaphores[i+6].xPointTrail, semaphores[i+6].yPointTrail + TRAIL_H/2, 0, 0, itofix(semaphores[i+6].trail_angle));
            pthread_mutex_unlock(&semaphores[i].mutex);
        }

        // TRENI
        for (i = 1; i < TMAX; i++){
            pthread_mutex_lock(&train_par[i].mutex);
            if (train_par[i].run == true){
                // DEBUG GRAFICO
                // line(buffer, train_par[i].stop_x, train_par[i].wagons[0].posy + TRAIN_H, train_par[i].stop_x, train_par[i].wagons[0].posy - TRAIN_H,(0,0,0));
                for (j = 0; j < WAGONS; j++){
                    draw_sprite(buffer, train_par[i].wagons[j].bmp, train_par[i].wagons[j].posx, train_par[i].wagons[j].posy);
                }
            }
            pthread_mutex_unlock(&train_par[i].mutex);
        }

        // FRECCE DIREZIONI
        pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
        pthread_mutex_lock(&user_direction_mutex);

        if (ASSIGNED_DIRECTION == false) {
            stretch_sprite(buffer, green_arrow_dx, 40, H/2 - SPACE - 20, arrow_w, arrow_h);
            stretch_sprite(buffer, green_arrow_sx, W - 40 - arrow_w, H/2 + SPACE - 20 + arrow_h, arrow_w, arrow_h);
        }

        else if (user_direction == FROM_DX) {
            stretch_sprite(buffer, red_arrow_dx, 40, H/2 - SPACE - 20, arrow_w, arrow_h);
            stretch_sprite(buffer, green_arrow_sx, W - 40 - arrow_w, H/2 + SPACE - 20 + arrow_h, arrow_w, arrow_h);
        }

        else {
            stretch_sprite(buffer, green_arrow_dx, 40, H/2 - SPACE - 20, arrow_w, arrow_h);
            stretch_sprite(buffer, red_arrow_sx, W - 40 - arrow_w, H/2 + SPACE - 20 + arrow_h, arrow_w, arrow_h);
        }

        pthread_mutex_unlock(&user_direction_mutex);
        pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);

        // INTERFACCIA
        for (i = 0; i < N_BUTTONS ; i++) {
            if (button[i].state == true)    blit(button[i].button_on, interface_buffer, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);
            else                            blit(button[i].button_off, interface_buffer, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);
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
        sprintf(str, "Queue: %d", trains_in_binary[4]);
        textout_ex(buffer, font, str, 650, 5.2*SPACE, (0,0,0),-1);
        sprintf(str, "Queue: %d", trains_in_binary[5]);
        textout_ex(buffer, font, str, 650, 6.2*SPACE, (0,0,0),-1);
        sprintf(str, "Queue: %d", trains_in_binary[6]);
        textout_ex(buffer, font, str, 650, 7.2*SPACE, (0,0,0),-1);
        sprintf(str, "Queue: %d", trains_in_binary[7]);
        textout_ex(buffer, font, str, 650, 8.2*SPACE, (0,0,0),-1);

        sprintf(str, "Queue: %d", semaphores[0].queue);
        textout_ex(buffer, font, str, semaphores[0].xPointDraw, H/2, (0,0,0),-1);
        sprintf(str, "Queue: %d", semaphores[1].queue);
        textout_ex(buffer, font, str, semaphores[1].xPointDraw, H/2, (0,0,0),-1);
        sprintf(str, "Queue: %d", semaphores[2].queue);
        textout_ex(buffer, font, str, semaphores[2].xPointDraw, H/2, (0,0,0),-1);


        // STAMPA I DL MISS SU SCHERMO
        sprintf(str, "GRAPHIC TASK DL: %d", tp[GRAPHIC_TASK_ID].dmiss);
        textout_ex(interface_buffer, font, str, 800, 10, (0,0,0), -1);
        sprintf(str, "USER TASK DL: %d", tp[USER_TASK_ID].dmiss);
        textout_ex(interface_buffer, font, str, 800, 30, (0,0,0), -1);
        sprintf(str, "STATION MANAGER TASK DL: %d", tp[STATION_MANAGER_TASK_ID].dmiss);
        textout_ex(interface_buffer, font, str, 800, 50, (0,0,0), -1);
        sprintf(str, "TOTAL TRAIN TASK DL: %d", total_train_dl);
        textout_ex(interface_buffer, font, str, 800, 70, (0,0,0), -1);

        // int k = 0;
        // sprintf(sem_number, "%d", k);
        // line(buffer, semaphores[k].xPointStop, 0,   semaphores[k].xPointStop, H, (0,0,0));
        // textout_ex(buffer, font, sem_number, semaphores[k].xPointStop + 10, semaphores[k].yPointDraw - 10, (0,25,0), -1);
        // sprintf(sem_number, "%d", k+1);
        // line(buffer, semaphores[k+1].xPointStop, 0, semaphores[k+1].xPointStop, H, (0,0,0));
        // textout_ex(buffer, font, sem_number, semaphores[k+1].xPointStop + 10, semaphores[k+1].yPointDraw - 10, (0,25,0), -1);
        // sprintf(sem_number, "%d", k+2);
        // line(buffer, semaphores[k+2].xPointStop, 0, semaphores[k+2].xPointStop, H, (0,0,0));
        // textout_ex(buffer, font, sem_number, semaphores[k+2].xPointStop + 10, semaphores[k+2].yPointDraw - 10, (0,25,0), -1);
        // sprintf(sem_number, "%d", k+3);
        // line(buffer, semaphores[k+3].xPointStop, 0, semaphores[k+3].xPointStop, H, (0,0,0));
        // textout_ex(buffer, font, sem_number, semaphores[k+3].xPointStop + 10, semaphores[k+3].yPointDraw + 10, (0,25,0), -1);


        // DISEGNO IL BUFFER SULLO SCHERMO
        blit(buffer, screen, 0, 0, 0, 0, background->w, background->h);
        blit(interface_buffer, screen, 0, 0, 0, H, interface->w, interface->h);
        show_mouse(screen);

        // DEADLINE MISS
        if(deadline_miss(id))       printf("Deadline miss of graphic task \n");     
        wait_for_activation(id);
    }

    // DA AGGIUNGERE TUTTI I BITMPAS DA ELIMINARE
    destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(sem_r);
    destroy_bitmap(sem_g);
    destroy_bitmap(trail);
    destroy_bitmap(platform);
        for (i = 0; i < 3; i ++) {
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
    bool    run;
    bool    move_queue;
    int     trail_queue;
    int     binary;
    int     id;
    int     i;
    int     j;
    struct  timespec now;

    id = get_task_id(p);
    set_activation(id);

    while (EXIT == false){
        
        // ASSEGNAZIONE BINARI
        binary_assignment();
        //manual_binary_assignment(2);

        // GESTIONE TRENI
        // Controlla la prossima fermata di ogni treno in base al binario
        for (i = 1; i < TMAX; i++){

            pthread_mutex_lock(&train_par[i].mutex);
            run = train_par[i].run;
            pthread_mutex_unlock(&train_par[i].mutex);

            if (run == true){

                pthread_mutex_lock(&train_par[i].mutex);
                binary = train_par[i].binary;
                train_par[i].checked = false;
                pthread_mutex_unlock(&train_par[i].mutex);

                switch(binary){

                case(0):
                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_ON);
                    break;

                case(1):
                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_ON);
                    break;

                case(2):
                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_OFF);
                    checkSemaphoreIn(i, 2, TRAIL_ON);
                    break;

                case(3):
                    // check semaforo
                    checkSemaphoreIn(i, 0, TRAIL_OFF);
                    checkSemaphoreIn(i, 1, TRAIL_OFF);
                    checkSemaphoreIn(i, 2, TRAIL_OFF);
                    break;

                case(4):
                    // check semaforo
                    checkSemaphoreIn(i, 11, TRAIL_OFF);
                    checkSemaphoreIn(i, 10, TRAIL_OFF);
                    checkSemaphoreIn(i, 9, TRAIL_OFF);
                    break; 

                case(5):
                    // check semaforo
                    checkSemaphoreIn(i, 11, TRAIL_OFF);
                    checkSemaphoreIn(i, 10, TRAIL_OFF);
                    checkSemaphoreIn(i, 9, TRAIL_ON);
                    break; 

                case(6):
                    // check semaforo
                    checkSemaphoreIn(i, 11, TRAIL_OFF);
                    checkSemaphoreIn(i, 10, TRAIL_ON);
                    break; 

                case(7):
                    // check semaforo
                    checkSemaphoreIn(i, 11, TRAIL_ON);
                    break;

                default:
                    break;   
                }

                // CHECK STAZIONE
                checkStation(i);
            }
        }

        //GESTIONE CODE DEI SEMAFORI E DELLE STAZIONI
        for (i = 0; i < STATIONS_NUM; i++){

            pthread_mutex_lock(&station[i].mutex);
            move_queue = station[i].move_queue;
            pthread_mutex_unlock(&station[i].mutex);

            if (move_queue == true)     move_station_queue(i);
        }

        for (i = 0; i < SEMAPHORES_NUM; i++) {

            pthread_mutex_lock(&semaphores[i].mutex);
            trail_queue = semaphores[i].queue;
            pthread_mutex_unlock(&semaphores[i].mutex);

            if (trail_queue > 0)    move_semaphore_queue_in(i);
        }


        // checkSemaphoreOut();
        
        if(deadline_miss(id))           printf("Deadline miss of station manager task \n");
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
    int     direction;
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
                direction       = train_par[trainId].direction;
                pthread_mutex_unlock(&train_par[trainId].mutex);

                if(binary_assigned == false) {

                    pthread_mutex_lock(&trains_in_binary_mutex);
                    switch (direction)
                    {
                    case FROM_SX:
                        // Assegnamento binario per direzione SX-DX
                        switch (priority) {

                        case HIGH_PRIO:
                            if (trains_in_binary[3] <= trains_in_binary[2])         bin = 3;
                            else if (trains_in_binary[2] <= trains_in_binary[1])    bin = 2;
                            else if (trains_in_binary[1] <= trains_in_binary[0])    bin = 1;
                            else                                                    bin = 0;
                            break;

                        case MEDIUM_PRIO:
                            if (trains_in_binary[2] <= trains_in_binary[1])         bin = 2;
                            else if (trains_in_binary[1] <= trains_in_binary[0])    bin = 1;
                            else                                                    bin = 0;
                            break;

                        case LOW_PRIO:
                            if (trains_in_binary[1] <= trains_in_binary[0])         bin = 1;
                            else                                                    bin = 0;
                            break;

                        default:
                            break;
                        }
                        break;

                    case FROM_DX:
                        // Assegnamento binario per direzione DX-SX
                        switch (priority) {
                        case HIGH_PRIO:
                            if (trains_in_binary[4] <= trains_in_binary[5])         bin = 4;
                            else if (trains_in_binary[5] <= trains_in_binary[6])    bin = 5;
                            else if (trains_in_binary[6] <= trains_in_binary[7])    bin = 6;
                            else                                                    bin = 7;
                            break;

                        case MEDIUM_PRIO:
                            if (trains_in_binary[5] <= trains_in_binary[6])         bin = 5;
                            else if (trains_in_binary[6] <= trains_in_binary[7])    bin = 6;
                            else                                                    bin = 7;
                            break;

                        case LOW_PRIO:
                            if (trains_in_binary[6] <= trains_in_binary[7])         bin = 6;
                            else                                                    bin = 7;
                            break;

                        default:
                            break;
                        }
                        
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
// FUNZIONE manual_binary_assignment
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void manual_binary_assignment(int binary) {
    
    bool    run;
    bool    binary_assigned;
    int     i;
    int     bin;
    int     trainId;
    int     direction;
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
            direction       = train_par[trainId].direction;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            if(binary_assigned == false) {

                pthread_mutex_lock(&trains_in_binary_mutex);
                switch (direction) {

                case FROM_SX:
                    bin = binary;
                    break;
                case FROM_DX:
                    switch (binary) {
                    case 0:
                        bin = 7;
                        break;
                    case 1:
                        bin = 6;
                        break;
                    case 2:
                        bin = 5;
                        break;
                    case 3:
                        bin = 4;
                        break;
                    default:
                        break;
                    }
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

    bool                sem_status;
    bool                checked;
    bool                sem_passed;
    bool                semaphore_flag;
    bool                train_in_sem;
    bool                first_of_queue;
    int                 posx;
    int                 xPointIn;
    int                 xPointOut;
    int                 direction;
    int                 trail_state;

    pthread_mutex_lock(&train_par[trainId].mutex);
    posx            = train_par[trainId].posx;
    checked         = train_par[trainId].checked;
    sem_passed      = train_par[trainId].sem_passed[semId];
    direction       = train_par[trainId].direction;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    train_in_sem    = false;
    first_of_queue  = false;

    if (checked == false) {

        pthread_mutex_lock(&semaphores[semId].mutex);
        xPointIn    = semaphores[semId].xPointIn;
        xPointOut   = semaphores[semId].xPointOut;
        pthread_mutex_unlock(&semaphores[semId].mutex);

        // A seconda della direzione aggiorno il flag per indicare che mi trovo nel range del semaforo
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

        // Codice eseguito ad ogni ciclo se il treno si trova nel range del semaforo
        if (train_in_sem == true && sem_passed == false) {
            
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].checked  = true;
            semaphore_flag              = train_par[trainId].semaphore_flag;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            pthread_mutex_lock(&semaphores[semId].mutex);
            trail_state     = semaphores[semId].trail_state;
            sem_status      = semaphores[semId].status;
            pthread_mutex_unlock(&semaphores[semId].mutex);

            // Controllo se il binario non è nella poszione corretta o il semaforo è rosso, in
            // entrambi i casi il treno si deve fermare
            if (trail_state != SemRequiredState || sem_status == false ){

                pthread_mutex_lock(&train_par[trainId].mutex);
                train_par[trainId].checked = true;

                // Eseguito solo la prima volta che il treno entra nel range
                if (train_par[trainId].queue == false) {
                    
                    // Aggiorno la struttra dati del treno
                    train_par[trainId].semaphore_flag   = true;
                    train_par[trainId].queue            = true;
                    train_par[trainId].pos_in_queue     = semaphores[semId].queue;
                    train_par[trainId].stop_x           = semaphores[semId].xPointStop;
                    train_par[trainId].stop_id          = semId;
                    train_par[trainId].stop_type        = SEMAPHORE;

                    // Aggiorno la struttra dati del semaforo
                    pthread_mutex_lock(&semaphores[semId].mutex);
                    semaphores[semId].queue_list[semaphores[semId].queue]   = trainId;
                    semaphores[semId].trail_list[semaphores[semId].queue]   = SemRequiredState;
                    semaphores[semId].queue                                 += 1;

                    // Traslo i punti di ingresso e di stop a seconda della direzione
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
                pthread_mutex_unlock(&train_par[trainId].mutex); 
            }
        }
    } 
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE move_semaphore_queue_in
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void move_semaphore_queue_in(int semId) {
    int i;
    int direction;
    int SemRequiredState;
    int trail_state;
    int trainId;
    int queue;
    
    if (semId <= 2)         direction = FROM_SX;
    else                    direction = FROM_DX;

    pthread_mutex_lock(&semaphores[semId].mutex);
    SemRequiredState    = semaphores[semId].trail_list[0];
    trail_state         = semaphores[semId].trail_state;
    pthread_mutex_unlock(&semaphores[semId].mutex);

    if (SemRequiredState != trail_state) {
        pthread_mutex_lock(&semaphores[semId].mutex);

        // Il semaforo diveta rosso
        semaphores[semId].status = false;

        switch(direction) {

            case FROM_DX:
                if (SemRequiredState == TRAIL_ON) {
                    semaphores[semId].trail_angle -= TRAIL_ANGLE_INC;
                    if (semaphores[semId].trail_angle == TRAIL_DOWN_BIN_IN_SWITCH_ON) {
                        semaphores[semId].trail_state = TRAIL_ON;
                    }
                }
                else if (SemRequiredState == TRAIL_OFF) {
                    semaphores[semId].trail_angle += TRAIL_ANGLE_INC;
                    if (semaphores[semId].trail_angle == TRAIL_DOWN_BIN_IN_SWITCH_OFF) {
                        semaphores[semId].trail_state = TRAIL_OFF;
                    }
                }
                break;

            case FROM_SX:
                if      (SemRequiredState == TRAIL_ON) {
                    semaphores[semId].trail_angle -= TRAIL_ANGLE_INC;
                    if (semaphores[semId].trail_angle == TRAIL_UP_BIN_IN_SWITCH_ON) {
                        semaphores[semId].trail_state = TRAIL_ON;
                    }
                }
                else if (SemRequiredState == TRAIL_OFF) {
                    semaphores[semId].trail_angle += TRAIL_ANGLE_INC;
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
    else {

        pthread_mutex_lock(&semaphores[semId].mutex);
        semaphores[semId].status    = true;
        trainId                     = semaphores[semId].queue_list[0];
        queue                       = semaphores[semId].queue;
        pthread_mutex_unlock(&semaphores[semId].mutex);

        pthread_mutex_lock(&train_par[trainId].mutex);
        train_par[trainId].sem_passed[semId]    = true;
        train_par[trainId].queue                = false;
        pthread_mutex_unlock(&train_par[trainId].mutex);

        for (i = 1; i <= queue; i++) {
            
            // Traslo la coda dei treni e della lista di posizioni del binario richieste
            pthread_mutex_lock(&semaphores[semId].mutex);
            semaphores[semId].queue_list[i-1] = semaphores[semId].queue_list[i];
            semaphores[semId].trail_list[i-1] = semaphores[semId].trail_list[i];
            trainId = semaphores[semId].queue_list[i-1];
            pthread_mutex_unlock(&semaphores[semId].mutex);

            // Segnalo che i treni in coda si possono muovere
            if (trainId != 0) {
                pthread_mutex_lock(&train_par[trainId].mutex);
                train_par[trainId].ready_to_go_flag = true;
                pthread_mutex_unlock(&train_par[trainId].mutex);
            }

        }

        // Aggiorno la struttra del semaforo dopo il movimento della coda
        pthread_mutex_lock(&semaphores[semId].mutex);
        semaphores[semId].move_queue   = false;
        semaphores[semId].queue        --;  

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
// FUNZIONE checkSemaphoreOut
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void checkSemaphoreOut(){
    // cerco il treno con il massimo della priorità tra i treni pronti per uscire dalla stazione
    // assegno alla variabile bin il numero del binario che rispetta la condizione sopra
    // faccio i vari casi in base al bin
    int maxprioId = 0;
    int maxprio = 0;
    int bin = 0;
    int t = 0;
    // bool train_found = false;
    int     xPointIn_sem;
    int     xPointOut_sem;

    if (!max_prio_train_found) {
        printf("searching max prio train\n");
        for (int i = 1; i < TMAX; i++){
            if (train_par[i].run){
                printf("train %d on binary %d is checked: %d pos_queue %d\n",i, train_par[i].binary, train_par[i].checked, train_par[i].pos_in_queue);  
                printf("station %d queue: %d\n", train_par[i].binary, station[train_par[i].binary].queue); 
                if ((train_par[i].checked == true) && (train_par[i].pos_in_queue == 0)){
                    if(train_par[i].priority > maxprio){
                        maxprio = train_par[i].priority;
                        max_prio_train_id = i;
                        max_prio_train_found = true;
                    }
                }
            }
        }
        printf("max prio is %d\n", max_prio_train_id);
        if (station[train_par[max_prio_train_id].binary].status == false) {
            station[train_par[max_prio_train_id].binary].status = true;
        }
    }
    
    maxprioId = max_prio_train_id;
    if (max_prio_train_found == true){
        bin = train_par[maxprioId].binary;
        t = abs(bin-5);
        pthread_mutex_lock(&semaphores[t].mutex);
        xPointIn_sem       = station[bin].xPointOut ;
        xPointOut_sem      = semaphores[t].xPointOut;
        pthread_mutex_unlock(&semaphores[t].mutex);

        switch (bin){

            case(0):
                if ((train_par[maxprioId].posx >= xPointIn_sem) && (train_par[maxprioId].posx <= xPointOut_sem)){
                    semaphores[4].status = false;
                    semaphores[3].status = false;
                    station[3].status = false;
                };
                semaphores[5].trail_angle += semaphores[5].trail_angle_inc;
                if (semaphores[5].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
                }; 
                if (train_par[maxprioId].posx >= W){
                    semaphores[4].status = true;
                    semaphores[3].status = true;
                    station[3].status = true;
                    max_prio_train_found = false;
                    train_par[maxprioId].checked = false;
                };
                break;
            case(1):

                if ((train_par[maxprioId].posx >= xPointIn_sem) && (train_par[maxprioId].posx <= xPointOut_sem)){
                    semaphores[5].status = false;
                    semaphores[3].status = false;
                    station[3].status = false;
                }
                semaphores[4].trail_angle += semaphores[4].trail_angle_inc;
                if (semaphores[4].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {
                    semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
                }; 
                semaphores[5].trail_angle -= semaphores[5].trail_angle_inc;
                if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                };  
                if (train_par[maxprioId].posx >= W){
                    semaphores[5].status = true;
                    semaphores[3].status = true;
                    station[3].status = true;
                    max_prio_train_found = false;
                    train_par[maxprioId].checked = false;
                };
                break;
            case(2): 
                if ((train_par[maxprioId].posx >= xPointIn_sem) && (train_par[maxprioId].posx <= xPointOut_sem)){
                    semaphores[5].status = false;
                    semaphores[4].status = false;
                    station[3].status = false;
                };
                semaphores[3].trail_angle += semaphores[3].trail_angle_inc;
                if (semaphores[3].trail_angle > TRAIL_UP_BIN_OUT_SWITCH_ON) {
                    semaphores[3].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_ON;
                }; 
                semaphores[4].trail_angle -= semaphores[4].trail_angle_inc;
                if (semaphores[4].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }; 
                semaphores[5].trail_angle -= semaphores[5].trail_angle_inc;
                if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                };  
                if (train_par[maxprioId].posx >= W){
                    semaphores[5].status = true;
                    semaphores[4].status = true;
                    station[3].status = true;
                    max_prio_train_found = false;
                    train_par[maxprioId].checked = false;
                };
                break;
            case(3):
                if ((train_par[maxprioId].posx >= station[bin].xPointIn) && (train_par[maxprioId].posx <= semaphores[bin].xPointOut)){
                    semaphores[5].status = false;
                    semaphores[4].status = false;
                    semaphores[3].status = false;
                };
                semaphores[3].trail_angle -= semaphores[3].trail_angle_inc;
                if (semaphores[3].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[3].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }; 
                semaphores[4].trail_angle -= semaphores[4].trail_angle_inc;
                if (semaphores[4].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[4].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }; 
                semaphores[5].trail_angle -= semaphores[5].trail_angle_inc;
                if (semaphores[5].trail_angle < TRAIL_UP_BIN_OUT_SWITCH_OFF) {
                    semaphores[5].trail_angle = TRAIL_UP_BIN_OUT_SWITCH_OFF;
                }; 
                if (train_par[maxprioId].posx >= W ){
                    semaphores[5].status = true;
                    semaphores[4].status = true;
                    semaphores[3].status = true;
                    max_prio_train_found = false;
                    train_par[maxprioId].checked = false;
                };
                break;          
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE CheckStation
// 
// 
//-------------------------------------------------------------------------------------------------------------------------
void checkStation(int trainId) {
    bool    station_status;
    bool    checked;
    bool    station_passed;
    bool    semaphore_flag;
    bool    train_in_station;
    struct  timespec    now;
    struct  timespec    leave_time;
    int     stationId;
    int     posx;
    int     xPointIn;
    int     xPointOut;
    int     direction;
    
    pthread_mutex_lock(&train_par[trainId].mutex);
    stationId       = train_par[trainId].binary;
    posx            = train_par[trainId].posx;
    checked         = train_par[trainId].checked;
    station_passed  = train_par[trainId].station_passed[stationId];
    direction       = train_par[trainId].direction;
    pthread_mutex_unlock(&train_par[trainId].mutex);

    train_in_station = false;

    if (checked == false) {

        pthread_mutex_lock(&station[stationId].mutex);
        xPointIn    = station[stationId].xPointIn;
        xPointOut   = station[stationId].xPointOut;
        pthread_mutex_unlock(&station[stationId].mutex);

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

        if (train_in_station == true && station_passed == false) {
            
            // Flag per evitare di controllare tutti i semafori successivi
            pthread_mutex_lock(&train_par[trainId].mutex);
            train_par[trainId].checked  = true;
            semaphore_flag              = train_par[trainId].semaphore_flag;
            pthread_mutex_unlock(&train_par[trainId].mutex);

            // Eseguito solo la prima volta che il treno entra nel range della stazione
            if (semaphore_flag == false){
                pthread_mutex_lock(&train_par[trainId].mutex);

                // Flag per comunicare che il treno è in stazione
                train_par[trainId].semaphore_flag = true;

                // Eseguito la prima volta e ogni volta che la coda si muove
                if (train_par[trainId].queue == false) {
                    pthread_mutex_lock(&station[stationId].mutex);

                    // Salvo la posizione in coda del treno
                    train_par[trainId].pos_in_queue = station[stationId].queue;
                    
                    // Salvo la posizione in cui mi dovrò fermare
                    train_par[trainId].stop_x       = station[stationId].xPointStop;
                    train_par[trainId].stop_id      = stationId;
                    train_par[trainId].stop_type    = STATION;

                    // Inserisco il treno in coda
                    station[stationId].queue_list[station[stationId].queue] = trainId;
                    station[stationId].queue ++;
                    
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

                // Salvo l'istante in cui il semaforo diventa rosso
                clock_gettime(CLOCK_MONOTONIC, &now);                

                // Aggiorno la struttura della stazione
                pthread_mutex_lock(&station[stationId].mutex);
                time_copy(&station[stationId].green_time, now);
                time_add_ms(&station[stationId].green_time, STOP_TIME);
                station[stationId].status = false;
                pthread_mutex_unlock(&station[stationId].mutex);
            }

            // SEMAFORO ROSSO
            pthread_mutex_lock(&station[stationId].mutex);
            station_status = station[stationId].status;
            pthread_mutex_unlock(&station[stationId].mutex);

            if (station_status == false) {

                // Timer per far tornare il semaforo della stazione verde
                clock_gettime(CLOCK_MONOTONIC, &now);

                pthread_mutex_lock(&station[stationId].mutex);
                time_copy(&leave_time, station[stationId].green_time);
                pthread_mutex_unlock(&station[stationId].mutex);

                if (time_cmp(now, leave_time) == 1) {
                    
                    // Il semaforo diventa verde e segno l'istante in cui deve tornare rosso se c'è una coda
                    pthread_mutex_lock(&station[stationId].mutex);
                    station[stationId].status = true;
                    pthread_mutex_unlock(&station[stationId].mutex);

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
            train_par[trainId].ready_to_go_flag = true;
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
            time_add_ms(&ready_time, MS_BETWEEN_TRAINS);
            k = time_cmp(now, ready_time);
            if ( k == 1)    time_copy(&last_assigned_train_from_dx, now);
            pthread_mutex_unlock(&last_assigned_train_from_dx_mutex);
                break;
            
            case FROM_SX:
            pthread_mutex_lock(&last_assigned_train_from_sx_mutex);
            time_copy(&ready_time, last_assigned_train_from_sx);
            time_add_ms(&ready_time, MS_BETWEEN_TRAINS);
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
                //  printf("GO FAST \n");
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
                //  printf("SLOW DOWN \n");
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
                //  printf("QUEUE \n");         
                // MOVIMENTO DELLA CODA DEL SEMAFORO
                pthread_mutex_lock(&train_par[id].mutex);
                if (train_par[id].ready_to_go_flag == true) {

                    train_par[id].ready_to_go_flag  = false;
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
                //  printf("STOP \n");
                
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
                //  printf("SPEED UP \n");
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
    train_par[i].ready_to_go_flag   = false; 
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
    
    train_par[i].count                  = 0;
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
// FUNZIONE get_scancode
//
// funzione che da come risultato il tasto premuto, se ne è stato 
// premuto uno, altrimenti da come risultato 0
//----------------------------------------------------------------------------
char get_scancode() {
    
    if(keypressed()) {
        return readkey() >> 8;
    }
    else{
        return 0;
    }    
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