//3350
//program: walk2.cpp
//original author:  Gordon Griesel
//date:    fall 21
//
//Walk cycle using a sprite sheet.
//images courtesy: http://games.ucla.edu/resource/walk-cycles/
//
//This program includes:
//  multiple sprite-sheet animations
//  a level tiling system
//  parallax scrolling of backgrounds
//
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
//#include "ppm.h"
#include <GL/glut.h>

#include "fonts.h"
#include "sabdulrazzak.cpp"
#include "dleon.cpp"
#include "ekyles.cpp"
#include "jmedina2.cpp"

//defined types
typedef double Flt;
typedef double Vec[3];
typedef Flt Matrix[4][4];

//macros
//  #define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand() % a)
#define MakeVector(v, x, y, z) (v)[0] = (x), (v)[1] = (y), (v)[2] = (z)
#define VecCopy(a, b) \
	(b)[0] = (a)[0];  \
	(b)[1] = (a)[1];  \
	(b)[2] = (a)[2]
#define VecDot(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define VecSub(a, b, c)       \
	(c)[0] = (a)[0] - (b)[0]; \
	(c)[1] = (a)[1] - (b)[1]; \
	(c)[2] = (a)[2] - (b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define ALPHA 1
#define PI 3.141592653589793

//function prototypes
void initOpengl();
void checkMouse(XEvent *e);
int checkKeys(XEvent *e);
void init();
void physics();
void render();

//-----------------------------------------------------------------------------
//Setup timers
class Timers
{
public:
	double physicsRate;
	double oobillion;
	struct timespec timeStart, timeEnd, timeCurrent;
	struct timespec walkTime;
	Timers()
	{
		physicsRate = 1.0 / 30.0;
		oobillion = 1.0 / 1e9;
	}
	double timeDiff(struct timespec *start, struct timespec *end)
	{
		return (double)(end->tv_sec - start->tv_sec) +
			   (double)(end->tv_nsec - start->tv_nsec) * oobillion;
	}
	void timeCopy(struct timespec *dest, struct timespec *source)
	{
		memcpy(dest, source, sizeof(struct timespec));
	}
	void recordTime(struct timespec *t)
	{
		clock_gettime(CLOCK_REALTIME, t);
	}
} timers;
//-----------------------------------------------------------------------------

class Image;

//---------Set up Monster Class
class Monster
{
public:
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[4];
	float angle;
	float color[3];
	struct Monster *prev;
	struct Monster *next;

public:
	Monster()
	{
		prev = NULL;
		next = NULL;
	}
};


class Game
{
public:
	Monster *ahead;
	int nMonsters;

public:
	Game()
	{
		ahead = NULL;
		nMonsters = 0;
		//build 10 Monsters...
		for (int j = 0; j < 10; j++)
		{
			Monster *a = new Monster;
			a->nverts = 4;
			a->radius = rnd() * 80.0 + 40.0;
			Flt r2 = a->radius / 2.0;
			Flt angle = 0.0f;
			Flt inc = (PI * 2.0) / (Flt)a->nverts;
			for (int i = 0; i < a->nverts; i++)
			{
				
				a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
				a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
				angle += inc;
			}
			a->pos[0] = (Flt)(rand() % 800);
			a->pos[1] = (Flt)(rand() % 600);
			a->pos[2] = 0.0f;
			a->angle = 0.0;
			a->color[0] = 255;
			a->color[1] = 215;
			a->color[2] = 0;
			a->vel[0] = (Flt)(rnd() * 2.0 - 1.0);
			a->vel[1] = (Flt)(rnd() * 2.0 - 1.0);
			//std::cout << "Monster" << std::endl;
			//add to front of linked list
			a->next = ahead;
			if (ahead != NULL)
				ahead->prev = a;
			ahead = a;
			++nMonsters;
		}
	}
	~Game()
	{
	}
} g;

class Sprite
{
public:
	int onoff;
	int frame;
	double delay;
	Vec pos;
	Image *image;
	GLuint tex;
	struct timespec time;
	Sprite()
	{
		onoff = 0;
		frame = 0;
		image = NULL;
		delay = 0.1;
		pos[0] = 0;
        pos[1] = 0;
		pos[2] = 0; 

	}
};

class Texture
{
public:
	Image *backImage;
	GLuint backTexture;
	float xc[2];
	float yc[2];
};

class Global
{
public:
	int titleSound;
	int gameover;
	int gameScore;
	unsigned char keys[65536];
	int xres, yres;
	int movie, movieStep;
	int walk;
	int creds;
	int title;
	int quit;
	int walkFrame;
	double delay;
	Sprite Rowdy; 
	//declaring the bacground and its texture
	int backgroundFrame;
	//Adding background Texture for title screen
	Image *backgroundImage;
	GLuint backgroundTexture;
	GLuint gameOverText;
	Texture scrollingTexture;

	
	//camera is centered at (0,0) lower-left of screen.
	Flt camera[2];
	~Global()
	{
		logClose();
	}
	Global()
	{
		logOpen();
		camera[0] = camera[1] = 0.0;
		movie = 0;
		title = 0;
		movieStep = 2;
		xres = 1080;
		yres = 900;
		walk = 0;
		walkFrame = 0;
		Rowdy.image = NULL; 
		Rowdy.pos[0] = (Flt)(-(xres)/2) + 200;
        Rowdy.pos[1] = (Flt)(-(yres)/2) + 300;
        Rowdy.pos[2] = 0.0f;

		//walkImage = NULL;
		backgroundFrame = 0;
		backgroundImage = NULL;

		int gameover = 0;
		int gameScore = 0;
		int quit = 0;
		int titleSound = 1;

	
		delay = 0.1;
		memset(keys, 0, 65536);
	}
} gl;



void deleteMonster(Game *g, Monster *node)
{
        //Remove a node from doubly-linked list
        //Must look at 4 special cases below.
        if (node->prev == NULL) {
                if (node->next == NULL) {
                        //only 1 item in list.
                        g->ahead = NULL;
                } else {
                        //at beginning of list.
                        node->next->prev = NULL;
                        g->ahead = node->next;
                }
        } else {
                if (node->next == NULL) {
                        //at end of list.
                        node->prev->next = NULL;
                } else {
                        //in middle of list.
                        node->prev->next = node->next;
                        node->next->prev = node->prev;
                }
        }
        delete node;
        node = NULL;
};

//This builds new Asteroid
void buildMonsterFragment(Monster *ta, Monster *a)
{
	//build ta from a
	ta->nverts = 4;
	ta->radius = a->radius / 2.0;
	Flt r2 = ta->radius / 2.0;
	Flt angle = 0.0f;
	Flt inc = (PI * 2.0) / (Flt)ta->nverts;
	for (int i = 0; i < ta->nverts; i++)
	{
		ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
		ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
		angle += inc;
	}
	ta->pos[0] = a->pos[0] + rnd() * 10.0 - 5.0;
	ta->pos[1] = a->pos[1] + rnd() * 10.0 - 5.0;
	ta->pos[2] = 0.0f;
	ta->angle = 0.0;
	ta->color[0] = 0.5;
	ta->color[1] = 0.5;
	ta->color[2] = 0.5;
	ta->vel[0] = a->vel[0] + (rnd() * 2.0 - 1.0);
	ta->vel[1] = a->vel[1] + (rnd() * 2.0 - 1.0);
	//std::cout << "frag" << std::endl;
};

//X Windows variables
class X11_wrapper
{
private:
	Display *dpy;
	Window win;

public:
	~X11_wrapper()
	{
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void setTitle()
	{
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "Rowdy Bros.");
	}
	void setupScreenRes(const int w, const int h)
	{
		gl.xres = w;
		gl.yres = h;
	}
	X11_wrapper()
	{
		GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
		//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
		XSetWindowAttributes swa;
		setupScreenRes(gl.xres, gl.yres);
		dpy = XOpenDisplay(NULL);
		if (dpy == NULL)
		{
			printf("\n\tcannot connect to X server\n\n");
			exit(EXIT_FAILURE);
		}
		Window root = DefaultRootWindow(dpy);
		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
		if (vi == NULL)
		{
			printf("\n\tno appropriate visual found\n\n");
			exit(EXIT_FAILURE);
		}
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
						 StructureNotifyMask | SubstructureNotifyMask;
		win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
							vi->depth, InputOutput, vi->visual,
							CWColormap | CWEventMask, &swa);
		GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
		setTitle();
	}
	void reshapeWindow(int width, int height)
	{
		//window has been resized.
		setupScreenRes(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
		setTitle();
	}
	void checkResize(XEvent *e)
	{
		//The ConfigureNotify is sent by the
		//server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != gl.xres || xce.height != gl.yres)
		{
			//Window size did change.
			reshapeWindow(xce.width, xce.height);
		}
	}
	bool getXPending()
	{
		return XPending(dpy);
	}
	XEvent getXNextEvent()
	{
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
	void swapBuffers()
	{
		glXSwapBuffers(dpy, win);
	}
} x11;

class Image
{
public:
	int width, height;
	unsigned char *data;
	~Image() { delete[] data; }
	Image(const char *fname)
	{
		if (fname[0] == '\0')
			return;
		//printf("fname **%s**\n", fname);
		int ppmFlag = 0;
		char name[40];
		strcpy(name, fname);
		int slen = strlen(name);
		char ppmname[80];
		if (strncmp(name + (slen - 4), ".ppm", 4) == 0)
			ppmFlag = 1;
		if (ppmFlag)
		{
			strcpy(ppmname, name);
		}
		else
		{
			name[slen - 4] = '\0';
			//printf("name **%s**\n", name);
			sprintf(ppmname, "%s.ppm", name);
			//printf("ppmname **%s**\n", ppmname);
			char ts[100];
			//system("convert eball.jpg eball.ppm");
			sprintf(ts, "convert %s %s", fname, ppmname);
			system(ts);
		}
		//sprintf(ts, "%s", name);
		//printf("read ppm **%s**\n", ppmname); fflush(stdout);
		FILE *fpi = fopen(ppmname, "r");
		if (fpi)
		{
			char line[200];
			fgets(line, 200, fpi);
			fgets(line, 200, fpi);
			//skip comments and blank lines
			while (line[0] == '#' || strlen(line) < 2)
				fgets(line, 200, fpi);
			sscanf(line, "%i %i", &width, &height);
			fgets(line, 200, fpi);
			//get pixel data
			int n = width * height * 3;
			data = new unsigned char[n];
			for (int i = 0; i < n; i++)
				data[i] = fgetc(fpi);
			fclose(fpi);
		}
		else
		{
			printf("ERROR opening image: %s\n", ppmname);
			exit(0);
		}
		if (!ppmFlag)
			unlink(ppmname);
	}
};
Image img[6] = {
	"./images/walk.gif",
	"./images/fireBall.png",
	"./images/titleScreen.png",
	"./images/scrollingBackground.jpg",
	"./images/gameOver.jpg",
	"./images/coin8bit.png"};

int main(void)
{

	initOpengl();
	init();

	int done = 0;
	while (!done)
	{
		while (x11.getXPending())
		{
			XEvent e = x11.getXNextEvent();
			x11.checkResize(&e);
			checkMouse(&e);
			done = checkKeys(&e);
		}

		physics();
		render();
		x11.swapBuffers();
	}
	clean_sound();
	cleanup_fonts();
	return 0;
}

unsigned char *buildAlphaData(Image *img)
{
	//add 4th component to RGB stream...
	int i;
	unsigned char *newdata, *ptr;
	unsigned char *data = (unsigned char *)img->data;
	newdata = (unsigned char *)malloc(img->width * img->height * 4);
	ptr = newdata;
	unsigned char a, b, c;
	//use the first pixel in the image as the transparent color.
	unsigned char t0 = *(data + 0);
	unsigned char t1 = *(data + 1);
	unsigned char t2 = *(data + 2);
	for (i = 0; i < img->width * img->height * 3; i += 3)
	{
		a = *(data + 0);
		b = *(data + 1);
		c = *(data + 2);
		*(ptr + 0) = a;
		*(ptr + 1) = b;
		*(ptr + 2) = c;
		*(ptr + 3) = 1;
		if (a == t0 && b == t1 && c == t2)
			*(ptr + 3) = 0;
		ptr += 4;
		data += 3;
	}
	return newdata;
}

void initOpengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, gl.xres, gl.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();

	///Title Screen background set up
	glGenTextures(1, &gl.backgroundTexture);
	int w = img[2].width;
	int h = img[2].height;

	glBindTexture(GL_TEXTURE_2D, gl.backgroundTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img[2].data);
	//-------------------------------------------

	//Game Over Screen
	glGenTextures(1, &gl.gameOverText);
	w = img[4].width;
	h = img[4].height;
	glBindTexture(GL_TEXTURE_2D, gl.gameOverText);
	//unsigned char* ftData = buildAlphaData(&img[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img[4].data);

	//-----------------------------------------------------------
	//Coin image setup
	/*
    glGenTextures(1, &gl.coinTexture);
    w = img[4].width;
    h = img[4].height;
    glBindTexture(GL_TEXTURE_2D, gl.coinTexture);
    //unsigned char* ftData = buildAlphaData(&img[2]); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
     
     glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,0, GL_RGB, GL_UNSIGNED_BYTE, img[4].data);
    */

	/* 

    ////Main Screen Set-up 
    glGenTextures(1, &gl.scrollingTexture);
    w = img[3].width; 
    h = img[3].height; 
    
    glBindTexture(GL_TEXTURE_2D, gl.scrollingTexture);
    //unsigned char* ftData = buildAlphaData(&img[2]); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
     
     glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h,0, GL_RGB, GL_UNSIGNED_BYTE, img[3].data);
     //-------------------------------------------
    */
	gl.scrollingTexture.backImage = &img[3];
	//create opengl texture elements
	glGenTextures(1, &gl.scrollingTexture.backTexture);
	w = gl.scrollingTexture.backImage->width;
	h = gl.scrollingTexture.backImage->height;
	glBindTexture(GL_TEXTURE_2D, gl.scrollingTexture.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
				 GL_RGB, GL_UNSIGNED_BYTE, gl.scrollingTexture.backImage->data);
	gl.scrollingTexture.xc[0] = 0.0;
	gl.scrollingTexture.xc[1] = 0.25;
	gl.scrollingTexture.yc[0] = 0.0;
	gl.scrollingTexture.yc[1] = 1.0;
	////////////////////////////////////////////////////

	//
	//load the images file into a ppm structure.
	//
	w = img[0].width;
	h = img[0].height;
	//
	//create opengl texture elements
	glGenTextures(1, &gl.Rowdy.tex);

	//-------------------------------------------------------------------------
	//silhouette
	//this is similar to a sprite graphic
	//
	glBindTexture(GL_TEXTURE_2D, gl.Rowdy.tex);
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *walkData = buildAlphaData(&img[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, walkData);
	free(walkData);

}

void init()
{
}

void checkMouse(XEvent *e)
{
	//printf("checkMouse()...\n"); fflush(stdout);
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type != ButtonRelease && e->type != ButtonPress &&
		e->type != MotionNotify)
		return;

	int mx = e->xbutton.x;
	int my = e->xbutton.y;

	if (e->type == ButtonRelease)
	{
		return;
	}
	if (e->type == ButtonPress)
	{
		if (e->xbutton.button == 1)
		{
			//Left button is down
			//makeParticle(mx,my,gl.monster, gl.yres);
		}
		if (e->xbutton.button == 3)
		{
			//Right button is down
		}
	}
	if (e->type == MotionNotify)
	{
		if (savex != e->xbutton.x || savey != e->xbutton.y)
		{
			//Mouse moved
			savex = e->xbutton.x;
			savey = e->xbutton.y;

			//for(int i =0; i<5; i++)
			//makeParticle(savex,savey,gl.monster, gl.yres);
		}
	}
}


int checkKeys(XEvent *e)
{
	//keyboard input?
	static int shift = 0;
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	gl.keys[key] = 1;
	if (e->type == KeyRelease)
	{
		gl.keys[key] = 0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift = 0;
		return 0;
	}
	gl.keys[key] = 1;
	if (key == XK_Shift_L || key == XK_Shift_R)
	{
		shift = 1;
		return 0;
	}
	(void)shift;
	switch (key)
	{
	case XK_q:
		gl.gameover = 1;
		//resetGame(gl.gameover, gl.gameScore);
		break;
	case XK_s:
		break;
	case XK_m:
		gl.movie ^= 1;
		break;
	case XK_w:
		timers.recordTime(&timers.walkTime);
		gl.walk ^= 1;
		break;
	case XK_e:
		break;
	case XK_Left:
		break;
	case XK_f:
		break;
	case XK_Right:
		break;
	case XK_Up:
		break;
	case XK_c:
		gl.creds ^= 1;
		break;
	case XK_p:
		gl.title ^= 1;
		//sets game score back to zero
		//resetGame(gl.gameover, gl.gameScore);
		break;
	case XK_Down:
		break;
	case XK_equal:
		gl.delay -= 0.005;
		if (gl.delay < 0.005)
			gl.delay = 0.005;
		break;
	case XK_minus:
		gl.delay += 0.005;
		break;
	case XK_plus:
		gl.delay -= 0.005;
		break;
	case XK_Escape:
		return 1;
		break;
	}
	return 0;
}

Flt VecNormalize(Vec vec)
{
	Flt len, tlen;
	Flt xlen = vec[0];
	Flt ylen = vec[1];
	Flt zlen = vec[2];
	len = xlen * xlen + ylen * ylen + zlen * zlen;
	if (len == 0.0)
	{
		MakeVector(vec, 0.0, 0.0, 1.0);
		return 1.0;
	}
	len = sqrt(len);
	tlen = 1.0 / len;
	vec[0] = xlen * tlen;
	vec[1] = ylen * tlen;
	vec[2] = zlen * tlen;
	return (len);
}

void physics(void)
{
	if (gl.gameover)
		return;
	//to move backgriund when player is in motion
	//if(player in motion){
	//  gl.walk.xc[0]
	//
	//}

	//-------------------------
	
	if (gl.walk || gl.keys[XK_Right] || gl.keys[XK_Left])
	{
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walkTime, &timers.timeCurrent);
		if (timeSpan > gl.delay)
		{
			//advance
			++gl.walkFrame;
			if (gl.walkFrame >= 16)
				gl.walkFrame -= 16;
			timers.recordTime(&timers.walkTime);
		}
		for (int i = 0; i < 20; i++)
		{
			if (gl.keys[XK_Left])
			{	
				//edit this to inscrease sprite speed
				gl.Rowdy.pos[0] -= .5;
				gl.scrollingTexture.xc[0] -= 0.001;
				gl.scrollingTexture.xc[1] -= 0.001;
				if (gl.camera[0] < 0.0)
					gl.camera[0] = 0.0;
			}
			if (gl.keys[XK_Right])
			{
				gl.Rowdy.pos[0] += .5;
				 gl.scrollingTexture.xc[0] += 0.001;
				 gl.scrollingTexture.xc[1] += 0.001;
				if (gl.camera[0] < 0.0)
					gl.camera[0] = 0.0;
			}
			//
			if (gl.keys[XK_Right] && gl.keys[XK_f])
			{
				gl.Rowdy.pos[0] += 1;
				gl.scrollingTexture.xc[0] += 0.01;
				gl.scrollingTexture.xc[1] += 0.01;
				if (gl.camera[0] < 0.0)
					gl.camera[0] = 0.0;
			}

			if (gl.keys[XK_Left] && gl.keys[XK_f])
			{
				gl.Rowdy.pos[0] -= 1;
				gl.scrollingTexture.xc[0] -= 0.01;
				gl.scrollingTexture.xc[1] -= 0.01;
				if (gl.camera[0] < 0.0)
					gl.camera[0] = 0.0;
			}
		}
	}

	//Update Monster positions
	Monster *a = g.ahead;
	while (a)
	{
		a->pos[0] += a->vel[0];
		a->pos[1] += a->vel[1];
		//Check for collision with window edges
		if (a->pos[0] < -100.0)
		{
			a->pos[0] += (float)gl.xres + 200;
		}
		else if (a->pos[0] > (float)gl.xres + 100)
		{
			a->pos[0] -= (float)gl.xres + 200;
		}
		else if (a->pos[1] < -100.0)
		{
			a->pos[1] += (float)gl.yres + 200;
		}
		else if (a->pos[1] > (float)gl.yres + 100)
		{
			a->pos[1] -= (float)gl.yres + 200;
		}
		a = a->next;
	}
	//
	//Monster collision with Sprite?
	//If collision detected:
	//     1. delete the MOnster
	a = g.ahead;
	while (a) {
	        //is there a bullet within its radius?
	        int i=0;
	        while (i < 10) {
	                if ((gl.Rowdy.pos[0]) ==
					 (a->pos[0])) {
	                        //std::cout << "Monster hit." << std::endl;
	                        //this Monster is hit.
	                                Monster *savea = a->next;
	                                deleteMonster(&g, a);
	                                a = savea;
	                                g.nMonsters--;
	                        //delete the bullet... 
	                }
	                i++;
	        }
	        if (a == NULL)
	                break;
	        a = a->next;
	}
	//---------------------------------------------
	//-----------------------

}


