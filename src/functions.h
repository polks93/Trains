#include <stdbool.h>
#include "ptask.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_h

// TASK
#define     TMAX                                150
#define     TRAIN_TASK_PERIOD                   20
#define     TRAIN_TASK_DL                       10
#define     TRAIN_TASK_PRIO                     255 

#define     STATION_MANAGER_TASK_ID             TMAX
#define     STATION_MANAGER_TASK_PERIOD         20
#define     STATION_MANAGER_TASK_DL             10
#define     STATION_MANAGER_TASK_PRIO           255

#define     GRAPHIC_TASK_ID                     TMAX + 1
#define     GRAPHIC_TASK_PERIOD                 20
#define     GRAPHIC_TASK_DL                     10
#define     GRAPHIC_TASK_PRIO                   255

#define     USER_TASK_ID                        TMAX + 2
#define     USER_TASK_PERIOD                    50
#define     USER_TASK_DL                        10
#define     USER_TASK_PRIO                      255

// GRAFICA
#define     W                                   1280          
#define     H                                   630            
#define     INTERFACE_H                         90
#define     INTERFACE_W                         1280
#define     WINDOW_H                            H + INTERFACE_H
#define     N_BUTTONS                           9
#define     L_BUTTONS                           50
#define     SPACE_BUTTONS                       20
#define     SPACE                               H/9            

// TRENI
#define     TRAIN_W                             20            
#define     TRAIN_H                             10
#define     WAGONS                              3             
#define     WAGONS_SPACE                        2
#define     TRAIN_SPACE                         (TRAIN_W + WAGONS_SPACE + 2)*WAGONS 
#define     STOP_SPACE                          TRAIN_W
#define     SLOW_DOWN_SPACE                     4*TRAIN_W
#define     MAX_VEL                             150
#define     MAX_ACC                             100
#define     MIN_MS_BETWEEN_TRAINS               1000
#define     MAX_MS_BETWEEN_TRAINS               2000

// SEMAFORI, STAZIONI E BINARI
#define     SEMAPHORES_NUM                      12
#define     STATIONS_NUM                        8
#define     MAX_TRAINS_IN_QUEUE                 10       
#define     SEM_SIZE_FACTOR                     0.1
#define     ARROW_SIZE_FACTOR                   0.5
#define     TRAIL_W                             30
#define     TRAIL_H                             2
#define     TRAIL_UP_BIN_IN_SWITCH_ON           -32
#define     TRAIL_UP_BIN_IN_SWITCH_OFF          0
#define     TRAIL_UP_BIN_OUT_SWITCH_ON          -96
#define     TRAIL_UP_BIN_OUT_SWITCH_OFF         -128
#define     TRAIL_DOWN_BIN_IN_SWITCH_ON         -160
#define     TRAIL_DOWN_BIN_IN_SWITCH_OFF        -128
#define     TRAIL_DOWN_BIN_OUT_SWITCH_ON        32
#define     TRAIL_DOWN_BIN_OUT_SWITCH_OFF       0
#define     TRAIL_ANGLE_INC                     4
#define     STOP_TIME                           2000  

enum train_states{
    GO_FAST     = 0,    //0: -> andatura normale
    SLOW_DOWN   = 1,    //1: -> dentro stazione
    SPEED_UP    = 2,    //2: -> fuori dalla stazione
    WAIT        = 3,    //3: -> attesa
    STOP        = 4,    //4: -> alla fermata
    QUEUE       = 5     //5: -> in coda
};

enum trail_states{
    TRAIL_ON  = 1,
    TRAIL_OFF = 0 
};

enum train_direction{
    FROM_SX = 0,
    FROM_DX = 1
};

enum train_priority{
    HIGH_PRIO = 3,
    MEDIUM_PRIO = 2,
    LOW_PRIO = 1
};

enum type_of_stop{
    SEMAPHORE   = 0,
    STATION     = 1
};

enum buttons_names{
    NEW_RND_TRAIN = 0,
    NEW_HP_TRAIN = 1,
    NEW_MP_TRAIN = 2,
    NEW_LP_TRAIN = 3,
    TRAIN_FROM_DX = 4,
    TRAIN_FROM_SX = 5,
    RANDOM_DIRECTION = 6,
    AUTO_MODE = 7,
    CLOSE_PROGRAM = 8
};

struct train_bitmap {
    BITMAP              *train1;
    BITMAP              *train2;
    BITMAP              *train3;
    pthread_mutex_t     mutex;
};

struct button_struct {
    int     x_min;
    int     x_max;
    int     y_min;
    int     y_max;
    bool    state;
    BITMAP  *button_on;
    BITMAP  *button_off;
};

struct station_struct{
    bool                status;
    bool                move_queue;
    int                 xPointStop;                     // punto di frenata
    int                 xPointDraw;                     // punto in cui disegnare il semaforo
    int                 yPointDraw; 
    int                 xPointIn;                       // punto in cui si comincia a rallentare
    int                 xPointOut;                      // punto di uscita dalla stazione
    int                 xPointTrail;                    // punto in cui disegnare il binario mobile
    int                 yPointTrail;
    int                 queue;
    int                 trail_state;
    int                 trail_angle;
    int                 queue_list[MAX_TRAINS_IN_QUEUE];
    int                 trail_list[MAX_TRAINS_IN_QUEUE];
    float               trail_angle_cnt;
    float               trail_angle_inc;
    // struct              timespec    green_time;
    BITMAP              *sem;
    pthread_mutex_t     mutex;
};

struct wagon_parameters {
    bool    diag_passed;
    int     posx;
    int     posy;
    BITMAP  *bmp;
};

