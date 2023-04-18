#include "c_includ.h"
/*
 * cbzone_parseopts.c
 *  -- Todd W Mummert, December 1990, CMU
 *
 * RCS Info
 *  $Header: c_parseopts.c,v 1.1 91/01/12 02:03:36 mummert Locked $
 *
 * Parse the options, read the MOTD, etc...
 *
 * The prints in these routines will never go the game window, as it
 * does not exist.  cbzone cannot be backgrounded from the beginning
 * since the execl will block on tty output.  To allow this we create
 * yet another flag that specifies that motd is not to be read.
 */
 
int pager(file)
     char* file;
{
  char buf[100], *pager, *getenv();
  static char defaultpager[] = PAGER;
  FILE *f;

  if ((pager = getenv("PAGER")) == NULL)
    pager = defaultpager;
  sprintf(buf,"%s%s",TANKDIR,file);
  if ((f=fopen(buf,"r")) != NULL) {
    fclose(f);
    switch (fork()) {
    case 0:
#ifndef WIN31
      execl(pager,pager,buf,0);
#endif
      fprintf(stderr,"Exec of %s failed\n", pager);
      return 1;
    case -1:
      fprintf(stderr,"Unable to fork process\n");
      return 1;
    default:
#ifndef WIN31
      wait(0);
#endif
      break;
    }
  }
  else {
    fprintf(stderr,"File %s not found or unreadable.\n", buf);
    return 1;
  }
  return 0;
}
 
int getoptionint(s)
     char *s;
{
  char rest[100];
  int num;

  if (sscanf(s, "%d%s", &num, rest) != 1) {
    printf("Error in optional argument %s; use -help for help.\n",
           s);
    exit(0);
  }
  return(num);
}
 
/*
 * the following routine may be called in one of two ways...
 *  either w/ the display set or without...if without, then we
 *  will need to parse all the options...otherwise the resources should
 *  have taken care of most of them for us.
 *
 *  now even if the display is set, we may get options that were
 *  ambiguous.
 *
 *  since options are more than one letter, we can't use getopt.
 */
#define MAXOPTIONS 16
#define OPTIONINT 7
void parseopt(argc, argv, status)
     int argc;
     char* argv[];
     Bool status;
{
  int i;
  Bool early_exit = False;

  static char* optionnames[] = {
    "-xrm", "-delay", "-blocks", "-landers", "-tanks", "-missiles",
    "-salvos", "-coptersonly", "-quiet", "-scores", "-original",
    "-version", "-help", "-nooutput", "-mono", "-cursor",
    "-defaultcolormap", "-nofullscreen"};

#ifdef WIN32
  // Eric Fogelin: Fixed options (original game, in mono color, quiet)
  opt->mono = True;
  opt->loud = False;

  opt->mblocks = 8;
  opt->mlanders = 1;
  opt->mtanks = 1;
  opt->practice = True;

#else //X11
  for (argc--, argv++; argc>0; argc--, argv++) {
    for (i=0; i<MAXOPTIONS; i++)
      if (!strncmp(*argv,optionnames[i],strlen(*argv)))
        break;
    if (i < OPTIONINT) {
      argc--; argv++;
    }
    switch(i) {
    case 0:                     /* xrm */
      break;
    case 1:                     /* delay */
      opt->delay = getoptionint(*argv);
      break;
    case 2:                     /* blocks*/
      opt->mblocks = getoptionint(*argv);
      break;
    case 3:                     /* landers */
      opt->mlanders = getoptionint(*argv);
      break;
    case 4:                     /* tanks */
      opt->mtanks = getoptionint(*argv);
      break;
    case 5:                     /* missiles */
      opt->mmissiles = getoptionint(*argv);
      break;
    case 6:                     /* salvos */
      opt->msalvos = getoptionint(*argv);
      break;
    case 7:                     /* copter practice */
      opt->copters = True;
      break;
    case 8:                     /* quiet mode */
      opt->loud = False;
      break;
    case 9:                     /* scores only */
      opt->scores = True;
      break;
    case 10:                    /* original */
      opt->original = True;
      break;
    case 11:                    /* version */
      opt->version = True;
      break;
    case 12:                    /* help */
      opt->help = True;
      break;
    case 13:                    /* nooutput */
      opt->output = False;
      break;
    case 14:                    /* monocolor */
      opt->mono = True;
      break;
    case 15:                    /* cursor */
      opt->cursor = True;
      break;
    case 16:                    /* default colormap */
      opt->defaultcolormap = True;
      break;
    case 17:                    /* fullscreen */
      opt->fullscreen = False;
      break;
    }
  }
#endif //X11

  if (opt->scores || opt->help || opt->version)
    early_exit = True;

  if (opt->output) {
    pager("cbzone.motd");

    if (opt->scores)
      scores(-1);

    if (opt->version)
      printf("\nVersion \"%s\"\n", VERSION);

    if (opt->help && pager("cbzone.help"))
      printf("Sorry help information not available.\n");
  }

  if (early_exit)
#ifdef WIN32
     return;
#else //X11
    exit(0);
#endif

  if (!status)
    return;

#if 0 // Eric Fogelin: Allow copters and tanks
  if (opt->copters)
    opt->mtanks = 0;

  if (opt->original) {
    opt->mblocks = 8;
    opt->copters = False;
    opt->mlanders = 1;
    opt->mmissiles = 1;
    opt->mtanks = 1;
    opt->practice = True;
    opt->msalvos = 1;
  }
#endif

  opt->menemies = (opt->mtanks > opt->mmissiles ?
                   opt->mtanks : opt->mmissiles);
  if (!opt->menemies) {
    printf("Must have at least one missile or tank.\n");
#ifdef WIN32
     return;
#else //X11
     exit(1);
#endif
  }

  if (opt->msalvos == -1)
    opt->msalvos = opt->menemies;
  opt->mobjects = opt->mblocks + opt->mlanders + 2*opt->menemies +
    opt->msalvos + 1;
  opt->estart = 1;
  opt->lstart = opt->estart + opt->menemies;
  opt->sstart = opt->lstart + opt->mlanders;
  opt->bstart = opt->sstart + opt->menemies + opt->msalvos;

  if (opt->mmissiles == MMISSILES &&
      opt->mtanks == MTANKS &&
      opt->mlanders == MLANDERS &&
      opt->mblocks == MBLOCKS &&
      opt->delay <= DELAY &&
      opt->msalvos == opt->menemies)
    opt->practice = False;
  else
    opt->practice = True;
}
