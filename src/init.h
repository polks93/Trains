#include <stdbool.h>

#ifndef         INIT_H
#define         INIT_H

// --------------------------------------- VALORI COSTANTI --------------------------------------  //

// Task
#define     TMAX                                150         // Numero massimo di task treno
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

// Grafica
#define     W                                   1280                // Larghezza finestra principale
#define     H                                   630                 // Altezza finestra principale
#define     INTERFACE_H                         90                  // Altezza finestra interfaccia
#define     INTERFACE_W                         W                   // Larghezza finestra interfaccia
#define     WINDOW_H                            H + INTERFACE_H     // Altezza totale della finestra
#define     N_BUTTONS                           9                   // Numero di pulsanti
#define     L_BUTTONS                           50                  // Larghezza dei pulsanti
#define     SPACE_BUTTONS                       20                  // Spazio tra i vari pulsanti
#define     SPACE                               H/9                 // Spazio tra i binari

// Treni
#define     TRAIN_W                             20                                      // Larghezza treni            
#define     TRAIN_H                             10                                      // Altezza treni
#define     WAGONS                              3                                       // Numero di vagoni per treno
#define     WAGONS_SPACE                        2                                       // Spazio tra i vagoni
#define     TRAIN_SPACE                         (TRAIN_W + WAGONS_SPACE + 2)*WAGONS     // Ingombro del treno
#define     STOP_SPACE                          TRAIN_W                                 // Spazio tra "testa" del treno e coordinata di stop
#define     SLOW_DOWN_SPACE                     4*TRAIN_W                               // Lunghezza dello spazio in cui il treno deve frenare prima dello stop
#define     MAX_VEL                             200                                     // Velocità massima del treno                                     
#define     MAX_ACC                             50                                      // Acellerazione massima del treno
#define     MIN_MS_BETWEEN_TRAINS               1000                                    // Minimi MS tra un treno e il successivo
#define     MAX_MS_BETWEEN_TRAINS               2000                                    // MS tra un treno e l'altro in caso di modalità automatica

// Semafori, stazioni e binari
#define     SEMAPHORES_NUM                      12                      // Numero di semafori
#define     STATIONS_NUM                        8                       // Numero di stazioni
#define     MAX_TRAINS_IN_QUEUE                 10                      // Numero massimo di treni in coda
#define     SEM_SIZE_FACTOR                     0.1                     // Fattore di scala dei bitmap dei semafori
#define     ARROW_SIZE_FACTOR                   0.5                     // Fattore di scala dei bitmap delle frecce
#define     TRAIL_W                             30                      // Lunghezza binari mobili 
#define     TRAIL_H                             2                       // Altezza binari mobili
#define     TRAIL_UP_BIN_IN_SWITCH_ON           -32                     // Posizione angolare dei binari mobili 
#define     TRAIL_UP_BIN_IN_SWITCH_OFF          0                       // Posizione angolare dei binari mobili
#define     TRAIL_UP_BIN_OUT_SWITCH_ON          -96                     // Posizione angolare dei binari mobili 
#define     TRAIL_UP_BIN_OUT_SWITCH_OFF         -128                    // Posizione angolare dei binari mobili 
#define     TRAIL_DOWN_BIN_IN_SWITCH_ON         -160                    // Posizione angolare dei binari mobili 
#define     TRAIL_DOWN_BIN_IN_SWITCH_OFF        -128                    // Posizione angolare dei binari mobili
#define     TRAIL_DOWN_BIN_OUT_SWITCH_ON        32                      // Posizione angolare dei binari mobili
#define     TRAIL_DOWN_BIN_OUT_SWITCH_OFF       0                       // Posizione angolare dei binari mobili
#define     TRAIL_ANGLE_INC                     4                       // Velocità angolare dei binari mobili
#define     STOP_TIME                           1000                    // Tempo massimo di stop alla stazione

// ------------------------------------- ASSEGANZIONE NOMI A VARIABILI NUMERICHE ------------------------------------- // 