struct train_parameters {
    bool                run;
    bool                binary_assigned;
    bool                ready_to_go_flag;
    bool                semaphore_flag;
    bool                station_flag;
    bool                checked;
    bool                queue;
    bool                binary_occupied;
    bool                station_passed[STATIONS_NUM];
    bool                sem_passed[SEMAPHORES_NUM];
    int                 direction;
    int                 binary;
    int                 priority;
    int                 posx;
    int                 count;
    int                 stop_x;
    int                 stop_type;
    int                 stop_id;
    int                 pos_in_queue;
    int                 first_diagonal_wagon;
    float               currentVel;
    pthread_mutex_t     mutex;
    struct              wagon_parameters wagons[WAGONS];
    BITMAP              *bmp[3];
};

// VARIABILI GLOBALI
bool    EXIT;
bool    EXIT_COMMAND;
bool    ASSIGNED_DIRECTION;
bool    INIT_RED_TIME_SX;
bool    INIT_RED_TIME_DX;
bool    MOVE_TRAILS_SX;
bool    MOVE_TRAILS_DX;
bool    READY_TO_GO_SX;
bool    READY_TO_GO_DX;
bool    AUTO_MODE_ON;
int     ready_trains_num;
int     last_assigned_train_id;
int     total_train_dl;
int     user_direction;
int     max_prio_train_sx;
int     max_prio_train_dx;
struct  timespec    last_assigned_train_from_sx;
struct  timespec    last_assigned_train_from_dx;
struct  timespec    last_red_time_sx;
struct  timespec    last_red_time_dx;


// Mutex delle variabili globali
pthread_mutex_t ASSIGNED_DIRECTION_MUTEX;
pthread_mutex_t INIT_RED_TIME_SX_MUTEX;
pthread_mutex_t INIT_RED_TIME_DX_MUTEX;
pthread_mutex_t MOVE_TRAILS_SX_MUTEX;
pthread_mutex_t MOVE_TRAILS_DX_MUTEX;
pthread_mutex_t READY_TO_GO_SX_MUTEX;
pthread_mutex_t READY_TO_GO_DX_MUTEX;
pthread_mutex_t AUTO_MODE_ON_MUTEX;
pthread_mutex_t ready_trains_num_mutex;
pthread_mutex_t last_assigned_train_id_mutex;
pthread_mutex_t trains_in_binary_mutex;
pthread_mutex_t user_direction_mutex;
pthread_mutex_t last_assigned_train_from_dx_mutex;
pthread_mutex_t last_assigned_train_from_sx_mutex;
pthread_mutex_t max_prio_train_sx_mutex;
pthread_mutex_t max_prio_train_dx_mutex;
pthread_mutex_t last_red_time_sx_mutex;
pthread_mutex_t last_red_time_dx_mutex;

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
BITMAP *from_dx_on;
BITMAP *from_dx_off;
BITMAP *from_sx_on;
BITMAP *from_sx_off;
BITMAP *rnd_direction_on;
BITMAP *rnd_direction_off;
BITMAP *auto_mode_on;
BITMAP *auto_mode_off;
BITMAP *green_arrow_dx;
BITMAP *green_arrow_sx;
BITMAP *red_arrow_dx;
BITMAP *red_arrow_sx;

/** Array di strutture delle stazoioni centrali */
struct station_struct   station[STATIONS_NUM];

/** Array di strutture dei semafori degli scambi */
struct station_struct   semaphores[SEMAPHORES_NUM];

/** Array di strutture dei treni */
struct train_parameters train_par[TMAX];

/** Array di strutture dei bitmap dei treni */
struct train_bitmap     train_bmp[3];

/** Array di strutture dei pulsanti */
struct button_struct    button[N_BUTTONS];

/** Trails points */
int trail1_xPoints[4];  //vettore di 4 interi che mi conterrà le x dei 4 punti di snodo
int trail2_xPoints[4];
int trail3_xPoints[4];
int trail4_xPoints[4];  
int* trails_xPoints[8]; //vettore di puntatori a intero, contiene gli indirizzi di tutti i vettori sopra
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
 * binary_assignement. 
 * 
 */
void binary_assignment();

void manual_binary_assignment(int binary);

void move_semaphore_queue_in(int semId);

void move_station_queue(int stationId);

/**
 * Train task. 
 * It manages the train.
 */
void *train(void *p);

/**
 * Set the train parameters.
 */
void set_train_parameters(int i);

 /** 
  * Manages the train movements. 
  */
// void move(int i, int step);
void move(int trainId, float vel, float acc);

/**
 * Moves the train along diagonal trail towards up.
 */
void move_diag_up(int i, int j, int inc);

/**
 * Moves the train along diagonal trail towards down.
 */
void move_diag_down(int i, int j, int inc);

/**
 * Move the train along straight trail.
 */
void move_forward(int i, int j, int inc);

/**
 * Check the semaphore IN semId state w.r.t the train trainId and
    changes the switch position to reach the required state semStateRequired.
 */

void checkSemaphoreIn(int trainId, int semId, int semStateRequired);

/**
 * Check the semaphore OUT semId state w.r.t the train trainId and
    changes the switch position to reach the required state semStateRequired.
 */

void stationOutSx();

void stationOutDx();
/**
 * Check the semaphore OUT semId state w.r.t the train trainId and
    changes the switch position to reach the required state semStateRequired.
 */

void checkSemaphoreOutDown();
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

/**
 * Restituisce il segno dell'intero
 *         
 */
int sign(int x);

#endif