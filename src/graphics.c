#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "init.h"
#include "ptask.h"
#include "graphics.h"

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE graphics_init()
//--------------------------------------------------------------------------------------------------------------------------------------------

void graphics_init(){
    int     i;
    int     black, red, blue, green, white, grey;
    int     x0, x1, x2, x3, y0, y1, y2, y3;
    int     trail_diag;
    char    str[50];

     // INIT ALLEGRO
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(15);
    black   = makecol(0, 0, 0);
    red     = makecol(0, 0, 255);
    blue    = makecol(165, 165, 0);
    green   = makecol(0, 255, 0);
    white   = makecol(255, 255,255);
    grey    = makecol(50, 50, 50);

    // LOADING DEI BITMAPS
    loading_bitmaps();

    // CREAZIONE INTERFACCIA
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, WINDOW_H, 0, 0);
    background          = create_bitmap(W, H);
    buffer              = create_bitmap(W, H);
    interface           = create_bitmap(INTERFACE_W, INTERFACE_H);
    interface_buffer    = create_bitmap(INTERFACE_W, INTERFACE_H);
    clear_to_color(background, grey);
    clear_to_color(interface, grey);
    rectfill(background, 0, H-1, W-1, H-10, black);

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
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE loading_bitmaps()
//--------------------------------------------------------------------------------------------------------------------------------------------

