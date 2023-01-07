#ifndef     GRAPHICS_H
#define     GRAPHICS_H

/**
 * Inizializzazione grafica:
 * inizializza allegro e disegna l'interfaccia iniziale
 */
void graphics_init();

/**
 * Loading di tutti i bitmaps dalla cartella img
 */
void loading_bitmaps();

/**
 * Aggiorna i semafori degli incroci
 */
void draw_semaphores(int sem_w, int sem_h);

/**
 * Aggiorna i semafori delle stazioni
 */
void draw_stations(int sem_w, int sem_h);


/**
 * Aggiorna i binari mobili
 */
void draw_trails();

/**
 * Aggiorna i treni
 */
void draw_trains();

/**
 * Aggiorna le frecce direzionali
 */
void draw_arrows(int arrow_w, int arrow_h);

/**
 * Aggiorna l'interfaccia
 */
void draw_interface();

/**
 * Graphic task. 
 * Gestisce tutta la parte grafica
 */
void *graphics(void *p);

#endif