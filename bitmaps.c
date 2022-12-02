#include <allegro.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "ptask.h"
#include "functions.h"

int main(){
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
    int green = makecol(0, 255, 0);
    int pink = makecol(255, 0, 255);
    int red = makecol(0, 0, 255);

    mysprite = create_bitmap(100, 51);
    clear_to_color(mysprite, pink);
    rectfill(mysprite, 25, 14, 100, 34, green);
    triangle(mysprite, 25, 0, 25, 50, 0, 25, green);

    save_bitmap("green_arrow_sx.bmp", mysprite, pal);

}
