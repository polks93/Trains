#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "ptask.h"
#include "functions.h"

int main() {
    
    int j = 1;

    initialize();
    
    do {   
        if (EXIT_COMMAND == true) j = 0; 
    } while (j); 
    
    exit_all();
    return 0;
}