#include "c_includ.h"
#ifdef WIN31
#include <toolhelp.h>
#endif

HPEN hpen[MAX_COLORS];
HBRUSH hbrushBlack;
HBRUSH hbrushFill;

VKEY vkey[] = { VK_LBUTTON, 0, 'a', 'A',
		'Q', 0, 'Q', 'Q',
		VK_RBUTTON, 0, 'c', 'C' };

// Stub out c_score.c by Eric Fogelin
LONG scores( LONG score)
{
    return 0;
}
/*
*/

// Implement gettimeofday by Eric Fogelin
void gettimeofday( struct timeval *unixtime, int dummy )
{
#ifdef WIN32
    unsigned int millisecs;

    millisecs = GetTickCount();
    unixtime->tv_sec = millisecs / 1000;            // seconds
    unixtime->tv_usec = (millisecs % 1000) * 1000;  // micro seconds
#else
    time_t secs;

    unixtime->tv_sec = secs;
    unixtime->tv_usec = 0;
#endif
}
/*
*/

#ifdef WIN31
void Sleep( DWORD msec )
{
    DWORD endtime;
    // TIMERINFO ti;
    BOOL done;

    // TimerCount( &ti );
    endtime = GetTickCount() + msec;
    do {
	done = ( GetTickCount() >= endtime);
	// TimerCount( &ti );
	// done = ( ti.dwmsSinceStart >= endtime);
    } while (!done);
}
/*
*/
#endif //WIN31

// Implement select by Eric Fogelin
// select appears to be used as a fine grain delay (microseconds)
void select( int dummy1, int dummy2, int dummy3, int dummy4, struct timeval *unixtime )
{
#ifdef WIN32
    // Convert from micro to milliseconds
    Sleep( unixtime->tv_sec * 1000 + unixtime->tv_usec/1000 );
#endif
}
/*
*/
