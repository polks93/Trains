#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "init.h"
#include "trains.h"

int main() {
    
    int i;

    initialize();
    
    i = 1;
    do {   
        if (EXIT_COMMAND == true) i = 0; 
    } while (i); 
    
    exit_all();
    return 0;
}