void loading_bitmaps() {

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
    random_train_on     = load_bitmap("img/buttons/random_on.bmp", NULL);
    random_train_off    = load_bitmap("img/buttons/random_off.bmp", NULL);

    hp_train_on         = load_bitmap("img/buttons/hp_on.bmp", NULL);
    hp_train_off        = load_bitmap("img/buttons/hp_off.bmp", NULL);

    mp_train_off        = load_bitmap("img/buttons/mp_off.bmp", NULL);
    mp_train_on         = load_bitmap("img/buttons/mp_on.bmp", NULL);

    lp_train_off        = load_bitmap("img/buttons/lp_off.bmp", NULL);
    lp_train_on         = load_bitmap("img/buttons/lp_on.bmp", NULL);

    from_dx_off         = load_bitmap("img/buttons/from_dx_off.bmp", NULL);
    from_dx_on          = load_bitmap("img/buttons/from_dx_on.bmp", NULL);

    from_sx_off         = load_bitmap("img/buttons/from_sx_off.bmp", NULL);
    from_sx_on          = load_bitmap("img/buttons/from_sx_on.bmp", NULL);

    rnd_direction_off   = load_bitmap("img/buttons/rnd_direction_off.bmp", NULL);
    rnd_direction_on    = load_bitmap("img/buttons/rnd_direction_on.bmp", NULL);

    auto_mode_off       = load_bitmap("img/buttons/auto_mode_off.bmp", NULL);
    auto_mode_on        = load_bitmap("img/buttons/auto_mode_on.bmp", NULL);
    
    close_program_off   = load_bitmap("img/buttons/exit_off.bmp", NULL);
    close_program_on    = load_bitmap("img/buttons/exit_on.bmp", NULL);

    // BITMAPS FRECCE
    green_arrow_dx      = load_bitmap("img/arrows/green_arrow_dx.bmp", NULL);
    green_arrow_sx      = load_bitmap("img/arrows/green_arrow_sx.bmp", NULL);
    red_arrow_dx        = load_bitmap("img/arrows/red_arrow_dx.bmp", NULL);
    red_arrow_sx        = load_bitmap("img/arrows/red_arrow_sx.bmp", NULL);
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE draw_semaphores()
//-------------------------------------------------------------------------------------------------------------------------

void draw_semaphores(int sem_w, int sem_h){
    int i;

    for (i = 0; i < 3; i++){
        
        // A seconda dello status dei semafori, aggiorna il bitmap nella struttra del semaforo
        pthread_mutex_lock(&semaphores[i].mutex);
        if (semaphores[i].status == false)      semaphores[i].sem = sem_r;      // Semaforo rosso
        else                                    semaphores[i].sem = sem_g;      // Semaforo verde
        stretch_sprite(buffer, semaphores[i].sem, semaphores[i].xPointDraw - sem_w/2, semaphores[i].yPointDraw - sem_h, sem_w, sem_h);
        pthread_mutex_unlock(&semaphores[i].mutex);
  
        // Stessa procedura per i semafori lato DX
        pthread_mutex_lock(&semaphores[i+9].mutex);
        if (semaphores[i+9].status == false)        semaphores[i+9].sem = sem_r;
        else                                        semaphores[i+9].sem = sem_g;
        stretch_sprite(buffer, semaphores[i+9].sem, semaphores[i+9].xPointDraw - sem_w/2, semaphores[i+9].yPointDraw - sem_h, sem_w, sem_h);
        pthread_mutex_unlock(&semaphores[i+9].mutex);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE draw_stations()
//-------------------------------------------------------------------------------------------------------------------------

void draw_stations(int sem_w, int sem_h){
    int i;
    
    // A seconda dello status dei semafori delle stazioni, aggiorna il bitmap nella struttra del semaforo
    for (i = 0; i < STATIONS_NUM; i++){     
        pthread_mutex_lock(&station[i].mutex);
        if (station[i].status == false)     station[i].sem = sem_r;     // Semaforo rosso
        else                                station[i].sem = sem_g;     // Semaforo verde
        stretch_sprite(buffer, station[i].sem, station[i].xPointDraw - sem_w/2, station[i].yPointDraw - sem_h, sem_w, sem_h);
        pthread_mutex_unlock(&station[i].mutex);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE draw_trails()
//-------------------------------------------------------------------------------------------------------------------------

void draw_trails(){
    int i;

    // Ruota i bitmaps dei binari mobili della quantità definita nel campo "trail_angle" della corrispettiva struttura
    for (i = 0; i < SEMAPHORES_NUM; i++) {
        pthread_mutex_lock(&semaphores[i].mutex); 
        pivot_sprite(buffer,trail, semaphores[i].xPointTrail, semaphores[i].yPointTrail, 0, 0, itofix(semaphores[i].trail_angle));
        pthread_mutex_unlock(&semaphores[i].mutex);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE draw_trains()
//-------------------------------------------------------------------------------------------------------------------------

void draw_trains(){
    int i;
    int j;

    for (i = 1; i < TMAX; i++){
        pthread_mutex_lock(&train_par[i].mutex);

        // Se il treno è attivo, disegna il bitmap, presente nella struttura di ogni vagone, nella posizione corrente, 
        // anch'essa definita allinterno della struttura
        if (train_par[i].run == true){
            for (j = 0; j < WAGONS; j++){
                draw_sprite(buffer, train_par[i].wagons[j].bmp, train_par[i].wagons[j].posx, train_par[i].wagons[j].posy);
            }
        }
        pthread_mutex_unlock(&train_par[i].mutex);
    }
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE draw_arrows()
//-------------------------------------------------------------------------------------------------------------------------

void draw_arrows(int arrow_w, int arrow_h){

    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
    pthread_mutex_lock(&user_direction_mutex);

    // Se non è stata assegnata nessuna direzione dall'utente, entrambe le frecce sono verdi
    if (ASSIGNED_DIRECTION == false) {
        stretch_sprite(buffer, green_arrow_dx, 40, H/2 - SPACE - 20, arrow_w, arrow_h);
        stretch_sprite(buffer, green_arrow_sx, W - 40 - arrow_w, H/2 + SPACE - 20 + arrow_h, arrow_w, arrow_h);
    }

    // Quando viene assegnata una direzione, la freccia corrispettiva resta verde, la freccia opposta diventa rossa
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
}

//-------------------------------------------------------------------------------------------------------------------------
// FUNZIONE draw_interface()
//-------------------------------------------------------------------------------------------------------------------------

void draw_interface(){
    int i;
    char str[100];

    // Aggiorno i bitmaps dei pulsanti in base al loro status
    for (i = 0; i < N_BUTTONS ; i++) {
        if (button[i].state == true)    blit(button[i].button_on, interface_buffer, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);
        else                            blit(button[i].button_off, interface_buffer, 0, 0, button[i].x_min, button[i].y_min, L_BUTTONS, L_BUTTONS);
    }

    // Stampa dei DL miss in un riquadro dell'interfaccia 
    sprintf(str, "GRAPHIC TASK DL: %d", tp[GRAPHIC_TASK_ID].dmiss);
    textout_ex(interface_buffer, font, str, 800, 10, (0,0,0), -1);
    sprintf(str, "USER TASK DL: %d", tp[USER_TASK_ID].dmiss);
    textout_ex(interface_buffer, font, str, 800, 30, (0,0,0), -1);
    sprintf(str, "STATION MANAGER TASK DL: %d", tp[STATION_MANAGER_TASK_ID].dmiss);
    textout_ex(interface_buffer, font, str, 800, 50, (0,0,0), -1);
    sprintf(str, "TOTAL TRAIN TASK DL: %d", total_train_dl);
    textout_ex(interface_buffer, font, str, 800, 70, (0,0,0), -1);
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

    id = get_task_id(p);
    set_activation(id);

    // Imposta i fattori di scala dei bitmaps associati a semafori e frecce direzionali
    sem_w = sem_r->w*SEM_SIZE_FACTOR;
    sem_h = sem_r->h*SEM_SIZE_FACTOR;
    arrow_w = green_arrow_dx->w*ARROW_SIZE_FACTOR;
    arrow_h = green_arrow_dx->h*ARROW_SIZE_FACTOR;

    while(EXIT == false) {
        
        // COPIA DEI DUE SCHERMI SU DUE BUFFER
        blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
        blit(interface, interface_buffer, 0, 0, 0, 0, interface->w, interface->h);

        // AGGIORNAMENTO DEI SEMAFORI DEGLI INCROCI
        draw_semaphores(sem_w, sem_h);
        
        // AGGIORNAMENTO DEI SEMAFORI DELLA STAZIONE
        draw_stations(sem_w, sem_h);

        // AGGIORNAMENTO DEI BINARI MOBILI
        draw_trails();

        // AGGIORNAMENTO DEI TRENI
        draw_trains();

        // AGGIORNAMENTO DELLE FRECCE DIREZIONI
        draw_arrows(arrow_w, arrow_h);

        // AGGIORNAMENTO DELL'INTERFACCIA
        draw_interface();

        // DISEGNO I DUE BUFFER SUGLI SCHERMI
        blit(buffer, screen, 0, 0, 0, 0, background->w, background->h);
        blit(interface_buffer, screen, 0, 0, 0, H, interface->w, interface->h);
        show_mouse(screen);

        // DEADLINE MISS
        if(deadline_miss(id))       printf("Deadline miss of graphic task \n");     
        wait_for_activation(id);
    }

    printf("Closing graphics...\n");
    ptask_exit(id);
}
