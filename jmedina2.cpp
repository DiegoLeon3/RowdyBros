#include "fonts.h"

void show_javier_creds(int n, int m) {
        
        Rect r;
        r.bot = n;
        r.left = m;
        r.center = 1;
        ggprint8b(&r, 16, 0x00ff0000, "Javier did some work on his computer at starbucks. Very good.");

}