// Macchina a stati del task train
enum train_states{
    GO_FAST     = 0,    // Andatura normale
    SLOW_DOWN   = 1,    // Rallentare prima di una fermata
    SPEED_UP    = 2,    // Acellerare dopo una fermata
    WAIT        = 3,    // Attesa
    STOP        = 4,    // Stop alla fermata quando il treno è il primo della coda
    QUEUE       = 5     // All'interno di una coda
};

// Direzione dei treni
enum train_direction{
    FROM_SX = 0,
    FROM_DX = 1
};

// Priorità dei treni
enum train_priority{
    HIGH_PRIO   = 3,
    MEDIUM_PRIO = 2,
    LOW_PRIO    = 1
};

// Posizione dei binari mobili
enum trail_states{
    TRAIL_ON  = 1,      // Posizione orizzontale
    TRAIL_OFF = 0       // Posizione inclinata
};

// Tipologie di fermata
enum type_of_stop{
    SEMAPHORE   = 0,    // Semaforo davanti a un binario mobile
    STATION     = 1     // Semaforo della stazione
};

// Nome dei pulsanti sull'interfaccia
enum buttons_names{
    NEW_RND_TRAIN =     0,      // Genera un nuovo task treno con priorità casuale
    NEW_HP_TRAIN =      1,      // Genera un nuovo task treno con priorità alta
    NEW_MP_TRAIN =      2,      // Genera un nuovo task treno con priorità media
    NEW_LP_TRAIN =      3,      // Genera un nuovo task treno con priorità bassa
    TRAIN_FROM_DX =     4,      // Seleziona la direzione di arrivo dei treni da DX
    TRAIN_FROM_SX =     5,      // Seleziona la direzione di arrivo dei treni da SX
    RANDOM_DIRECTION =  6,      // La direzione verà assegnata in modo casuale
    AUTO_MODE =         7,      // I task treni verrano generati in modo autonomo
    CLOSE_PROGRAM =     8       // Uscita dal programma
};

// ---------------------------------------------- DEFINIZIONE STRUTTURE --------------------------------------------- // 

// Struttura che contiene i bitmap associati a ogni treno
struct train_bitmap {
    BITMAP              *train1;        // Treno orizzontale
    BITMAP              *train2;        // Treno inclinato a 45°
    BITMAP              *train3;        // Treno inclinato a -45°
    pthread_mutex_t     mutex;          // Mutex associato alla struttura
};

// Struttura associata a ogni pulsante
struct button_struct {
    int     x_min;                      // Coordinata X in cui inizia il pulsante 
    int     x_max;                      // Coordinata X in cui finisce il pulsante
    int     y_min;                      // Coordinata Y in cui inizia il pulsante
    int     y_max;                      // Coordinata Y in cui finisce il pulsante
    bool    state;                      // Stato del pulsante: true = premuto, false = non premuto
    BITMAP  *button_on;                 // Bitmap associato al pulsante premuto
    BITMAP  *button_off;                // Bitmap associato al pulsante non premuto
};

// Struttura associata ad ogni stazione o semaforo
struct station_struct{
    bool                status;                             // Stato del semaforo: true = verde, false = rosso
    bool                move_queue;                         // Flag per indicare quando muovere la coda
    bool                trail_state;                        // Posizione del binario mobile (ON/OFF)
    int                 xPointStop;                         // Coordinata X in cui è necessario fermarsi
    int                 xPointDraw;                         // Coordinata X in cui disegnare il semaforo
    int                 yPointDraw;                         // Coordinata Y in cui disegnare il semaforo
    int                 xPointIn;                           // Coordinata X in cui il treno deve cominciare a rallentare
    int                 xPointOut;                          // Coordinata X di uscita dalla stazione
    int                 xPointTrail;                        // Coordinata X in cui disegnare il binario mobile
    int                 yPointTrail;                        // Coordinata Y in cui disegnare il binario mobile
    int                 queue;                              // Numero di treni in coda al semaforo
    int                 trail_angle;                        // Posizione angolare del binario mobile espressa in numeri
    int                 queue_list[MAX_TRAINS_IN_QUEUE];    // Lista dei treni in coda al semaforo
    int                 trail_list[MAX_TRAINS_IN_QUEUE];    // Lista delle posizione richieste dai corrispetivi treni in coda
    BITMAP              *sem;                               // Bitmap associato al semaforo
    pthread_mutex_t     mutex;                              // Mutex associato al semaforo
};

