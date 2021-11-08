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
void show_background(int yres, int xres,GLuint backgroundTexture ) {
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
         glBindTexture(GL_TEXTURE_2D, backgroundTexture);
         glBegin(GL_QUADS);
             glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
             glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
             glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
             glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
         glEnd();
}
