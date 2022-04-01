#include <stdbool.h>
#include "ptask.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_h
#define TMAX                            100     // Numero massimo di treni
#define track_pieces                    12      // Numero di scambi dei binari
#define STATIONS_NUM                    8
#define MAX_TRAINS_IN_QUEUE             2
#define RAILWAYS_SWITCHES_SEMAPHORES    12
#define MAX_WAGONS_PER_TRAIN            10
#define N_BUTTONS                       10

enum train_states{
    GO_FAST  = 0, //0: -> fuori da stazione
    GO_SLOW  = 1, //1: -> dentro stazione
    WAIT     = 2, //2: -> attesa
    STOP     = 3, //3: -> alla fermata
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
    int             xPointStop;
    int             yPointStop;
    int             xPointIn;
    int             xPointOut;
    int             queue;
    int             trail_angle;
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
    int wagonsNumber;
    struct wagon_parameters wagons[MAX_WAGONS_PER_TRAIN];
    BITMAP *bmp;
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
int stationsManagerTaskId;
int semaphoresManagerTaskId;
int graphicTaskId;
int userTaskId;

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
void move_diag_up(int i, int j, int bin, int inc);

/**
 * Moves the train along diagonal trail towards down.
 */
void move_diag_down(int i, int j, int bin, int inc);

/**
 * Move the train along straight trail.
 */
void move_forward(int i, int j, int bin, int inc);

/**
 * Stops the train at the station for a given time.
 */
void stopAtStation(int i);

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