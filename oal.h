#ifndef _OALSOUND_H_
#define _OALSOUND_H_

//#include "al.h"
//#include "alc.h"
#ifdef GGWINDOWS
#include <al/alut.h>
#endif //GGWINDOWS
#ifdef GGLINUX
#include </usr/include/AL/alut.h>
#endif //GGLINUX

//#define USE_SOUNDQUEUE
#define USE_SOUNDHEAP

#ifdef USE_SOUNDHEAP
const int MAX_SOUNDHEAP = 64;
#endif

#define MAX_SOUNDQUEUES (32)

#ifdef USE_SOUNDQUEUE
typedef struct t_soundqueue {
	int start_ticks;
	int time;
	int param;
	int diff;
	float pos[3];
	ALuint source;
	float dist;
	int table_ptr;
	t_soundqueue *prev;
	t_soundqueue *next;
} Soundqueue;
typedef struct t_soundqueue_memory {
	int table[MAX_SOUNDQUEUES];
	int ptr;
	Soundqueue **s;
} Soundqueue_memory;
#endif //USE_SOUNDQUEUE
#ifdef USE_SOUNDHEAP
typedef struct t_soundheap {
	int start_ticks;
	int play_ticks;
	unsigned int source;
	int param;
	Flt dist;
	float pos[3];
} Soundheap;
#endif //USE_SOUNDHEAP


class gp_Sound {
public:
	gp_Sound();
	~gp_Sound() { }
public:
	int onOff;
	int errorflag;
	int listener_interval;
	#ifdef USE_SOUNDQUEUE
	Soundqueue *soundqueue_head;
	#endif //USE_SOUNDQUEUE
	#ifdef USE_SOUNDHEAP
	Soundheap *soundheap;
	int nSoundheap;
	#endif //USE_SOUNDHEAP

	ALuint alSourceLanderExhaust;
	ALuint alSourceHardLandingGround;
	ALuint alSourceSoftLandingGround;
	ALuint alSourceHardLandingLz;
	ALuint alSourceSoftLandingLz;
	ALuint alSourceSoftBounce[8];
	ALuint alSourceHardBounce[8];
	ALuint alSourceEngineOff;
	ALuint alSourceTick;
	ALuint alSourceSlurp;
	ALuint alSourceDing;
	ALuint alSourceCannon;
	ALuint alSourceScore1;
	ALuint alSourceGroundhit;
	ALuint alSourceNukeExplosion;
	ALuint alSourcePortalBounce;
	ALuint alSourceTetraExplode;
	ALuint alSourceRover;
	ALuint alSourceGlider;
	ALuint alSourceRico;
	ALuint alSourceArchbump;
	ALuint alSourceOuch;
	//
	float doppler_factor;
	float doppler_velocity;
	ALuint alCurrentLanderEngine;
	static const int nsoftbounce=5;
	static const int nhardbounce=3;
	int nsoundqueues;
	int nsources;
	int nsoundson;
	int nsoundsoff;
	int nsoundslooping;
	int update_pitch;
	int setup_ticks;
	float listenerPos[3], listenerOri[6];
	#ifdef USE_SOUNDQUEUE
	Soundqueue_memory soundqueue_memory;
	void check_sound_queue(void);
	void delete_soundqueue(Soundqueue *s);
	void insert_soundqueue(const Vec pos, const unsigned int ssource);
	Soundqueue * get_soundqueue_mem(void);
	void setup_soundqueue_memory(void);
	#endif //USE_SOUNDQUEUE
	#ifdef USE_SOUNDHEAP
	void verifyHeap();
	//void heapDownDesc(int start, int end);
	void percolateDown(int node);
	void percolateUp(int node);
	//void heapUpDesc(int child);
	void soundheapIns(const Vec pos, const unsigned int ssource);
	void soundheapCheck();
	#endif //USE_SOUNDHEAP
	void turn_sound_on(int alutinit);
	void turn_sound_off(void);
	void set_source_properties(const ALuint source, const float maxdist, const float roff, const float min_volume, const float max_volume);
	void play_a_sound(ALuint source);
	void play_a_bounce(const Vec pos, const Flt movement, const Flt threshold);
	void stop_a_sound(ALuint source);
	void set_volume(const ALuint source, const float vlevel); // vlevel: 0.0 - 1.0
	void adjust_volume(const ALuint source, const float dist);
	void set_listener();
	void my_load_wav(const char *fname, unsigned char *wavmem, int len, ALuint *source);
	void load_sound(const char *wavfilename, unsigned char *wavmem, int looping, float pitch, float gain);
	//void insert_into_sound_queue(int distance, const Vec pos, unsigned int ssource, int btime);
	void make_wav_mem_image(const char *fname, const int flag);
	void record_sounds(void);
	//void load_nuke_wav(void);
	void set_engine_sound(const int flag);
	void play_sound_var_pitch(const ALuint source);

	void startup_sound(void);
	void sound_set(void);

	ALCcontext *recContext;
	ALCdevice *recDevice;
	ALCdevice *recCaptureDevice;
	int recNumber, recSum, recRecTime, recRecordNow, recTracks, recResolution;
	int recSamplesAvailable;
	int recWholeMemorySpace;
	char *recAmem;

};

#endif //_OALSOUND_H_