// Struttra associata ai singoli vagoni di ogni treno
struct wagon_parameters {
    bool    diag_passed;            //  Flag per indicare se il singolo vagone ha passato il punto di movimento in diagonale
    int     posx;                   //  Coordinata X del singolo vagone
    int     posy;                   //  Coordinata Y del singolo vagone
    BITMAP  *bmp;                   //  Bitmap associata al singolo vagone
};

// Struttra associata ad ogni treno
struct train_parameters {
    bool                run;                                // Flag per indicare se il treno è attivo
    bool                binary_assigned;                    // Flag per indicare se è stato assegnato un binario al treno
    bool                ready_to_go_from_queue;             // Flag per indicare che il treno è pronto a muoversi nella coda
    bool                semaphore_flag;                     // Flag per indicare che il treno è entrato per la prima volta nel range di un semaforo
    bool                checked;                            // Flag per indicare se lo station manager ha già trovato la prossima fermata
    bool                queue;                              // Flag per indicare se il treno si trova in coda
    bool                binary_occupied;                    // Flag per indicare se il treno sta occupando il binario, il binario è considerato libero dopo aver superato la stazione
    bool                station_passed[STATIONS_NUM];       // Array di flag per indicare se il treno ha superato uan determinata la stazione
    bool                sem_passed[SEMAPHORES_NUM];         // Array di flag per indicare se il treno ha superato un determinato semaforo
    int                 direction;                          // Direzione del treno: FROM_DX/FROM_SX
    int                 binary;                             // Binario associato al treno
    int                 priority;                           // Priorità associata al treno
    int                 posx;                               // Coordinata X della "testa" del treno
    int                 stop_x;                             // Coordinata X del prossimo stop
    int                 stop_type;                          // Tipologia del prossimo stop: STATION/SEMAPHORE
    int                 stop_id;                            // Id del prossimo stop
    int                 pos_in_queue;                       // Posizione del treno nell'eventuale coda
    int                 first_diagonal_wagon;               // Primo vagone che si sta muovendo in diagonale, in modo che quelli successivi ne seguano il movimento
    float               currentVel;                         // Velocità del treno
    pthread_mutex_t     mutex;                              // Mutex associato al treno
    struct              wagon_parameters wagons[WAGONS];    // Struttura associata ai singoli vagoni del treno
    BITMAP              *bmp[3];                            // Bitmaps associati al treno
};

// ------------------------------------------------ VARIABILI GLOBALI ------------------------------------------------ // 

