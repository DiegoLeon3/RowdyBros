//Diego Leon 
//
#include "fonts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h" 

void show_diego_creds(int n, int m) {
        
        Rect r;
        r.bot = n;
        r.left = m;
        r.center = 1;
        ggprint8b(&r, 16, 0x00ff0000, "Diego did some work on github");

}
void show_background(int yres, int xres,GLuint scrollingTexture, 
        float *xc, float *yc) {
      //establish centering 
      //using yres and xres
      //
      Rect r;
      r.bot = yres;
      r.left = xres;
      r.center = 1;
      //print the creds
      ggprint8b(&r, 16, 0xffff00, "Press P to Play");
       glColor3f(1.0, 1.0, 1.0);
         glBindTexture(GL_TEXTURE_2D, scrollingTexture);
         glBegin(GL_QUADS);
             glTexCoord2f(xc[0], yc[1]); glVertex2i(0, 0);
             glTexCoord2f(xc[0], yc[0]); glVertex2i(0, yres);
             glTexCoord2f(xc[1], yc[0]); glVertex2i(xres, yres);
             glTexCoord2f(xc[1], yc[1]); glVertex2i(xres, 0);
         glEnd();
}
