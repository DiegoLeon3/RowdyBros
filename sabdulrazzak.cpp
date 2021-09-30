
#include "fonts.h"

void show_my_creds(int n, int m){
        //establish centering
        //using yres and xres
        Rect r;
        r.bot = n;
        r.left = m;
        r.center = 1;
        //print8b the creds
        ggprint8b(&r, 16, 0x00ff0000, "Shadi did some nice work on this credit line");


}
