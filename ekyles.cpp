// Edward Kyles
// Software Engineering
// 10/1/21

#include "fonts.h"

void show_ed_creds(int n, int m) {
	//establish centering 
	//using yres and xres
	//
	Rect r;
	r.bot = n; 
	r.left = m; 
	r.center = 1; 
	//print the creds
	ggprint8b(&r, 16, 0x00ff0000, "Ed did some work on the proj"); 

}


void show_title(int n, int m) {
	//establish centering 
	//using yres and xres
	//
	Rect r;
	r.bot = n; 
	r.left = m; 
	r.center = 1;
	//print the creds
	ggprint8b(&r, 16, 0xffff00, "Press P to Play"); 
}