void renderSprite()
{
	float cx = gl.xres / 2.0;
	float cy = gl.yres / 2.0;
	float h = 200.0;
	float w = h * 0.5;
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, gl.Rowdy.tex);
	glEnable(GL_ALPHA_TEST);
	glTranslatef(gl.Rowdy.pos[0], gl.Rowdy.pos[1],gl.Rowdy.pos[2]);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255, 255, 255, 255);
	int ix = gl.walkFrame % 8;
	int iy = 0;
	if (gl.walkFrame >= 8)
		iy = 1;
	float fx = (float)ix / 8.0;
	float fy = (float)iy / 2.0;
	glBegin(GL_QUADS);
	if (gl.keys[XK_Left])
	{
		glTexCoord2f(fx + .125, fy + .5);
		glVertex2i(cx - w, cy - h);
		glTexCoord2f(fx + .125, fy);
		glVertex2i(cx - w, cy + h);
		glTexCoord2f(fx, fy);
		glVertex2i(cx + w, cy + h);
		glTexCoord2f(fx, fy + .5);
		glVertex2i(cx + w, cy - h);
	}
	else
	{
		glTexCoord2f(fx, fy + .5);
		glVertex2i(cx - w, cy - h);
		glTexCoord2f(fx, fy);
		glVertex2i(cx - w, cy + h);
		glTexCoord2f(fx + .125, fy);
		glVertex2i(cx + w, cy + h);
		glTexCoord2f(fx + .125, fy + .5);
		glVertex2i(cx + w, cy - h);
	}
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
}

