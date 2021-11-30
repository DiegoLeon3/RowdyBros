//Shadi Abdul Razzak

#include "fonts.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define USE_OPENAL_SOUND
#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif

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

void playCoin(){
    //Get started right here.
#ifdef USE_OPENAL_SOUND
    ALuint alBuffer;
    ALuint alSource;
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: alutInit()\n");
        return;
    }
    //Clear error state.
    alGetError();
    //
    //Setup the listener.
    //Forward and up vectors are used.
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);
    //
    //alBuffer holds the sound information.
    alBuffer = alutCreateBufferFromFile("./coin.wav");
    //
    //Source refers to the sound.
    //Generate a source, and store it in a buffer.
    alGenSources(1, &alSource);
    alSourcei(alSource, AL_BUFFER, alBuffer);
    //Set volume and pitch to normal, ensure looping of sound.
    alSourcef(alSource, AL_GAIN, 1.0f);
    alSourcef(alSource, AL_PITCH, 1.0f);
    alSourcei(alSource, AL_LOOPING, AL_TRUE);
    alSourcePlay(alSource);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: setting source\n");
        return;
    }
#endif //USE_OPENAL_SOUND
}
