#include <stdbool.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_h

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
 * Checks if the train is in the station of the binary it is moving on
 */
void checkStation (int trainId);

/**
 * Stops al the running thread and exit.
 */
void exit_all();


/**
 * Restituisce il segno dell'intero
 *         
 */
int sign(int x);

#endif