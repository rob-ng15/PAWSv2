#include <PAWSlibrary.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <pwd.h>
#include <sys/file.h>
#include <sys/wait.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <X11/Intrinsic.h>
//#include <X11/StringDefs.h>
//#include <X11/Shell.h>

typedef long LONG;

#include "c_config.h"
#include "c_colors.h"
#include "c_defs.h"
#include "c_struct.h"
#include "c_extern.h"

#define TANKDIR "/var/tmp/"

enum {XOFF=50,YOFF=50,XM=7,XD=10,YM=7,YD=10};
