#include <stdbool.h>
#include "ptask.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_h

#define     TMAX                                100     // Numero massimo di treni
#define     stationsManagerTaskId               TMAX
#define     graphicTaskId                       TMAX + 1
#define     userTaskId                          TMAX + 2
#define     track_pieces                        12      // Numero di scambi dei binari
#define     STATIONS_NUM                        8
#define     MAX_TRAINS_IN_QUEUE                 2
#define     RAILWAYS_SWITCHES_SEMAPHORES        12
#define     WAGONS                              2
#define     N_BUTTONS                           10
#define     W                                   1280          //larghezza della finestra
#define     H                                   630            //altezza delle finestra
#define     INTERFACE_H                         90
#define     INTERFACE_W                         1280
#define     L_BUTTONS                           50
#define     SPACE_BUTTONS                       20
#define     H_window                            720
#define     space                               H/9            //spazio tra un binario e l'altro
#define     train_w                             20            //larghezza treno
#define     train_h                             10             //altezza treno
#define     sem_size_factor                     0.1
#define     trail_w                             80
#define     trail_h                             2
#define     TRAIL_UP_BIN_IN_SWITCH_ON_ANGLE     -32
#define     TRAIL_UP_BIN_IN_SWITCH_OFF_ANGLE    0
#define     TRAIL_UP_BIN_OUT_SWITCH_ON_ANGLE    -96
#define     TRAIL_UP_BIN_OUT_SWITCH_OFF_ANGLE   128
#define     stop_space                          train_w

enum train_states{
    GO_FAST     = 0, //0: -> andatura normale
    SLOW_DOWN   = 1, //1: -> dentro stazione
    SPEED_UP    = 2, //2: -> fuori dalla stazione
    WAIT        = 3, //3: -> attesa
    STOP        = 4, //4: -> alla fermata
};

enum trail_states{
    TRAIL_ON  = 1,
    TRAIL_OFF = 0, 
};

struct train_bitmap {
    BITMAP  *train1;
    BITMAP  *train2;
    BITMAP  *train3;
    pthread_mutex_t mutex;
};

struct button_struct {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    bool state;
    BITMAP *button_on;
    BITMAP *button_off;
};

struct station_struct{
    bool            status;
    bool            move_queue;
    int             xPointDraw;
    int             yPointDraw; 
    int             xPointStop;
    int             yPointStop;
    int             xPointIn;
    int             xPointOut;
    int             queue;
    int             trail_state;
    int             trail_angle;
    float           trail_angle_cnt;
    float           trail_angle_inc;
    int             queue_list[MAX_TRAINS_IN_QUEUE];
    BITMAP          *sem;
    pthread_mutex_t mutex;
};

struct wagon_parameters {
    int posx;
    int posy;
    BITMAP *bmp;
};

struct train_parameters {
    bool run;
    bool ready_to_go_flag;
    bool alreadyStopped;
    bool station_flag;
    int direction;
    int binary;
    int priority;
    int posx;
    int posy;
    float currentVel;
    float maxVel;
    int count;
    int stopx;
    int pos_in_queue;
    int binary_occupied;
    struct wagon_parameters wagons[WAGONS];
    struct station_struct *stopToCheck;
    pthread_mutex_t mutex;
};

/** Generic variables */
bool EXIT;
bool EXIT_COMMAND;
int prova;
float px2m;
int ready_trains_num;
int going_trains_num;
int last_assigned_train_id;
int station_stop;
int station_in;
int station_out;
int semaphoresManagerTaskId;

/** Bitmaps */
BITMAP *background;
BITMAP *buffer;
BITMAP *interface;
BITMAP *interface_buffer;
BITMAP *sem_r;
BITMAP *sem_y;
BITMAP *sem_g;
BITMAP *trail;
BITMAP *sem;
BITMAP *platform;
BITMAP *random_train_on;
BITMAP *random_train_off;
BITMAP *hp_train_on;
BITMAP *hp_train_off;
BITMAP *mp_train_on;
BITMAP *mp_train_off;
BITMAP *lp_train_on;
BITMAP *lp_train_off;
BITMAP *close_program_on;
BITMAP *close_program_off;

/** Array di strutture delle stazoioni centrali */
struct station_struct   station[STATIONS_NUM];
/** Array di strutture dei semafori degli scambi */
/** I semafori sono utilizzati come stazioni così da poter sfruttare la funzione "manage_queue" */
struct station_struct   semaphores[RAILWAYS_SWITCHES_SEMAPHORES];
/** Array di strutture dei treni */
struct train_parameters train_par[TMAX];
/** Array di strutture dei bitmap dei treni */
struct train_bitmap     train_bmp[4];
/** Array di strutture dei pulsanti */
struct button_struct    button[N_BUTTONS];

/** Trails points */
int trail1_xPoints[4];  //vettore di 4 interi che mi conterrà le x dei 4 punti di snodo
int trail2_xPoints[4];
int trail3_xPoints[4];
int trail4_xPoints[4];  //probabilmente inutile perché il treno sul binario 4 non ha punti di snodo, vediamo
int* trails_xPoints[4]; //vettore di puntatori a intero, contiene gli indirizzi di tutti i vettori sopra
int trains_in_binary[STATIONS_NUM];

/**
 * Initialization function.
 * 
 * Funzione che inizializza alcune variabili, allegro e disegna il background
 * eventualmente sarebbe più facile caricare direttamente uno sfondo
 */
void initialize();

/**
 * User task. 
 * It creates other tasks and close the program
 */
void *user_task(void *p);

int check_button(int x, int y);


/**
 * Graphic task. 
 * It manages all the graphics objects.
 */
void *graphics(void *p);

/**
 * Station task. 
 * It manages all the stations and semaphores according to the trains movements.
 * 
 */
void *station_manager(void *p);

/**
 * Train task. 
 * It manages the train.
 */
void *new_train(void *p);

/**
 * Set the train parameters.
 */
void set_train_parameters(int i);

/**
 * Manages the queue ad the station.
 */
void manage_queue(int stationId);

 /** 
  * Manages the train movements. 
  */
// void move(int i, int step);
void move(int i, float vel, float acc);

/**
 * Moves the train along diagonal trail towards up.
 */
void move_diag_up(int i, int j, int priority, int inc);

/**
 * Moves the train along diagonal trail towards down.
 */
void move_diag_down(int i, int j, int priority, int inc);

/**
 * Move the train along straight trail.
 */
void move_forward(int i, int j, int priority, int inc);

/**
 * Stops the train at the station for a given time.
 */
void stopAtStation(int i);


void checkSemaphoreIn(int trainId, int semId, int semStateRequired);
/**
 * Check the semaphore OUT semId state w.r.t the train trainId and
    changes the switch position to reach the required state semStateRequired.
 */
void checkSemaphoreOut(int trainId, int semId, int semStateRequired);

/**
 * Checks if the train is in the station of the binary it is moving on
 */
void checkStation (int trainId);
/**
 * Stops al the running thread and exit.
 */
void exit_all();

/**
 * Get the scancode from keyboard.
 *         
 * Funzione che da come risultato il tasto premuto, se ne è stato 
 * premuto uno, altrimenti da come risultato 0
 */
char get_scancode();

#endif