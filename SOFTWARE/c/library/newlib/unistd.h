#ifndef _UNISTD_H_
#define _UNISTD_H_

# include <sys/unistd.h>

// PAWS NEWLIB PATCHED unistd.h
// RE-DIRECTS OPERATIONS TO PAWS LIBRARY

#define rmdir(a)        paws_rmdir(a)
extern int paws_rmdir (const char *__path);

#endif /* _UNISTD_H_ */
