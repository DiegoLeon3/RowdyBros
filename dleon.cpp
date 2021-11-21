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
#include "Monster.h"
#define rnd() (((double)rand())/(double)RAND_MAX)



Monster:: Monster(){
        pos[0] = 200.0f;
        pos[1] = 280.0f;
        vel[0] = rnd() * 0.6 - 0.3;
        vel[1] = rnd() * 0.6 - 0.3;
        xres = 800;
        yres = 800;
        n = 12; 
        w = 2;
        h = 2;
}


void makeParticle(int mx, int my, Monster *monster, int yres)
 {
     printf("makeParticle");
     if (monster->n >= MAX_PARTICLES)
        return;
     monster[monster->n].pos[0] = mx;
     monster[monster->n].pos[1] = yres - my;
     monster[monster->n].vel[0] = rnd() * 0.6 - 0.3;
     monster[monster->n].vel[1] = rnd() * 0.6 - 0.2;
     monster[monster->n].w = 4;
    monster[monster->n].h = 4;
    ++monster->n;
 }

void particlePhysics(Monster *monster){
     //printf("particlePhysics");
        Monster *p = monster;
     for (int i = 0; i < monster->n; i++) {
         p->vel[1] -= 0.5;
         p->pos[0] += p->vel[0];
         p->pos[1] += p->vel[1];
      //for( int j =0; j < 5; j++){
        //   if (p->pos[1]-p->h <= ybox[j] + hbox[j] &&
        //       p->pos[0]-p->w <= xbox[j] + wbox[j] &&
        //       p->pos[0] + p->w >= xbox[j] - wbox[j])
         
              p->pos[1] = p->h + 0.001;
              p->vel[1] = -(p->vel[1] * 0.9);
              p->vel[0] *= 1.1;
          
      //}
         //did particle fall off screen?
        //  if( p->pos[1] < 0.0)
        //  {
        //  //remove particle from array
        //      monster[i] = monster[monster->n-1];
        //      --monster->n;
        // }
        ++p;
     }

}

void particleRender(Monster *monster){
     printf("particleRender");
         Monster *p = monster;
     for (int i = 0; i < monster->n; i++) {
     glPushMatrix();
     glColor3ub(145, 145, 240);
         glTranslatef(p->pos[0], p->pos[1], 0.0f);
     glBegin(GL_QUADS);
         glVertex2f(-p->w, -p->h);
         glVertex2f(-p->w,  p->h);
         glVertex2f( p->w,  p->h);
         glVertex2f( p->w, -p->h);
 
     glEnd();
     glPopMatrix();
     ++p;
     }


}


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
