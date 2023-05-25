#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include <PAWSlibrary.h>

#include <pwd.h>
#include <sys/file.h>
#include <sys/wait.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <X11/Intrinsic.h>
//#include <X11/StringDefs.h>
//#include <X11/Shell.h>

typedef long LONG;
typedef int Font;

#include "c_config.h"
#include "c_colors.h"
#include "c_defs.h"
#include "c_struct.h"
#include "c_extern.h"

#define TANKDIR "/var/tmp/"

enum {XOFF=50,YOFF=50,XM=7,XD=10,YM=7,YD=10};

typedef struct {
  char name[14];                /* the color will be stored as  */
} Colorname;                    /*      #RRRRGGGGBBBB           */

typedef struct {
  int width;
  int height;
  char* bits;
  unsigned char *p;
  unsigned int color;
} Bmap;

typedef struct {
    short x, y;
} XPoint;

typedef struct {
    short x1, y1, x2, y2;
} XSegment;

typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;

typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
} XArc;

typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  /* do_red, do_green, do_blue */
	char pad;
} XColor;

typedef struct {
	int visualid;	/* visual id of this visual */
#if defined(__cplusplus) || defined(c_plusplus)
	int c_class;		/* C++ class of screen (monochrome, etc.) */
#else
	int class;		/* class of screen (monochrome, etc.) */
#endif
	unsigned long red_mask, green_mask, blue_mask;	/* mask values */
	int bits_per_rgb;	/* log base 2 of distinct color values */
	int map_entries;	/* color map entries */
} Visual;
