#ifndef MONSTER_H
#define MOSNTER_H
#define MAX_PARTICLES 100

class Monster {
      public:
          float pos[2];
          float vel[2];
          float w,h;
          int n, xres, yres;
          Monster();
         
   };
          void makeParticle(int, int); 
          void particlePhysics(); 
          void particleRender(); 
#endif