// Variabili globali che vengono modificate durante l'esecuzione del programma
bool        EXIT;                                                   // Flag per chiudere il programma
bool        EXIT_COMMAND;                                           // Flag per inviare il comando di chiusura del programma
bool        ASSIGNED_DIRECTION;                                     // Flag per indicare se la direzione di comparsa dei treni viene assegnata dall'utente
bool        INIT_RED_TIME_SX;                                       // Flag per indicare se il timer associato ai treni che vengono da SX è stato inizializzato
bool        INIT_RED_TIME_DX;                                       // Flag per indicare se il timer associato ai treni che vengono da DX è stato inizializzato
bool        MOVE_TRAILS_SX;                                         // Flag per indicare se muovere i binari mobili in uscita dalla stazione lato SX 
bool        MOVE_TRAILS_DX;                                         // Flag per indicare se muovere i binari mobili in uscita dalla stazione lato DX
bool        READY_TO_GO_SX;                                         // Flag per indicare se il treno può muoversi dalla stazione lato SX
bool        READY_TO_GO_DX;                                         // Flag per indicare se il treno può muoversi dalla stazione lato DX
bool        AUTO_MODE_ON;                                           // Flag per inidcare se è stata attivata la modalità di generazione di nuovi treni automatica
int         next_trainId;                                           // Id del prossimo task treno da generare
int         ready_trains_num;                                       // Numero di nuovi treni a cui deve essere assegnato un binario
int         last_assigned_train_id;                                 // Id dell'ultimo treno a cui è stato assegnato un binario
int         total_train_dl;                                         // DL miss totali di tutti i task treno
int         user_direction;                                         // Direzione decisa dall'utente: FROM_DX/FROM_SX
int         max_prio_train_sx;                                      // Id del treno a priorità massima tra i primi della coda delle stazioni lato SX
int         max_prio_train_dx;                                      // Id del treno a priorità massima tra i primi della coda delle stazioni lato DX
int         trains_in_binary[STATIONS_NUM];                         // Array che contiene il numero di treni presenti su ogni binario
struct      timespec            last_assigned_train_from_sx;        // Istante in cui è stato generato l'ultimo treno lato SX
struct      timespec            last_assigned_train_from_dx;        // Istante in cui è stato generato l'ultimo treno lato DX
struct      timespec            last_red_time_sx;                   // Istante in cui è diventato rosso il primo semaforo di una stazione lato SX
struct      timespec            last_red_time_dx;                   // Istante in cui è diventato rosso il primo semaforo di una stazione lato DX
struct      station_struct      station[STATIONS_NUM];              // Strutture associate a ogni staizone
struct      station_struct      semaphores[SEMAPHORES_NUM];         // Strutture associate a ogni semaforo
struct      train_parameters    train_par[TMAX];                    // Strutture associate a ogni task treno
struct      train_bitmap        train_bmp[3];                       // Strutture contenenti i bitmap dei treni
struct      button_struct       button[N_BUTTONS];                  // Strutture associate a ogni pulsante

// Mutex associati alle variabili globali definite sopra
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
pthread_mutex_t max_prio_train_sx_mutex;
pthread_mutex_t max_prio_train_dx_mutex;
pthread_mutex_t last_assigned_train_from_dx_mutex;
pthread_mutex_t last_assigned_train_from_sx_mutex;
pthread_mutex_t last_red_time_sx_mutex;
pthread_mutex_t last_red_time_dx_mutex;

// Variabili che non vengono mai modificate ma vengolo lette da diverse funzioni
int         trail1_xPoints[4];                  // Array dei punti in cui i binari presentano un angolo
int         trail2_xPoints[4];
int         trail3_xPoints[4];
int         trail4_xPoints[4];  
int         *trails_xPoints[8];                 // Array di puntatori a intero, contiene gli indirizzi di tutti i vettori sopra

// ----------------------------------------------------- BITMAPS ----------------------------------------------------- // 

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

// ----------------------------------------------------- FUNZIONI ---------------------------------------------------- // 

/**
 * Inizializzazione
 * 
 * Funzione che inizializza le variabili globali, allegro, disegna il background e genera 3 task:
 * - task utente
 * - task grafico
 * - task station
 */
void initialize();

/**
 * Inizializzazione di tutti i mutex
 * 
 */
void mutex_init();

/**
 * Inizializzazione di tutte le variabili globali
 * 
 */
void global_variables_init();

/**
 * Inizializzazione strutture semafori
 * 
 */
void semaphores_struct_init();

/**
 * Inizializzazione strutture stazioni
 * 
 */
void stations_struct_init();

/**
 * Inizializzazione struttra pulsanti
 * 
 */
void buttons_init();

/**
 * Chiude tutti i thread attivi e esce dal programma
 */
void exit_all();

#endif