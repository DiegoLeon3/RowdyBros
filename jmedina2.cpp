/*
Javier Medina
3350
Personal file for project

*/
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
#endif //USE_OPENAL_SOUND


#ifdef USE_OPENAL_SOUND
ALuint alBuffer;
ALuint alSource;
#endif

void show_javier_creds(int n, int m) {
        
        Rect r;
        r.bot = n;
        r.left = m;
        r.center = 1;
        ggprint8b(&r, 16, 0x00ff0000, "Javier did some work on his computer at starbucks. Very good.");

}

/*
 * play_sound must be called in physics when the bird hits coin
 * must be called in physics like -> play_sound(gl.alBufferCoin);
 * 
*/


void play_sound(){
        //Get started right here.
#ifdef USE_OPENAL_SOUND
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
        //alBufferCoin holds the sound information.
        alBuffer = alutCreateBufferFromFile("./theme.wav");
        //
        //Source refers to the sound.
        //Generate a source, and store it in a buffer.
        alGenSources(1, &alSource);
        alSourcei(alSource, AL_BUFFER, alBuffer);
        //Set volume and pitch to normal, no looping of sound.
        alSourcef(alSource, AL_GAIN, 1.0f);
        alSourcef(alSource, AL_PITCH, 1.0f);
        alSourcei(alSource, AL_LOOPING, AL_FALSE);
        for (int i=0; i<4; i++) {
                alSourcePlay(alSource);
                usleep(250000);
        }
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting source\n");
		return;
	}
#endif //USE_OPENAL_SOUND
}

void clean_sound(){
	
	#ifdef USE_OPENAL_SOUND
        //Cleanup.
        //First delete the source.
        alDeleteSources(1, &alSource);
        //Delete the buffer.
        alDeleteBuffers(1, &alBuffer);
        //Close out OpenAL itself.
        //Get active context.
        ALCcontext *Context = alcGetCurrentContext();
        //Get device for active context.
        ALCdevice *Device = alcGetContextsDevice(Context);
        //Disable context.
        alcMakeContextCurrent(NULL);
        //Release context(s).
        alcDestroyContext(Context);
        //Close device.
        alcCloseDevice(Device);
	#endif //USE_OPENAL_SOUND
}
