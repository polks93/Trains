#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "init.h"
#include "ptask.h"
#include "functions.h"
#include "user.h"

//-------------------------------------------------------------------------------------------------------------------------
// TASK user_task
// 
// Gestisce mouse, tastiera e si occupa della generazione dei task treno
//-------------------------------------------------------------------------------------------------------------------------
void    *user_task(void *p) {

    char    scan;
    int     id;

    id = get_task_id(p);
    set_activation(id);

    while(EXIT == false){
        
        // Check per resettare gli ID dei nuovi treni dopo aver superato il valore massimo
        if (next_trainId > TMAX)   next_trainId = 1;

        // PARTE 1: lettura comandi da mouse e interfaccia
        read_commands_from_mouse();
        
        // PARTE 2: lettura comandi da tastiera
        read_commands_from_keyboard();
        
        // PARTE 3: modalità autonoma che genera un treno random dopo 2 secondi dall'ultimo treno
        pthread_mutex_lock(&AUTO_MODE_ON_MUTEX);
        if (AUTO_MODE_ON == true)       autonomous_mode();
        pthread_mutex_unlock(&AUTO_MODE_ON_MUTEX);

    // Check dei DL miss del task utente. Oltre al print viene incrementato il counter dei DL miss associato al task
    if(deadline_miss(id))           printf("Deadline miss of user task \n");

    // Attesa fino alla nuova attivazione
    wait_for_activation(id);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE read_commands_from_mouse()
//--------------------------------------------------------------------------------------------------------------------------------------------

void read_commands_from_mouse() {
    int     mbutton;
    int     x;
    int     y;
    int     pressed_button;
    int     i;

    // mbutton vale 1 se viene registrato un click del pulsante SX
    mbutton = mouse_b & 1;

    if (mbutton) {

        // Coordinate del click del mouse nella finestra dell'interfaccia
        x = mouse_x;
        y = mouse_y - H;

        // pressed button corrisponde all'ID del pulsante premuto, se ne è stato premuto uno
        pressed_button = check_button(x, y);
        
        // Azioni che vengono eseguite solo al primo click rilevato di un pulsante, in modo da evitare di ripetere
        // le operazioni se l'utente non rilascia il pulsante del mouse
        if (button[pressed_button].state == false) {
            switch (pressed_button) {

            case NEW_RND_TRAIN:         // Generazione di un treno a priorità random
                task_create(train, next_trainId, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PRIO); 
                next_trainId++;
                break;
            
            case NEW_HP_TRAIN:          // Generazione di un treno a alta priorità
                task_create(train, next_trainId, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PRIO); 
                train_par[next_trainId].priority = 3;
                next_trainId++;
                break;
                
            case NEW_MP_TRAIN:          // Generazione di un treno a media priorità
                task_create(train, next_trainId, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PRIO); 
                train_par[next_trainId].priority = 2;
                next_trainId++;
                break;
            
            case NEW_LP_TRAIN:          // Generazione di un treno a bassa priorità
                task_create(train, next_trainId, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PRIO); 
                train_par[next_trainId].priority = 1;
                next_trainId++;
                break;
            
            case TRAIN_FROM_DX:         // Set direzione manuale dei nuovi treni a DX
                set_manual_direction(FROM_DX);
                break;

            case TRAIN_FROM_SX:         // Set direzione manuale dei nuovi treni a SX
                set_manual_direction(FROM_SX);
                break;

            case RANDOM_DIRECTION:      // Reset direzione random dei nuovi treni
                reset_random_direction();
                break;

            case AUTO_MODE:             // Attivazione/disattivazione modalità automatica di generazione dei treni
                auto_mode_manager();
                break;

            case CLOSE_PROGRAM:         // Uscita dal programma
                EXIT_COMMAND = true;
                break;

            default:
                break;
            }
            
            // Flag per indicare che il pulsante è premuto
            button[pressed_button].state = true;
        }
    }

    // Se nessun pulsante viene premuto, reset di tutte le flag dei pulsanti
    else    for (i = 0; i < N_BUTTONS; i++)     button[i].state = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE set_maunal_direction()
//--------------------------------------------------------------------------------------------------------------------------------------------

void set_manual_direction(int assigned_direction){
    
    // Variabile globale che indica che l'utente ha definito una direzione
    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
    ASSIGNED_DIRECTION = true;
    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);

    // Variabile globale che inidca la direzione definita dall'utente (FROM_DX/FROM_SX)
    pthread_mutex_lock(&user_direction_mutex);
    user_direction = assigned_direction;
    pthread_mutex_unlock(&user_direction_mutex);

    switch (assigned_direction) {

        case FROM_DX:
            button[TRAIN_FROM_DX].button_off = from_dx_on;
            button[TRAIN_FROM_SX].button_off = from_sx_off;
                
            printf("---------------------------------------------------------------------------- \n");
            printf("RIGHT DIRECTION SELECTED: new trains will appear only from the right side \n");
            break;

        case FROM_SX:
            button[TRAIN_FROM_SX].button_off = from_sx_on;
            button[TRAIN_FROM_DX].button_off = from_dx_off;

            printf("---------------------------------------------------------------------------- \n");
            printf("LEFT DIRECTION SELECTED: new trains will appear only from the left side \n");
            break;

        default:
            break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE reset_random_direction()
//--------------------------------------------------------------------------------------------------------------------------------------------

void reset_random_direction(){

    // Variabile globale che indica che l'utente NON ha definito una direzione
    pthread_mutex_lock(&ASSIGNED_DIRECTION_MUTEX);
    ASSIGNED_DIRECTION = false;
    pthread_mutex_unlock(&ASSIGNED_DIRECTION_MUTEX);

    button[TRAIN_FROM_SX].button_off = from_sx_off;
    button[TRAIN_FROM_DX].button_off = from_dx_off;

    printf("---------------------------------------------------------------------------- \n");                    
    printf("RANDOM DIRECTION RESTORED: new trains will appear from a random direction \n");
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE auto_mode_manager()
//--------------------------------------------------------------------------------------------------------------------------------------------

void auto_mode_manager() {

    // Cambia il valore della variabile globale associata alla modalità autonoma
    pthread_mutex_lock(&AUTO_MODE_ON_MUTEX);
    if (AUTO_MODE_ON == false) {
        AUTO_MODE_ON = true;
        button[AUTO_MODE].button_off = auto_mode_on;
        printf("---------------------------------------------------------------------------- \n");
        printf("AUTONOMOUS MODE ACTIVATED: new trains will be automatically generated after 2 second from the last one \n");
    }
    else {
        AUTO_MODE_ON = false;
        button[AUTO_MODE].button_off = auto_mode_off;
        printf("---------------------------------------------------------------------------- \n");
        printf("MANUAL MODE RESTORED: new trains will be generated only by the user \n");
    }
    pthread_mutex_unlock(&AUTO_MODE_ON_MUTEX);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE read_commands_from_keyboard()
//--------------------------------------------------------------------------------------------------------------------------------------------

void read_commands_from_keyboard(){
    char scan;

    // Variabile char che contiene il codice alfanumerico corrispondete ai pulsanti della tastiera
    scan = get_scancode();

    switch(scan) {

        case KEY_SPACE:         // Premendo SPACE viene generato un nuovo treno random
            task_create(train, next_trainId, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PRIO); 
            next_trainId++;
            break;

        case KEY_ESC:           // Premendo ESC si chiude il programma
            EXIT_COMMAND = true;
            break;

        default:
            break;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE get_scancode
//--------------------------------------------------------------------------------------------------------------------------------------------
char get_scancode() {
    
    if(keypressed()) {
        return readkey() >> 8;
    }
    else{
        return 0;
    }    
}
//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE autonomous_mode()
//--------------------------------------------------------------------------------------------------------------------------------------------
void autonomous_mode() {
    struct  timespec    now;
    struct  timespec    last_assigned_train;
    
    pthread_mutex_lock(&last_assigned_train_from_dx_mutex);
    pthread_mutex_lock(&last_assigned_train_from_sx_mutex);
    
    // Copia in "last_assigned_train" l'istante temporale di valore maggiore tra quello realitvo
    // all'ultimo treno generato da dx o all'ultimo treno generato da sx
    if (time_cmp(last_assigned_train_from_dx, last_assigned_train_from_sx) > 0) {
        time_copy(&last_assigned_train, last_assigned_train_from_dx);
    }
    else    time_copy(&last_assigned_train, last_assigned_train_from_sx);

    pthread_mutex_unlock(&last_assigned_train_from_dx_mutex);
    pthread_mutex_unlock(&last_assigned_train_from_sx_mutex);

    // Aggiunge 2000 MS alla struttura temporale "last_assigned_train", quando l'istante corrente "now" supera questo valore
    // viene generato un nuovo treno random
    clock_gettime(CLOCK_MONOTONIC, &now);
    time_add_ms(&last_assigned_train, MAX_MS_BETWEEN_TRAINS);

    if (time_cmp(now, last_assigned_train) > 0) {
        task_create(train, next_trainId, TRAIN_TASK_PERIOD, TRAIN_TASK_DL, TRAIN_TASK_PRIO);
        next_trainId++;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE check_button()
//--------------------------------------------------------------------------------------------------------------------------------------------
int check_button(int x, int y){
    bool    pressed;
    int     i;

    // Flag che viene impostata al valore "true" solo quando il click corrisponde alla posizione di un pulsante
    pressed = false;

    for (i = 0; i < N_BUTTONS; i++) {
        if (x < button[i].x_max && x > button[i].x_min &&
            y < button[i].y_max && y > button[i].y_min) {
                pressed = true;
                break;
            }  
    }
    if(pressed) return i;

    else        return  N_BUTTONS;
}