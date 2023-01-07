#ifndef     USER_H
#define     USER_H

/**
 * Task utente.
 * Permette di:
 *  - Creare nuovi task treno utilizzando mouse e interfaccia
 *  - Creare nuovi task treno tramite tastiera
 *  - Creare nuovi task treno in modalità autonoma
 *  - Uscire dal programma usando mouse o tastiera
 */
void *user_task(void *p);

/**
 *  Individua l'id del pulsante che è stato premuto
 */
void read_commands_from_mouse();

/**
 *  Imposta la direzione, definita tramite interfaccia, in cui appariranno i nuovi treni
 */
void set_manual_direction(int assigned_direction);

/**
 *  Reset della direzione random dei nuovi treni
 */
void reset_random_direction();

/**
 *  Attiva/disattiva la modalità automatica di generazione dei treni
 */
void auto_mode_manager();

/**
 *  Esegue le operazioni definite dai pulsanti:
 *  - SPACE : Creazione di un nuovo task treno a priorità casuale
 *  - ESC   : Chiusura del programma
 */
void read_commands_from_keyboard();

/**
 * 
 * Funzione che da come risultato il tasto premuto, se ne è stato 
 * premuto uno, altrimenti da come risultato 0
 */
char get_scancode();

/**
 *  Modalità automatica di generazione dei treni.
 *  Quando l'utente attiva questa modalità, un nuovo task treno viene creato dopo 2 secondi
 *  dal precedente. E' comunque possibile continuare a generare task in modo manuale
 */
void autonomous_mode();

/**
 *  Individua l'id del pulsante che è stato premuto
 */
int check_button(int x, int y);

#endif