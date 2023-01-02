#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "ptask.h"
#include "functions.h"

int main(){
char    scan;
BITMAP  *mysprite;
int     x = 300;
int     y = 200;
int     i = 0;
    allegro_init();
    set_color_depth(15);
    PALETTE pal;
    get_palette(pal);
    int col = makecol(0, 0, 255);
    int black = makecol(0, 0, 0);
    int grey = makecol(50, 50, 50);
    int white = makecol(255, 255, 255);

    mysprite = create_bitmap(50, 50);
    clear_to_color(mysprite, grey);
    textout_centre_ex(mysprite, font, "RESET", 25, 10, col, -1);
    textout_centre_ex(mysprite, font, "RANDOM", 25, 20, col, -1);
    textout_centre_ex(mysprite, font, "DIR", 25, 30, col, -1);
    rectfill(mysprite, 0, 49, 49, 48, black);
    rectfill(mysprite, 48, 0, 49, 49, black);
    rectfill(mysprite, 0, 0, 49, 1, white);
    rectfill(mysprite, 0, 1, 1, 49, white);
    putpixel(mysprite, 1, 49, black);
    putpixel(mysprite, 49 ,1, black);
    putpixel(mysprite, 0, 49, grey);
    putpixel(mysprite, 1, 48, grey);
    putpixel(mysprite, 48, 1, grey);
    putpixel(mysprite, 49, 0, grey);

    save_bitmap("rnd_direction_off.bmp", mysprite, pal);

}