void renderScreenText()
{
	Rect r;
	unsigned int c = 0x00ffff44;
	r.bot = gl.yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, c, "E   Explosion");
	ggprint8b(&r, 16, c, "+   faster");
	ggprint8b(&r, 16, c, "-   slower");
	ggprint8b(&r, 16, c, "right arrow -> walk right");
	ggprint8b(&r, 16, c, "right arrow + f -> walk faster");
	ggprint8b(&r, 16, c, "left arrow  <- walk left");
	ggprint8b(&r, 16, c, "Press C for credits");
	ggprint8b(&r, 16, c, "Game Score: %i", gl.gameScore);
	ggprint8b(&r, 16, c, "Press Q to quit");
}

void renderTitleScreen()
{
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2i(-gl.xres, gl.yres);
	glVertex2i(-gl.xres, -gl.yres);
	glVertex2i(gl.xres, -gl.yres);
	glVertex2i(gl.xres, gl.yres);
	glEnd();
	glPopMatrix();
	show_title(gl.yres, gl.xres, gl.backgroundTexture);
	play_sound();
}
void renderGameOverScreen()
{
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2i(-gl.xres, gl.yres);
	glVertex2i(-gl.xres, -gl.yres);
	glVertex2i(gl.xres, -gl.yres);
	glVertex2i(gl.xres, gl.yres);
	glEnd();
	glPopMatrix();
	show_title(gl.yres, gl.xres, gl.gameOverText);
}



