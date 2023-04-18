#include <windows.h>

//Temporary fixes added by Eric Fogelin

#define Bool int
#define False 0
#define None 0
#define True 1
#define Font int
#define TANKDIR "."
#define fork() 0 // HACK HACK disable fork
#ifdef WIN31
#define printf();
#endif

#include <stdlib.h>
#include <string.h>
#include <process.h>

#define random rand
#define srandom srand

// HACK for mono case.	Used by setmonomap()
#define BlackPixelOfScreen(screen) 0x00000000
#define WhitePixelOfScreen(screen) 0x0000FF00

struct timeval {
    int tv_sec;
    int tv_usec;
};

typedef struct {
    short x1, y1, x2, y2;
} XSegment;

typedef struct {
    short x, y, width, height;
} XRectangle;

typedef struct {
    short x, y;
} XPoint;

typedef struct {
    int vkey;
    short prevstate;
    int vkeydown;
    int vkeyup;
} VKEY;

typedef int Pixel;

typedef int Pixmap;
typedef int Window;
typedef int Display;
typedef int Screen;

typedef int Colormap;
typedef int Visual;
typedef int GC;

extern HDC hdc;
extern HDC hpen[];
extern HBRUSH hbrushBlack;
extern HBRUSH hbrushFill;
extern LPSTR lpszCmdLine;

#define NUMVKEYS 3
#define KEYDOWN 0x8000
extern VKEY vkey[];

LONG scores( LONG score);
void gettimeofday( struct timeval *unixtime, int dummy );
void select( int dummy1, int dummy2, int dummy3, int dummy4, struct timeval *unixtime );
//void cbzone_main(int argc, char *argv);

#define exit return

//End Temporary fixes added by Eric Fogelin
