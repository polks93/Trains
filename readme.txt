Per compilare gli script digitare il seguete codice:

gcc main.c functions.c ptask.c -o main `allegro-config --libs` -lpthread -lm

Dopo aver compilato eseguire il main:

./main


la sequenza di ogni riga del file "trains_info.txt" è : treno binario priorità

NUOVA LOGICA TRENI

Fuori dallo spazio di frenata
    vai normale

Nello spazio di frenata 
    Leggo lo stato del semaforo

        Se è verde e non ci sono treni tra me e il semaforo
            vado avanti
        Altrimenti
            mi fermo -> STOP

STOP
    Aspetto che il semaforo sia verde e che il mio id sia il primo della fila

Dopo la stazione
    Accelero fino a Vmax

idem per gli altri semafori

Quando il primo treno deve portare lo switch in un'altra posizione e anche il successivo deve fare la stessa cosa, il secondo treno non passa da move_semaphore_queue!!!!