void renderCredits()
{
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2i(-gl.xres, gl.yres);
	glVertex2i(-gl.xres, -gl.yres);
	glVertex2i(gl.xres, -gl.yres);
	glVertex2i(gl.xres, gl.yres);
	glEnd();
	glPopMatrix();
	show_my_creds(gl.yres / 2, gl.xres / 2);
	show_ed_creds((gl.yres / 2) + 15, gl.xres / 2);
	show_diego_creds((gl.yres / 2) + 30, gl.xres / 2);
	show_javier_creds((gl.yres / 2) + 45, gl.xres / 2);
}

void render(void)
{
	show_background(gl.yres, gl.xres, gl.scrollingTexture.backTexture, gl.scrollingTexture.xc, gl.scrollingTexture.yc);
	renderSprite();
	renderScreenText();
	//--------------
	//Draw the Monsters
	{
		Monster *a = g.ahead;
		while (a)
		{
			//Log("draw Monster...\n");
			glColor3fv(a->color);
			glPushMatrix();
			glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
			glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
			glBegin(GL_POLYGON);
			//glBegin(GL_LINE_LOOP);
			//Log("%i verts\n",a->nverts);
			for (int j = 0; j < a->nverts; j++)
			{
				glVertex2f(a->vert[j][0], a->vert[j][1]);
			}
			glEnd();
			//glBegin(GL_LINES);
			//      glVertex2f(0,   0);
			//      glVertex2f(a->radius, 0);
			//glEnd();
			glPopMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_POINTS);
			glVertex2f(a->pos[0], a->pos[1]);
			glEnd();
			a = a->next;
		}
	}



	if (!gl.title)
	{
		renderTitleScreen();
	}
	if (gl.gameover)
	{
		renderGameOverScreen();
	}
	if (gl.creds)
	{
		renderCredits();
	}
}
