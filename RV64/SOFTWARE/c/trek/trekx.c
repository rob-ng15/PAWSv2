/*
 * Trek 73, translated from Data General BASIC to Turbo Pascal 3.0
 * and finally to C, by Howard Chu. Turbo Pascal version completed
 * Fall/Winter 1985. Author of BASIC program unknown. C version of
 * November 1990.
 *
 * This program is copyright 1990 by Howard Chu. All rights reserved.
 */

#include <ncurses.h>		/* Also pulls in stdio and sgtty */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

#define	reg	register

#if	!GEMDOS
#include <sys/types.h>
#include <sys/time.h>		/* For select() call */
#else
long	_stksize=32768L;
#define	M_PI		PI
#define	toupper		_toupper	/* MWC is weird */
#define	tolower		_tolower
#endif

#define	boolean		char
#define	Char		char
#define	Local		static
#define	Static		static
#define	Void		void
#define AryMax          100
typedef	unsigned char	uchar;
#ifndef true
#define	true		(1)
#define	false		(0)
#endif
#define	Malloc		malloc

#include <PAWSlibrary.h>

typedef struct Object {
  double Act_Spd, Des_Spd, Act_Crs, Des_Crs, Fuel, Prx_Del, Tim_Del, XPos,
	 YPos;
  short Trgt, ObTyp;
} Object;

typedef Object *Olist[AryMax];
typedef enum {
  Comp, Snsr, Prob, Warp, Engn
} Damages;

typedef struct Srec {
  double Regen, Weff, Option;
  uchar Fcap;
  short Crew;
  double PChrg[4], PCrts[4], Pdepl[4], Sdrns[4], Seff[4];
  boolean Dmge[5];
  short Tengy[6], Pstat[6];
  double TDepl[6];
  short Tstat[9];
} Srec;

typedef Srec *Slist[10];
typedef int Digs[10];

double Iter = 0.25, SimTim = 2.0;
uchar Phasers = 4, Tubes = 6;

char *DText[] = {
  "Computer destroyed", "Sensors annihilated", "Probe Launcher crushed",
  "Warp Drive disabled"
};

char *Male = "Sir";
char *Female = "Miss";
Char Namebuf[21], *Title, *Enemy, *Capt;
char *Snams[10], *Name;
Slist Ships;
Olist Objects;
uchar Ct[4];
uchar Win, Enemies, Vessels, Timer, ObjCount;
int HelpX;

char *Badguys[] = {
  "Barilion", "Crion", "Displacer", "Kahlos", "Lamphry", "Marauder", "Phobos",
  "Xorn", "Zircon"
};

char *Feds[] = {
  "Constitution", "Enterprise", "Exeter", "Hornet", "Independence",
  "Lexington", "Merrimac", "Potemkin", "Republic", "Trenton", "Wasp",
  "Yorktown"
};

char *EnemNms[] = {
  "Gorn", "Tholian", "Klingon", "Romulan", "Orion"
};
#define Romulan EnemNms[3]

char *Slurs[] = {
  "Fruit", "Slime", "Twerp", "Scuzz", "Swine"
};

char *Captains[] = {
  "Bolak", "Kang", "Koloth", "Kor", "Korax", "Krulix", "Quarlo", "Tal",
  "Tolan", "Troblak"
};

char *Missions[] = {
  " We are acting in response to a priority 1 distress call from",
  "space station K7.",
  " We are orbiting Gamma 2 to make a routine check of automatic",
  "communications and astrogation stations.",
  " We are on course for Epsilon Canares 3 to treat Commissioner",
  "Headford for Sukaro's disease.",
  " We have been assigned to transport Federation ambassadors to",
  "the planet conference, code named BABEL, on Tellarite Andorean.",
  " Our mission is to investigate a find of tritanium on Beta 7.",
  " We are orbiting Rigel 4 for therapeutic shore leave.",
  " We are orbiting Sigma Iota 2 to study the effects of",
  "contamination upon a developing culture.",
  " We have altered course for a rescue mission in the Gamma 7A", "system.",
  " We are presently on course for Altair 6 to attend inauguration",
  "ceremonies on that planet.",
  " We are on a cartographic mission to Polex 9.",
  " We are headed for Malurian in response to a distress call from",
  "that system.",
  " We are to negotiate a treaty to mine dilithium crystals from",
  "the Haulkins.",
  " We are to investigate strange sensor readings reported by a",
  "scoutship investigating Gamma Triangula 6.",
  " We are headed for planets L370 and L374 to investigate the",
  "disappearance of the starship Constellation in that vicinity.",
  " We are ordered, with a skeleton crew, to proceed to space",
  "station K2 to test Dr. Richard Daystrom's computer M-5.",
  " We have encountered debris from the SS Beagle and are",
  "proceeding to investigate.",
  " We are on course for Ekos to locate John Guild.",
  " We are to divert an asteroid from destroying an inhabited", "planet.",
  " We are responding to a distress call from the scientific",
  "expedition on Triacus.",
  " We have been assigned to transport the Medusan ambassador to",
  "his home planet.", " ***End of Missions***"
};

Damages CC;
short Shps[10];
char	scrstr[256];

void Delay(int msec)
{
    set_timer1khz( (short)msec, 0 );
    while( get_timer1khz( 0 ) && !ps2_character_available() );
}

int _randint(range)
int range;
{
  return ((rand()>>3)%range);
}

/*
 * Get a string with echo and erase and kill character processing
 */

char _Erase, _Kill;

void mygetstr(win, str, eol)
WINDOW *win;
char *str;
boolean eol;
{
  reg char c, *ptr;
  int y, x, y1, x1;
  reg int i=0;

  ptr = str;
  getyx(win, y1, x1);
  wrefresh(win);
  do {
    c=wgetch(win);
    if ((c == _Erase) && i) {
      ptr--;
      i--;
      getyx(win, y, x);
      mvwaddch(win, y, x-1, ' ');
    } else if (c == _Kill) {
      ptr=str;
      i=0;
      wmove(win, y1, x1);
      wclrtoeol(win);
    } else if ((c == '\n') || (c=='\r')) {
      *ptr = '\0';
      if (eol) {
        waddstr(win, "\n");
	wclrtoeol(win);
      }
      wrefresh(win);
      return;
    } else {
      *ptr++ = c;
      waddch(win, c);
    }
    wrefresh(win);
  } while (c != '\n');
}

WINDOW *Radwin, *Repwin, *Diswin, *Cmdwin, *Scnwin, *Prbwin;
int Cmdrow, Disrow, Discol, DisXmid, DisYmid;
boolean cheat;

void SysInit()
{
  srand(getpid());
  initscr();

  if ((LINES < 24) || (COLS < 80)) {
	printf("Sorry, you must have at least a 24 by 80 screen to play.\n");
	exit(1);
  }

  _Erase=erasechar();
  _Kill=killchar();
  noecho();
  raw();
  nonl();

  Disrow = LINES * 19 / 24;
  Cmdrow = LINES - Disrow;
  if (!(Disrow & 1)) {
	Disrow++;	/* Insure Disrow is odd */
	Cmdrow--;
  }
  Discol = COLS - 45;
  if (!(Discol & 1)) {
	Discol--;
  }
}

void TrekInit()
{
  register int I, J, K;
  int FORLIM;
  double Junk;
  Object *WITH;
  Srec *WITH1;
  int FORLIM1;
  WINDOW *Twin, *subwin();
  boolean Intro, Ok;
  char *Trash, *Temp, *Temp2, c;

  clear();
  mvaddstr(0,31,"*** Star Trek ***");
  mvaddstr(2,29,"Version 1.01 - curses:");
  refresh();
  mvaddstr(3,10,
	 "\"Novice\" has 60 seconds for response, otherwise 10 seconds.");

  mvaddstr(6,0,"Captain: My last name is ");
  do {
    move(6,25);
    clrtoeol();
    mygetstr(stdscr,scrstr,true);
     J = strlen(scrstr);
  } while (J <= 1);
  Name=Namebuf;
  strncpy(Name,scrstr,20);

  cheat = false;
  if (Name[0] == '*' || Name[0] == '!') {
    if (Name[0] == '!')
	cheat = true;
    Name++;
    J--;
    Intro = false;
  } else
    Intro = true;
  for (I = 0; I < J; I++) {
    if (isupper(Name[I]))
      Name[I] = tolower(Name[I]);
  }
  if (!strcmp(Name, "novice"))
    Timer = 59;
  else
    Timer = 10;
  Name[0] = toupper(Name[0]);

  mvaddstr(8,0, Name);
  addstr(": My sex is ");
  mygetstr(stdscr,scrstr,true);
  scrstr[0] = toupper(scrstr[0]);
  if (scrstr[0] == 'M')
    Title = Male;
  else if (scrstr[0] == 'F')
    Title = Female;
  else
    Title = Slurs[_randint(5)];
  do {
    Ok = true;
    move(9,3);
    addstr("I'm expecting [1-9] enemy vessels: ");
    clrtobot();
    mygetstr(stdscr,scrstr,true);
    I=atoi(scrstr);
    if (I < 1 || I > 9) {
      Ok = false;
      sprintf(scrstr,"Uhura: %s, Starfleet Command reports that it can only\n",
	     Title);
      mvaddstr(11,0,scrstr);
      addstr("   be from 1 to 9.\n\n");
      refresh();

      Delay(500L);

      addstr(Name);
      addstr(": Correct, Lieutenant - just testing your attention.");
      refresh();
      getch();
    }
  } while (!Ok);
  Enemies = I;
  Vessels = Enemies + 1;
  ObjCount = Vessels;
  for (I = 0; I < 4; I++)
    Ct[I] = 0;
  Win = 0;
  for (I = 0; I < 100; I++)
    Objects[I] = NULL;
  FORLIM = Vessels;
  for (I = 0; I < FORLIM; I++) {
    Objects[I] = (Object *)Malloc(sizeof(Object));
    WITH = Objects[I];
    WITH->Act_Spd = 0.0;
    WITH->Des_Spd = 0.0;
    WITH->Act_Crs = 0.0;
    WITH->Des_Crs = 0.0;
    WITH->Fuel = 150.0;
    WITH->Tim_Del = 0.0;
    WITH->Prx_Del = 0.0;
    WITH->XPos = 0.0;
    WITH->YPos = 0.0;
    WITH->Trgt = 0;
    WITH->ObTyp = I;
  }
  for (I = 0; I < 10; I++)
    Shps[I] = I;
  for (I = 1; I < 9; I++) {
    J = _randint(10 - I) + I;
    K = Shps[I];
    Shps[I] = Shps[J];
    Shps[J] = K;
  }
  FORLIM = Vessels;
  for (I = 1; I < FORLIM; I++)
    Snams[I] = Badguys[Shps[I]-1];
  Snams[0] = Feds[_randint(12)];

  for (I = 1; I < FORLIM; I++)
    Objects[I]->ObTyp = Shps[I];
  Enemy = EnemNms[_randint(5)];
  Capt = Captains[_randint(10)];
  Twin = subwin(stdscr, 0, 0, 0, 7);
  scrollok(Twin,TRUE);

  clear();
  refresh();
  if (Intro) {
    mvwaddstr(Twin,1,6,"Space, the final frontier.");
    mvwaddstr(Twin,2,6,"These are the voyages of the Starship ** ");
    waddstr(Twin,Snams[0]);
    waddstr(Twin," **.");
    mvwaddstr(Twin,3,6,"Its five year mission: To explore strange new worlds,");
    mvwaddstr(Twin,4,6,"to seek out new life and new civilizations,");
    mvwaddstr(Twin,5,6,"to boldly go where no man has gone before!");
    mvwaddstr(Twin,7,24,"S T A R   T R E K");
    wmove(Twin,8,0);
    wrefresh(Twin);
    Delay(3000);

    Junk = (_randint(50) + 50) * 100 + _randint(1000) / 10.0;
    sprintf(scrstr,"\nCaptain's Log, stardate %6.1f\n\n", Junk);
    waddstr(Twin, scrstr);

    J = 0;
    FORLIM = _randint(20);
    for (I = 0; I <= FORLIM;) {
      if (Missions[J][0] == ' ')
	 I++;
      J++;
    }
    Temp = Missions[J-1];
    sprintf(scrstr,"  %s\n", Temp);
    waddstr(Twin, scrstr);
    do {
      Temp = Missions[J];
      J++;
      if (Temp[0] != ' ') {
	sprintf(scrstr,"   %s\n", Temp);
        waddstr(Twin,scrstr);
      }
    } while (Temp[0] != ' ');
    c="a23456789"[Enemies - 1];
    if (Enemies > 1)
      Trash = "s";
    else
      Trash = "";
    wrefresh(Twin);
    Delay(2000);
    sprintf(scrstr,"\nSulu: %s, I'm picking up %c vessel%s on interception\n",
	   Title, c, Trash);
    waddstr(Twin,scrstr);
    sprintf(scrstr,"   course%s with the %s.\n\n", Trash, Snams[0]);
    waddstr(Twin,scrstr);
    wrefresh(Twin);
    Delay(1500);
    if (Enemies > 1) {
      Temp = "them";
      Temp2 = "";
    } else {
      Temp = "it";
      Temp2 = "a ";
    }
    sprintf(scrstr,"Spock: Sensors identify %s as %s%s battle\n", Temp, Temp2,
	 Enemy);
    waddstr(Twin,scrstr);
    sprintf(scrstr,"   cruiser%s, probably under the command of Captain %s.\n",
	   Trash, Capt);
    waddstr(Twin,scrstr);
    wrefresh(Twin);
    Delay(1000);
    sprintf(scrstr,"\n%s: Sound General Quarters, Lieutenant!\n", Name);
    waddstr(Twin,scrstr);
    wrefresh(Twin);
    Delay(1000);
    sprintf(scrstr,"\nUhura: Aye, %s!\n", Title);
    waddstr(Twin,scrstr);
    wrefresh(Twin);
    Delay(1000);
  }
  sprintf(scrstr,"\nComputer: The %ss are attacking the %s with the\n",
	 Enemy, Snams[0]);
  waddstr(Twin,scrstr);
  waddstr(Twin,"  ");
  FORLIM = Vessels;
  for (I = 1; I < FORLIM; I++) {
    if (I == 7)
      waddstr(Twin,"\n  ");
    waddch(Twin,' ');
    waddstr(Twin,Snams[I]);
    if (I != Enemies) {
      if (Enemies > 2)
	waddch(Twin,',');
      if (I == Vessels - 2)
	waddstr(Twin," and the");
    }
  }
  waddstr(Twin,".\n\n\n[Press a key to continue]");
  wrefresh(Twin);
  getch();
  for (I = 0; I < 10; I++)
    Ships[I] = NULL;
  FORLIM = Vessels;
  for (I = 0; I < FORLIM; I++) {
    Ships[I] = (Srec *)Malloc(sizeof(Srec));
    WITH1 = Ships[I];
    WITH1->Weff = 0.75;
    WITH1->Fcap = 200;
    WITH1->Regen = 10.0;
    FORLIM1 = Phasers;
    for (J = 0; J < FORLIM1; J++) {
      WITH1->PChrg[J] = 10.0;
      WITH1->PCrts[J] = 10.0;
      WITH1->Pstat[J] = 0;
    }
    WITH1->Pstat[Phasers] = 10;
    WITH1->Pstat[Phasers + 1] = 100;
    WITH1->Pdepl[0] = 90.0;
    WITH1->Pdepl[1] = 0.0;
    WITH1->Pdepl[2] = 0.0;
    WITH1->Pdepl[3] = 270.0;
    for (J = 0; J < 6; J++) {
      WITH1->Tstat[J] = 0;
      WITH1->Tengy[J] = 0;
    }
    WITH1->Tstat[Tubes] = 12;
    WITH1->Tstat[Tubes + 1] = 10;
    WITH1->Tstat[Tubes + 2] = 200;
    WITH1->TDepl[0] = 120.0;
    WITH1->TDepl[1] = 60.0;
    WITH1->TDepl[2] = 0.0;
    WITH1->TDepl[3] = 0.0;
    WITH1->TDepl[4] = 300.0;
    WITH1->TDepl[5] = 240.0;
    for (CC = Comp; (long)CC <= (long)Engn; CC = (Damages)((long)CC + 1))
      WITH1->Dmge[(long)CC] = false;
    for (J = 0; J < 4; J++) {
      WITH1->Sdrns[J] = 1.0;
      WITH1->Seff[J] = 100.0;
    }
    WITH1->Crew = 350;
  }
  WITH1 = Ships[0];
  WITH1->Fcap = 250;
  WITH1->Crew = 450;
  WITH1->Weff = 1.0;
  FORLIM = Vessels;
  for (I = 0; I < FORLIM; I++) {
    WITH = Objects[I];
    WITH->Act_Spd = 1.0;
    WITH->Des_Spd = 1.0;
    WITH->Act_Crs = _randint(360);
    WITH->Des_Crs = WITH->Act_Crs;
    WITH->Trgt = 0;
    WITH->Tim_Del = 1.0;
    WITH->Prx_Del = 0.0;
    J = _randint(500) + 4300;
    Junk = _randint(360) * M_PI / 180.0;
    WITH->XPos = J * cos(Junk);
    WITH->YPos = -J * sin(Junk);
  }
  WITH = Objects[0];
  WITH->Act_Crs = 0.0;
  WITH->Des_Crs = 0.0;
  WITH->Tim_Del = 0.0;
  WITH->Fuel = 200.0;
  WITH->XPos = 0.0;
  WITH->YPos = 0.0;
  delwin(Twin);
}

/* End of trek0, start of trek1 */

bool Stop, Bombed, Redo;
int LKnt, TCount, Radius;
char Ch;

void DrawRep()
{
  int i, j;

  werase(Repwin);
  mvwaddstr(Repwin,0,14,"Warp Course Bearing Range");
  j=1;
  for (i=0; i<=Enemies; i++)
    if (Ships[i]) {
      sprintf(scrstr,"%12s:",Snams[i]);
      mvwaddstr(Repwin,j,0,scrstr);
      j++;
    }
}

void DrawBox(win, rows, cols)
WINDOW *win;
int rows, cols;
{
  int i, j;

  rows--;
  cols--;
  wmove(win, 0, 1);
  for (i=1; i<cols; i++)
    waddch(win,ACS_HLINE);
  j=cols;
  for (i=1; i<rows; i++)
  {
    mvwaddch(win, i, j, ACS_VLINE);
    mvwaddch(win, i, 0, ACS_VLINE);
  }
  wmove(win, i, 1);
  for (i=1; i<cols; i++)
    waddch(win,ACS_HLINE);
}

void DrawScr()
{
  WINDOW *Radframe, *Repframe, *Disframe;
  int drow, dcol;

  Radwin = subwin(stdscr, 1, 6, 0, COLS - 29);
  Repwin = subwin(stdscr, Vessels+1, 44, 2, COLS - 45);
  Cmdwin = subwin(stdscr, Cmdrow, 0, Disrow, 0);
  scrollok(Cmdwin, true);
  Radframe = subwin(stdscr, 1, 25, 0, COLS - 46);
  Repframe = subwin(stdscr, Vessels+3, 46, 1, COLS - 46);
  Disframe = subwin(stdscr, Disrow, Discol, 0, 0);
  drow = Disrow - 2;
  dcol = Discol - 2;
  DisXmid = dcol / 2;
  DisYmid = drow / 2;
  Diswin = subwin(stdscr, drow, dcol, 1, 1);
  if (LINES - Vessels > 18)
  {
    Scnwin = newwin(14, 44, Vessels+4, COLS - 45);
    Prbwin = newwin(12, 44, Vessels+4, COLS - 45);
    HelpX = 22;
  } else {
    Scnwin = newwin(drow, dcol, 1, 1);
    Prbwin = newwin(12, 44, 3, COLS - 45);
    HelpX = DisXmid;
  }
  clear();

  attron(A_ALTCHARSET);
  wattron(Radframe, A_ALTCHARSET);
  wattron(Repframe, A_ALTCHARSET);
  wattron(Disframe, A_ALTCHARSET);
  DrawBox(Repframe, Vessels+3, 46);
  DrawBox(Disframe, Disrow, Discol);
  mvaddch(0,0,ACS_ULCORNER);
  mvaddch(0,COLS-46,ACS_URCORNER);
  mvaddch(1,COLS-46,ACS_LTEE);
  mvaddch(1,COLS-1,ACS_URCORNER);
  mvaddch(Vessels+3,COLS-46,ACS_LTEE);
  mvaddch(Vessels+3,COLS-1,ACS_LRCORNER);
  mvaddch(drow+1,0,ACS_LLCORNER);
  mvaddch(drow+1,dcol+1,ACS_LRCORNER);
  attroff(A_ALTCHARSET);
  wattroff(Radframe, A_ALTCHARSET);
  wattroff(Repframe, A_ALTCHARSET);
  wattroff(Disframe, A_ALTCHARSET);
  mvwaddch(Diswin,DisYmid,DisXmid,'+');
  mvwaddstr(Radframe,0,1,"Display Radius:");
  delwin(Radframe);
  delwin(Repframe);
  delwin(Disframe);
  DrawRep();
  wnoutrefresh(stdscr);
}


void Endln()
{
  LKnt++;
  if (LKnt == Cmdrow) {
    wrefresh(Cmdwin);
    getch();
    LKnt = 0;
  }
  waddstr(Cmdwin,"\n");
  wclrtoeol(Cmdwin);
  wrefresh(Cmdwin);
}

void SEndln()
{
  waddstr(Cmdwin,scrstr);
  Endln();
}

int Sign(X)
double X;
{
  int Y;

  if (X < 0)
    Y = -1;
  else
    Y = 1;
  return Y;
}


double RMod(X)
double X;
{
  double TEMP;

  modf(X/360, &TEMP);
  X -= TEMP * 360;
  if (X >= 360)
    X -= 360;
  if (X < 0)
    X += 360;
  return X;
}


double Bearing(From, NTo)
int From, NTo;
{
  double X, Y, Temp;

  X = Objects[NTo]->XPos - Objects[From]->XPos;
  Y = Objects[From]->YPos - Objects[NTo]->YPos;
  if (X != 0)
    Temp = atan(Y / X) * 180 / M_PI;
  else
    Temp = 90.0;
  if (X >= 0 && Y < 0)
    Temp += 360;
  else if (X < 0)
    Temp += 180;
  return Temp;
}


double Range(From, NTo)
int From, NTo;
{
  double X, Y;

  if (Objects[NTo] != NULL) {
    X = Objects[NTo]->XPos - Objects[From]->XPos;
    Y = Objects[From]->YPos - Objects[NTo]->YPos;
    return sqrt(X * X + Y * Y);
  } else
    return 9999.0;
}


double Radians(X)
double X;
{
  return (X * M_PI / 180.0);
}


void Status()
{
  reg int I, J, K;
  double Angle, Dist, X, Y, Temp;
  static char Symbol[4]="#*:";
  reg Object *WITH;

  J = 1;
  for (I = 0; I <= Enemies; I++) {
    if (Ships[I] != NULL) {
      Temp = Objects[0]->Act_Crs;
      WITH = Objects[I];
      sprintf(scrstr,"%5.1f%6.0f", WITH->Act_Spd, WITH->Act_Crs);
      mvwaddstr(Repwin, J, 13, scrstr);
      J++;
      if (I) {
	Angle = Bearing(0, I);
	Dist = Range(0, I);
	Angle = RMod(Angle - Temp);
	sprintf(scrstr,
	   "%6ld%8ld", (long)floor(Angle + 0.5), (long)floor(Dist + 0.5));
	waddstr(Repwin, scrstr);
      }
    }
  }
  wmove(Repwin, 1, 26);
  wclrtoeol(Repwin);
  if (Ships[0] != NULL) {
    I = Objects[0]->Trgt;
    if (I) {
      if (I < 0) {
	waddstr(Repwin,"elud");
	I = (-I);
      } else
	waddstr(Repwin,"pursu");
      sprintf(scrstr,"ing %s", Snams[I-1]);
      waddstr(Repwin,scrstr);
    }
  }
  wnoutrefresh(Repwin);
  werase(Diswin);
  if (Ships[0] != NULL && !Ships[0]->Dmge[(int)Snsr])
  {
    for (I = 0; I < ObjCount; I++) {
      if (Objects[I] != NULL) {
        X = (Objects[I]->XPos - Objects[0]->XPos) * DisXmid / Radius + DisXmid;
        Y = (Objects[I]->YPos - Objects[0]->YPos) * DisYmid / Radius + DisYmid;
        J = (int)(X + 0.5);
        K = (int)(Y + 0.5);
        if (J >= 0 && J <= Discol-2 && K >= 0 && K <= Disrow-2) {
          wmove(Diswin, K, J);
          J = Objects[I]->ObTyp;
          if (J) {
            if (J < 10)
   	      waddch(Diswin,Snams[I][0]);
            else
	      waddch(Diswin,Symbol[J / 10 - 1]);
          }
        }
      }
    }
  }

  mvwaddch(Diswin,DisYmid,DisXmid,'+');
  wnoutrefresh(Diswin);
  wmove(Radwin,0,0);
  if (!Ships[0] || Ships[0]->Dmge[(long)Snsr])
    waddstr(Radwin,"-----");
  else {
    sprintf(scrstr,"%d", Radius);
    waddstr(Radwin,scrstr);
  }
  wnoutrefresh(Radwin);
  doupdate();
}


Void Distrib(V, X)
int V;
double X;
{
  double H, H1, Q;
  reg int I;
  reg Srec *WITH;

  H = Objects[V]->Fuel + Ships[V]->Regen * X;
  WITH = Ships[V];
  for (I = 0; I < Phasers; I++) {
    if (WITH->Pstat[I] >= 0 && WITH->PCrts[I] < 0) {
      Q = -WITH->PCrts[I] * X;
      if (Q > WITH->PChrg[I])
	Q = WITH->PChrg[I];
      if (Q < 0)
	Q = 0.0;
      WITH->PChrg[I] -= Q;
      H += Q;
    }
  }
  for (I = 0; I <= 3; I++) {
    if (WITH->Seff[I] > 0) {
      H1 = WITH->Sdrns[I] * X;
      if (H1 > H)
	H1 = H;
      H -= H1;
      WITH->Sdrns[I] = H1 / X;
    }
  }
  if (H > 0) {
    for (I = 0; I < Phasers; I++) {
      if (WITH->PChrg[I] < 10 && WITH->Pstat[I] >= 0 && WITH->PCrts[I] >= 0) {
	H1 = H;
	if (H1 > 10 - WITH->PChrg[I])
	  H1 = 10 - WITH->PChrg[I];
	if (H1 > WITH->PCrts[I] * X)
	  H1 = WITH->PCrts[I] * X;
	H -= H1;
	WITH->PChrg[I] += H1;
      }
    }
  }
  Objects[V]->Fuel = H;
  if (Objects[V]->Fuel > WITH->Fcap)
    Objects[V]->Fuel = WITH->Fcap;
  H -= Objects[V]->Fuel;
  if (H <= 0)
    return;
  for (I = 0; I < Phasers; I++) {
    if (H > 0 && WITH->Pstat[I] >= 0) {
      H1 = 10 - WITH->PChrg[I];
      if (H1 > H)
	H1 = H;
      WITH->PChrg[I] += H1;
      H -= H1;
    }
  }
}


void Damage(From, Nto, Hits, Sorc)
int From, Nto, Hits, Sorc;
{
  int fax[6];
  int odds[(int)Warp - (int)Comp + 1];
  reg int I, J, S;
  double H;
  Damages L;
  reg Srec *WITH;
  int FORLIM;

  H = Bearing(From, Nto);
  H = RMod(H - Objects[Nto]->Act_Crs + 225) / 90 + 1;
  S = (long)H;
  if (S == 5)
    S = 1;
  sprintf(scrstr," Hit %d on %s's shield %d", Hits, Snams[Nto], S);
  SEndln();
  if (Sorc == 0) {
    fax[0] = 50;
    fax[1] = 2;
    fax[2] = 20;
    fax[3] = 3;
    fax[4] = 10;
    fax[5] = 3;
    odds[0] = 1000;
    odds[(int)Snsr - (int)Comp] = 500;
    odds[(int)Prob - (int)Comp] = 100;
    odds[(int)Warp - (int)Comp] = 50;
  } else {
    fax[0] = 100;
    fax[1] = 3;
    fax[2] = 10;
    fax[3] = 2;
    fax[4] = 7;
    fax[5] = 6;
    odds[0] = 1500;
    odds[(int)Snsr - (int)Comp] = 750;
    odds[(int)Prob - (int)Comp] = 150;
    odds[(int)Warp - (int)Comp] = 75;
  }
  WITH = Ships[Nto];
  S--;
  H = Hits * (1 - WITH->Sdrns[S] * WITH->Seff[S] / 100);
  WITH->Weff += H / fax[0];
  Objects[Nto]->Fuel -= H / fax[1];
  if (Objects[Nto]->Fuel < 0)
    Objects[Nto]->Fuel = 0.0;
  WITH->Regen -= H / fax[2];
  if (WITH->Regen < 0)
    WITH->Regen = 0.0;
  if (!S)
    fax[3] = (long)floor(fax[3] * 1.5 + 0.5);
  WITH->Seff[S] -= (double)Hits / fax[3];
  if (WITH->Seff[S] < 0)
    WITH->Seff[S] = 0.0;
  WITH->Crew -= _randint((long)fax[4]);
  if (WITH->Crew < 0)
    WITH->Crew = 0;
  FORLIM = (long)floor(H / fax[5] + 0.5);
  for (I = 0; I < FORLIM; I++) {
    J = _randint((long)(Phasers + Tubes));
    if (J >= Phasers) {
      J -= Phasers;
      if (WITH->Tstat[J] >= 0) {
	WITH->Tstat[J] = (-1);
	WITH->Tengy[J] = 0;
	if (!Nto) {
	  sprintf(scrstr," Tube %d damaged.", J+1);
	  SEndln();
	}
      }
    } else {
      if (WITH->Pstat[J] >= 0) {
	WITH->Pstat[J] = (-1);
	WITH->PChrg[J] = 0.0;
	WITH->PCrts[J] = 0.0;
	if (!Nto) {
	  sprintf(scrstr," Phaser %d damaged.", J+1);
	  SEndln();
	}
      }
    }
  }
  for (L = Comp; (long)L <= (long)Warp; L = (Damages)((long)L + 1)) {
    if (H >= _randint((long)odds[(long)L - (long)Comp])) {
      if (!WITH->Dmge[(long)L]) {
	WITH->Dmge[(long)L] = true;
	if (!Nto) {
	  sprintf(scrstr,"  %s", DText[(long)L - (long)Comp]);
	  SEndln();
	}
      }
    }
  }
  if (H > 42)
    Objects[Nto]->Tim_Del = 10000.0;
  Distrib(Nto, Iter);
}


void TrakPhas(I, J)
int I, J;
{
  int K;
  double D;

  K = Ships[I]->Pstat[J];
  if (K >= 100)
    K -= 100;
  if (K) {
    if (Ships[K-1] == NULL) {
      Ships[I]->Pstat[J] = 0;
      K = 0;
      if (!I) {
	sprintf(scrstr," Phaser %d disengaging.", J+1);
	SEndln();
      }
    } else {
      D = Bearing(I, K-1) - Objects[I]->Act_Crs;
      D = RMod(D);
      Ships[I]->Pdepl[J] = (long)floor(D + 0.5);
    }
  }
  D = Ships[I]->Pdepl[J];
  /* If phaser was set to fire, but engineering is in the way, don't fire. */
  if (D > 125 && D < 235 && !Ships[I]->Dmge[(int)Engn])
    Ships[I]->Pstat[J] = K;
}


int PhHits(From, NTo, Phnum, Units)
int From, NTo, Phnum, *Units;
{
  double F3, G2, G3;
  int Hits;

  Hits = 0;
  *Units = (long)floor(Ships[From]->Pstat[5] / 100.0 *
		       Ships[From]->PChrg[Phnum] + 0.5);
  G2 = Range(From, NTo);
  if (G2 > 1000)
    return Hits;
  F3 = Ships[From]->Pstat[4];
  G3 = Bearing(From, NTo);
  G3 = RMod(Objects[From]->Act_Crs + Ships[From]->Pdepl[Phnum] - G3);
  if (G3 <= F3 || 360 - G3 <= F3)
    Hits = (long)(*Units * sqrt(1 - G2 / 1000) * 45 / F3);
  return Hits;
}


void PhaserFire(I)
int I;
{
  int J, K, L, Hits, Units;

  K = (-1);
  for (J = 0; J < Phasers; J++) {
    if (Ships[I]->Pstat[J] > 99) {
      K = J;
      break;
    }
  }
  if (K < 0)
    return;
  TrakPhas(I, K);
  if (Ships[I]->Pstat[K] <= 99)
    return;
  sprintf(scrstr," <<%s Firing Phasers>>", Snams[I]);
  SEndln();
  Ships[I]->Pstat[K] -= 100;
  for (J = 0; J < ObjCount; J++) {
    if (Objects[J] != NULL) {
      L = Objects[J]->ObTyp;
      if (I != J && L >= 0) {
	Hits = PhHits(I, J, K, &Units);
	if (Hits > 0) {
	  if (L > 10) {
	    Objects[J]->Fuel -= Hits / 2.0;
	    if (Objects[J]->Fuel < 0)
	      Objects[J]->Fuel = 0.0;
	    Objects[J]->Tim_Del = 10000.0;
	  }
	  L/=10;
	  switch (L) {

	  case 0:
	    Damage(I, J, Hits, 0);
	    break;

	  case 1:
	    sprintf(scrstr,"%s's Engineering hit.",
		   Snams[(Objects[J]->ObTyp) % 10]);
	    break;

	  case 2:
	    sprintf(scrstr,"Hit on probe %d", J);
	    break;

	  case 3:
	    sprintf(scrstr,"Hit on torpedo %d", J);
	    break;
	  }
	  if (L > 0)
	    SEndln();
	}
      }
    }
  }
  Ships[I]->PChrg[K] -= Units;
}


void Launch(I, J)
int I, *J;
{
  reg int K;
  reg Object *WITH;

  *J = 0;
  for (K = 1; K < AryMax; K++) {
    if (Objects[K] == NULL) {
      *J = K;
      break;
    }
  }
  if (*J == 0) {
    printf("Sorry, game has run out of array space (Too many Torps, etc)\n");
    exit(1);
  }
  Objects[K] = (Object *)Malloc(sizeof(Object));
  WITH = Objects[K];
  WITH->ObTyp = (-1);
  WITH->XPos = Objects[I]->XPos;
  WITH->YPos = Objects[I]->YPos;
  WITH->Act_Spd = 0.0;
  WITH->Des_Spd = 0.0;
  WITH->Act_Crs = 0.0;
  WITH->Des_Crs = 0.0;
  WITH->Fuel = 0.0;
  WITH->Trgt = 0;
  WITH->Tim_Del = 0.0;
  WITH->Prx_Del = 0.0;
  K = 0;
  for (I = AryMax-1; I > 0; I--) {
    if (Objects[I]) {
      K = I+1;
      break;
    }
  }
  ObjCount = K;
}


void TrakTorp(I, J)
int I, J;
{
  int K;
  double D;

  K = Ships[I]->Tstat[J];
  if (K >= 100)
    K -= 100;
  if (K) {
    if (Ships[K-1] == NULL) {
      Ships[I]->Tstat[J] = 0;
      K = 0;
      if (!I) {
	sprintf(scrstr," Tube %d disengaging.", J+1);
	SEndln();
      }
    } else {
      D = Bearing(I, K-1) - Objects[I]->Act_Crs;
      D = RMod(D);
      Ships[I]->TDepl[J] = D;
    }
  }
  D = Ships[I]->TDepl[J];
  if (D > 135 && D < 225 && !Ships[I]->Dmge[(long)Engn])
    Ships[I]->Tstat[J] = K;
}


void Torpfire(I)
int I;
{
  int J, K, Num;
  double X;
  reg Object *WITH;

  K = (-1);
  for (J = 0; J < Tubes; J++) {
    if (Ships[I]->Tstat[J] > 99) {
      K = J;
      break;
    }
  }
  if (K < 0)
    return;
  TrakTorp(I, K);
  if (Ships[I]->Tstat[K] <= 99)
    return;
  Launch(I, &Num);
  sprintf(scrstr,"<<%s firing Torpedo %d>>", Snams[I], Num);
  SEndln();
  WITH = Objects[Num];
  WITH->Des_Spd = Ships[I]->Tstat[6];
  WITH->Act_Spd = WITH->Des_Spd;
  WITH->Des_Crs = RMod(Ships[I]->TDepl[K] + Objects[I]->Act_Crs);
  WITH->Act_Crs = WITH->Des_Crs;
  WITH->Fuel = Ships[I]->Tengy[K];
  WITH->ObTyp = I + 30;
  WITH->Tim_Del = 10000 - Ships[I]->Tstat[7] * 4.0;
  WITH->Prx_Del = Ships[I]->Tstat[8];
  X = Radians(WITH->Act_Crs);
  WITH->XPos += WITH->Act_Spd * cos(X) * Iter * 100;
  WITH->YPos -= WITH->Act_Spd * sin(X) * Iter * 100;
  Ships[I]->Tstat[K] -= 100;
  Ships[I]->Tengy[K] = 0;
}


int AMHits(I, J)
int I, J;
{
  int Hits;
  double F1, F2, G2;

  Hits = 0;
  F1 = Objects[I]->Fuel * 5;
  G2 = Range(I, J);
  F2 = F1 * 10;
  if (G2 <= F2)
    Hits = (long)(F1 * sqrt(1 - G2 / F2));
  return Hits;
}


void Detonate(I)
int I;
{
  int Hits, I0, I1, J, K, M;

  I0 = Objects[I]->ObTyp;
  M = I0 % 10;
  I0 /= 10;
  I1 = I0;
  if (I0 == 0) {
    sprintf(scrstr,"++%s++ Destruction", Snams[I]);
    for (K = 0; K < Phasers; K++)
      Objects[I]->Fuel += Ships[I]->PChrg[K];
    for (K = 0; K < Tubes; K++)
      Objects[I]->Fuel += Ships[I]->Tengy[K];
    Vessels--;
  } else if (I0 == 1)
    sprintf(scrstr,"##%s's Engineering##", Snams[M]);
  else if (I0 == 2)
    sprintf(scrstr,"** %d **", I);
  else
    sprintf(scrstr,":: %d ::", I);
  SEndln();
  for (J = 0; J < ObjCount; J++) {
    if (Objects[J] != NULL) {
      if (Objects[J]->Trgt == I+1)
	Objects[J]->Trgt = 0;
    }
    if (J != I && Objects[J] != NULL) {
      Hits = AMHits(I, J);
      I0 = Objects[J]->ObTyp;
      M = I0 % 10;
      I0 /= 10;
      if (Hits > 0) {
	if (I0 == 0)
	  Damage(I, J, Hits, 1);
	else if (I0 == 1)
	  sprintf(scrstr,"Hit on %s's jettisoned Engineering", Snams[M]);
	else if (I0 == 2)
	  sprintf(scrstr,"Hit on probe %d", J);
	if (I0 > 0) {
	  Objects[J]->Tim_Del = 9999.0;
	  if (I0 < 3)
	    SEndln();
	}
      }
    }
  }
  free(Objects[I]);
  Objects[I] = NULL;
  if (I1 != 0)
    return;
  free(Ships[I]);
  Ships[I] = NULL;
  Redo = true;
}


void Antimatter(I)
int I;
{
  int J, M, K0;
  boolean Boom;
  reg Object *WITH;

  WITH = Objects[I];
  M = WITH->ObTyp % 10;
  WITH->Tim_Del++;
  K0 = (long)WITH->Tim_Del;
  if (K0 > 10000) {
    Detonate(I);
    return;
  }
  Boom = false;
  if (WITH->Prx_Del != 0) {
    for (J = 0; J < ObjCount; J++) {
      if (Objects[J] != NULL && J != I) {
	if (Objects[J]->ObTyp < 20 && WITH->Prx_Del >= Range(I, J) && J != M)
	  Boom = true;
      }
    }
  }
  if (Boom)
    Detonate(I);
}


static void Nowarp(i)
int i;
{
  double Temp;
  reg Object *WITH;

  WITH = Objects[i];
  Temp = fabs(WITH->Des_Spd);
  if (Temp > 0.99)
    Temp = 0.99;
  WITH->Des_Spd = Sign(WITH->Des_Spd) * Temp;
}


void Movem(i)
int i;
{
  uchar Otp;
  double Accel, Crs1, Rate, Turn, Temp;
  reg Object *WITH;

  WITH = Objects[i];
  Otp = WITH->ObTyp / 10;
  if (!Otp) {
    if (Ships[i]->Crew == 0)
      WITH->Des_Spd = 0.0;
    if (Ships[i]->Dmge[(long)Comp])
      WITH->Trgt = 0;
    if (Ships[i]->Dmge[(long)Warp])
      Nowarp(i);
    if (fabs(WITH->Des_Spd) >= 1) {
      WITH->Fuel -= fabs(WITH->Des_Spd) * Ships[i]->Weff * Iter;
      if (WITH->Fuel <= 0) {
	WITH->Fuel = 0.0;
	WITH->Des_Spd = 0.0;
	Endln();
	sprintf(scrstr,"%s's warp drive shutting down.", Snams[i]);
	SEndln();
	Nowarp(i);
      }
    }
  }
  WITH = Objects[i];
  if (!Otp) {
    if (WITH->Trgt != 0) {
      WITH->Des_Crs = Bearing(i, abs(WITH->Trgt)-1);
      if (WITH->Trgt < 0)
	WITH->Des_Crs = RMod(WITH->Des_Crs + 180);
    }
    if (WITH->Des_Crs != WITH->Act_Crs) {
      Crs1 = RMod(WITH->Des_Crs - WITH->Act_Crs);
      if (Crs1 >= 180)
	Crs1 -= 360;
      Rate = 10.0;
      if (i > 0)
	Rate = 12.0;
      Turn = (Rate - fabs(WITH->Act_Spd)) * 5 * Iter;
      if (Ships[i]->Dmge[(long)Warp])
	Turn /= 2;
      Temp = fabs(Crs1);
      if (Temp > Turn)
	Temp = Turn;
      Temp = Sign(Crs1) * Temp;
      WITH->Act_Crs = RMod(WITH->Act_Crs + Temp);
    }
  }
  if (Otp < 2) {
    Accel = WITH->Des_Spd - WITH->Act_Spd;
    WITH->Act_Spd += Sign(Accel) * sqrt(fabs(Accel)) * Iter;
  } else if (Otp == 2) {
    if (WITH->Trgt > 0)
      WITH->Act_Crs = Bearing(i, WITH->Trgt-1);
  }
  WITH->Act_Crs = RMod(WITH->Act_Crs);
  Crs1 = Radians(WITH->Act_Crs);
  WITH->XPos += WITH->Act_Spd * cos(Crs1) * Iter * 100;
  WITH->YPos -= WITH->Act_Spd * sin(Crs1) * Iter * 100;
}


int Win2=0;

void Ender()
{
  int I;
  boolean Debris;
  Char *X1, *X2;
  char *plur;

  Debris = false;
  if (Win < 6) {		/* Check for flying antimatter */
    for (I = 1; I < ObjCount; I++) {
      if (Objects[I] && (Objects[I]->ObTyp >= 10)) {
        Debris = true;
        break;
      }
    }
  }
  Endln();
  if (Debris) {
    if (Win == Win2)
      return;

    Win2 = Win;
    switch (Win) {

    case 1:
      sprintf(scrstr,"%s:  Message to the Federation:  This is commander %s of",
	Capt, Capt);
      SEndln();
      sprintf(scrstr,"   the %s empire.  We have defeated the %s and are",
	Enemy, Snams[0]);
      SEndln();
      waddstr(Cmdwin,"   departing the quadrant.");
      break;

    case 2:
      sprintf(scrstr,"Spock:  All %s vessels have been either crippled or",
	Enemy);
      SEndln();
      waddstr(Cmdwin,"   destroyed.  We still, however, have antimatter devices to");
      Endln();
      waddstr(Cmdwin,"   avoid.");
      break;

    case 3:
      sprintf(scrstr,"Sulu:  The %ss are falling behind, and seem to be",
	Enemy);
      SEndln();
      waddstr(Cmdwin,"   breaking off their attack.");
      break;

    case 4:
      waddstr(Cmdwin,"Uhura:  I'm informing Starfleet Command of our disposition.");
      break;

    case 5:
      sprintf(scrstr,"Spock:  Although the %ss have surrendered, there are",
	Enemy);
      SEndln();
      waddstr(Cmdwin,"   still anti-matter devices floating around us.");
      break;
    }
    Endln();
    return;
  }
  X1 = "he";
  X2 = "his";
  if (Title == Female) {
    X1 = "she";
    X2 = "her";
  } else if (Title != Male) {
    X1 = "it";
    X2 = "its";
  }
  waddstr(Cmdwin,"Starfleet Command:   ");
  switch (Win) {

  case 1:
    waddstr(Cmdwin,"We have received confirmation that the");
    Endln();
    sprintf(scrstr,"   U.S.S. %s, Captained by %s, was destroyed",
      Snams[0], Name);
    SEndln();
    sprintf(scrstr,"   by %s battle cruisers.  May future Federation",
      Enemy);
    SEndln();
    waddstr(Cmdwin,"   officers perform better in their duties.");
    break;

  case 2:
    sprintf(scrstr,"We commend Captain %s and the crew of the", Name);
    SEndln();
    sprintf(scrstr,"   %s on their fine performance against the %ss.",
      Snams[0], Enemy);
    SEndln();
    sprintf(scrstr,"   May %s be an inspiration to future starship captains.",
      X1);
    waddstr(Cmdwin,scrstr);
    break;

  case 3:
    sprintf(scrstr,"Captain %s of the U.S.S. %s has", Name, Snams[0]);
    SEndln();
    sprintf(scrstr,"   out-maneuvered %s foes. We commend %s",
      Enemy, X2);
    SEndln();
    waddstr(Cmdwin,"   tactical ability.");
    break;

  case 4:
    sprintf(scrstr,"Captain %s of the U.S.S. %s has", Name, Snams[0]);
    SEndln();
    sprintf(scrstr,"   surrendered %s vessel to the %ss.  May Captain",
      X2, Enemy);
    SEndln();
    waddstr(Cmdwin,"   Dunsell be remembered.");
    break;

  case 5:
    sprintf(scrstr,"We have received confirmation from the %s that",
      Snams[0]);
    SEndln();
    sprintf(scrstr,"   the %ss have surrendered.", Enemy);
    waddstr(Cmdwin,scrstr);
    break;

  case 6:
    if (Enemies != 1)
      plur = "s";
    else
      plur = "";
    waddstr(Cmdwin,"One of our vessels has reported the wreckage");
    Endln();
    sprintf(scrstr,"   of the %s and %d %s vessel%s.", Snams[0],
      Enemies, Enemy, plur);
    waddstr(Cmdwin,scrstr);
  }
  Endln();
  Endln();
  Debris = false;
  for (I = 0; I <= Enemies; I++) {
    if (Ships[I] != NULL) {
      if (!Debris) {
	Debris = true;
	waddstr(Cmdwin,"Survivors reported:");
	Endln();
      }
      sprintf(scrstr,"%s - %d", Snams[I], Ships[I]->Crew);
      SEndln();
    }
  }
  if (!Debris) {
    waddstr(Cmdwin,"*** No survivors reported ***");
    Endln();
  }
  Stop = true;
}


void Execute()
{
  int I, I0, J, N1, N2;
  boolean Ok;
  double Rn;
  int FORLIM;
  Srec *WITH;

  FORLIM = (long)(SimTim / Iter);
  for (I = 0; I < FORLIM; I++) {
    for (J = 0; J < ObjCount; J++) {
      if (Objects[J] != NULL) {
	I0 = Objects[J]->ObTyp;
	if (I0 < 10) {
	  if (Ships[J]->Crew > 0) {
	    if (I < Phasers)
	      PhaserFire(J);
	    if (I < Tubes)
	      Torpfire(J);
	  }
	}
	Antimatter(J);
	if (Objects[J] != NULL)
	  Movem(J);
      }
    }
  }
  for (I = 0; I <= Enemies; I++) {
    if (Ships[I] != NULL) {
      Distrib(I, 2.0);
      for (J = 0; J < Phasers; J++) {
	if (Ships[I]->Pstat[J] >= 0)
	  TrakPhas(I, J);
      }
      for (J = 0; J < Tubes; J++) {
	if (Ships[I]->Tstat[J] >= 0)
	  TrakTorp(I, J);
      }
    }
  }
  TCount = (-1);
  if (Ships[0] != NULL) {
    I0 = (long)floor((10000 - Objects[0]->Tim_Del) * Iter + 0.5);
    if (I0 != 0 && I0 <= 15) {
      sprintf(scrstr,"Computer: %d seconds to self-destruct.\n", I0);
      SEndln();
    }
  }
  /*
   * Fuses, bluffs, surrenders
   */
  for (I = 0; I <= 3; I++) {
    if (Ct[I] != 0)
      Ct[I]++;
  }
  N1 = 0;
  N2 = 0;
  Win = 0;
  /*
   * Federation disposition
   */
  if ((Ships[0] == NULL) || (Ships[0]->Crew == 0))
    Win = 1;
  else {
    WITH = Ships[0];
    Ok = false;
    if ((WITH->Regen > 1) ||
       ((!WITH->Dmge[(int)Warp] || !WITH->Dmge[(int)Prob]) &&
	Objects[0]->Fuel > 10)) {
      Ok = true;
    } else {
      for (I = 0; I < Phasers; I++) {
        if (WITH->PChrg[I] > 0) {
	  Ok = true; break;
        }
      }
      for (I = 0; (I < Tubes) && !Ok; I++) {
        if (WITH->Tengy[I] > 0) {
	  Ok = true; break;
        }
      }
    }
    if (!Ok)
      Win = 1;
  }
  /*
   * Enemy disposition
   */
  for (I = 1; I <= Enemies; I++) {
    if ((Ships[I] == NULL) || (Ships[I]->Crew == 0)) {
      N1++;
      continue;
    }
    Rn = Range(I, 0);
    if (((Rn > 3500) && Ships[I]->Dmge[(long)Warp]) ||
	((Rn > 20000) && (Objects[0]->Tim_Del > 100))) {
      N2++;
      continue;
    }
    if (Objects[I]->Fuel > 10)
      continue;
    Ok = false;
    for (J = 0; J < Phasers; J++) {
      if (Ships[I]->PChrg[J] > 0) {
	    Ok = true;
	    break;
      }
    }
    if (Ok)
      continue;
    for (J = 0; J < Tubes; J++) {
      if (Ships[I]->Tengy[J] > 0) {
	    Ok = true;
	    break;
      }
    }
    if (!Ok)
      N1++;
  }

  /* Don't quit on all disabled... */
  if (cheat)
    N2 = 0;

  switch (Win) {

  case 0:
    if (N1 == Enemies)
      Win = 2;
    else if (N1 + N2 == Enemies)
      Win = 3;
    break;

  case 1:
    if (N1 == Enemies)
      Win = 6;		/* Nobody won */
/*    else if (N1 + N2 < Enemies)
      Win = 5;		/* No special case needed */
    break;
  }
  if (Win > 0)
    Ender();
}


void Detach(I)
int I;
{
  int J;
  boolean Drop;

  Launch(I, &J);
  Drop = false;
  Objects[J]->Tim_Del = 10000 - 10 / Iter;
  Objects[J]->Fuel = Objects[I]->Fuel;
  Objects[I]->Fuel = 0.0;
  Objects[J]->ObTyp = I + 10;
  Ships[I]->Regen = 0.0;
  Ships[I]->Fcap = 0;
  Ships[I]->Weff = 10.0;
  Ships[I]->Dmge[(long)Engn] = true;
  Ships[I]->Dmge[(long)Prob] = true;
  for (J = 0; J < Phasers; J++) {
    if (Ships[I]->PChrg[J] > 0)
      Drop = true;
  }
  if (Drop) {
    for (J = 0; J <= 3; J++)
      Ships[I]->Seff[J] = 0.0;
  }
}


/* Local variables for EStrat: */
struct LOC_EStrat {
  int Sh, Targ;
  double Bear, Rnge;
} ;

Local Void Desist(LINK)
reg struct LOC_EStrat *LINK;
{
  Objects[LINK->Sh]->Des_Spd = 1.0;
  Objects[LINK->Sh]->Des_Crs = 0.0;
}

Local Void Retreat(LINK)
reg struct LOC_EStrat *LINK;
{
  Object *WITH;

  WITH = Objects[LINK->Sh];
  if (WITH->Des_Spd < 0 || WITH->Fuel <= 0)
    return;
  sprintf(scrstr,"Sulu: The %s is retreating.", Snams[LINK->Sh]);
  WITH->Des_Spd = -0.99;
  WITH->Trgt = LINK->Targ;
  SEndln();
}

Local Void Evade(LINK)
reg struct LOC_EStrat *LINK;
{
  sprintf(scrstr,"%s taking evasive action.", Snams[LINK->Sh]);
  SEndln();
  Objects[LINK->Sh]->Des_Spd = _randint(8L) + 3.0;
  Objects[LINK->Sh]->Des_Crs = _randint(360L);
  if (Ships[LINK->Sh]->Dmge[(long)Warp]) {
    Objects[LINK->Sh]->Trgt = LINK->Targ;
    Objects[LINK->Sh]->Des_Spd = 0.99;
  } else
    Objects[LINK->Sh]->Trgt = 0;
}

Local Void Ruse(LINK)
reg struct LOC_EStrat *LINK;
{
  int i;
  switch(Ct[0]) {

  case 1:
    if (_randint(10) > 2) {
      waddstr(Cmdwin,"Sulu:  No apparent change in the enemy's actions.");
      Endln();
      Ct[0]=6;
      return;
    }
    Ct[0]=2;

  case 2:
    Objects[LINK->Sh]->Des_Spd = 1.0;
    if (_randint(2)) {
      Objects[LINK->Sh]->Trgt = 1;
    } else {
      Objects[LINK->Sh]->Trgt = (-1);
    }
    return;

  case 3:
    sprintf(scrstr,"Sulu:  The %s is ",Snams[LINK->Sh]);
    waddstr(Cmdwin,scrstr);
    if (Objects[LINK->Sh]->Trgt > 0)
      waddstr(Cmdwin,"cautiously advancing.");
    else
      waddstr(Cmdwin,"turning away.");
    Endln();

  case 4:
  case 5:
    if ((Objects[0]->Act_Spd > 1) || (LINK->Rnge < 200)) {
      Ct[0]=6;
      return;
    }
    for (i=0; i<4; i++)
      if (Ships[0]->Sdrns[i]) {
	Ct[0]=6;
	return;
      }
  }
}

Local Void Bluff(LINK)
reg struct LOC_EStrat *LINK;
{
  int i;
  switch(Ct[1]) {

  case 1:
    i=4;
    if (Enemy == Romulan)
      i = 6;
    if (i>_randint(10)) {
      sprintf(scrstr,"Spock:  %ss giving ground, Captain.  Obviously, they", Enemy);
      SEndln();
      waddstr(Cmdwin,"   tapped in as you expected them to.");
      Endln();
      sprintf(scrstr,"%s:  A logical assumption, Mr. Spock.  Are they still", Name);
      SEndln();
      waddstr(Cmdwin,"   retreating?");
      Endln();
      sprintf(scrstr,"Spock:  Yes, %s.", Title);
      SEndln();
      sprintf(scrstr,"%s:  Good.  All hands, stand by.", Name);
      SEndln();
    } else {
      sprintf(scrstr,"Uhura:  Message coming in from the %ss.", Enemy);
      SEndln();
      sprintf(scrstr,"%s:  Put it on audio.", Name);
      SEndln();
      if (_randint(2))
	sprintf(scrstr,"%s:  Ha, ha, ha, %s.  You lose.", Capt, Name);
      else
	sprintf(scrstr,"%s:  I fell for that last time we met, idiot!", Capt);
      SEndln();
      Ct[1] = 6;
      return;
    }
    Ct[1] = 2;

  case 2:
    Objects[LINK->Sh]->Trgt = (-1);
    Objects[LINK->Sh]->Des_Spd = _randint(7)+4;
    return;

  case 3:
  case 4:
  case 5:
    if (Objects[0]->Act_Spd > 2)
      Ct[1]=6;
    break;
  }
}

Local Void Surr(LINK)
reg struct LOC_EStrat *LINK;
{
  int i;
  boolean Ok;

  switch(Ct[2]) {

  case 1:
    i=50;
    if (Enemy == Romulan) {
      i=5;
      waddstr(Cmdwin,"Chekov:  The Romulans do not take prisoners.");
      Endln();
    }
    if (i>_randint(100)) {
      sprintf(scrstr,"Uhura:  Message coming in from the %s.", Snams[LINK->Sh]);
      SEndln();
      sprintf(scrstr,"%s:  On behalf of the %s empire, I accept your surrender.",
	Capt, Enemy);
      SEndln();
      waddstr(Cmdwin,"   You have five seconds to drop your shields, cut warp,");
      Endln();
      waddstr(Cmdwin,"   and prepare to be boarded.");
      Endln();
      Ct[2] = 2;
    } else {
      if (_randint(2))
	sprintf(scrstr,"%s:  Prepare to die, chicken %s!", Capt, Name);
      else
	sprintf(scrstr,"Uhura:  No reply from the %ss.", Enemy);
      SEndln();
      Ct[2] = 6;
      return;
    }

  case 2:
    Objects[LINK->Sh]->Trgt = 1;
    return;

  case 3:
  case 4:
  case 5:
    Ok = true;
    for (i=0; i<4; i++)
      if (Ships[0]->Sdrns[i]) {
	Ok = false;
	break;
      }
    if (Ok) {
      if (LINK->Rnge <= 400)
        Objects[LINK->Sh]->Des_Spd = 1;
      if ((LINK->Rnge <= 200) && (Objects[0]->Act_Spd <=1))
	Win = 4;
    }
    break;
  }
}

Local Void ESurr(LINK)
reg struct LOC_EStrat *LINK;
{
  int i;
  boolean Ok;

  Ok = true;
  if (Ct[3] == 1) {
    for (i=1; i<= Enemies; i++)
      if (Ships[i] && Ships[i]->Crew>100) {
	Ok=false;
	break;
      }
    if (Ok) {
      i=50;
      if (Enemy == Romulan)
	i=5;
      if (_randint(100)<i) {
	sprintf(scrstr,"%s:  As much as I hate to, Captain %s, we will surrender.",
	  Capt, Name);
	SEndln();
	waddstr(Cmdwin,"   We are dropping shields.  You may board us.");
	Endln();
	Ct[3] = 2;
      } else
	Ok = false;
    }
  }
  if (Ct[3] == 2) {
    for (i=0; i<4; i++)
      Ships[LINK->Sh]->Sdrns[i] = 0;
    Objects[LINK->Sh]->Des_Spd = 0;
    Win = 5;
    return;
  }
  if (!Ok) {
    sprintf(scrstr,"%s:  You must be joking, Captain %s.  Why don't you",
      Capt, Name);
    SEndln();
    waddstr(Cmdwin,"   surrender?");
    Endln();
    Ct[3] = 6;
  }
}

Local Void Prlnch(LINK)
reg struct LOC_EStrat *LINK;
{
  int I;
  double X;
  reg Object *WITH;

  Launch(LINK->Sh, &I);
  sprintf(scrstr,"%s launching probe %d", Snams[LINK->Sh], I);
  SEndln();
  X = Radians(LINK->Bear);
  WITH = Objects[I];
  WITH->Act_Spd = 2.0;
  WITH->Des_Spd = 2.0;
  WITH->Fuel = Objects[LINK->Sh]->Fuel;
  if (WITH->Fuel > 20)
    WITH->Fuel = 20.0;
  Objects[LINK->Sh]->Fuel -= WITH->Fuel;
  Ships[LINK->Sh]->Fcap = (long)(Ships[LINK->Sh]->Fcap - WITH->Fuel);
  WITH->ObTyp = LINK->Sh + 20;
  WITH->Trgt = LINK->Targ;
  WITH->Tim_Del = 10000 - 15 / Iter;
  WITH->Prx_Del = _randint(300L) + 201.0;
  WITH->XPos += WITH->Act_Spd * 100 * cos(X) * Iter;
  WITH->YPos -= WITH->Act_Spd * 100 * sin(X) * Iter;
}

Local Void Jettison(LINK)
reg struct LOC_EStrat *LINK;
{
  sprintf(scrstr,"Spock:  Sensors indicate debris being left by the %s",
	 Snams[LINK->Sh]);
  SEndln();
  waddstr(Cmdwin,"   Insufficient mass?");
  Endln();
  Detach(LINK->Sh);
}

Local Void Destruct(LINK)
reg struct LOC_EStrat *LINK;
{
  sprintf(scrstr,"Spock:  The %s is overloading what remains of its anti-",
	 Snams[LINK->Sh]);
  SEndln();
  waddstr(Cmdwin,
	"   matter pods -- obviously a suicidal gesture.  Detonation in");
  Endln();
  waddstr(Cmdwin,"   five seconds.");
  Endln();
  Objects[LINK->Sh]->Tim_Del = 10000 - 5 / Iter;
}

Local Void Scan(LINK)
reg struct LOC_EStrat *LINK;
{
  if (!Ships[LINK->Sh]->Dmge[(long)Snsr]) {
    sprintf(scrstr,"Spock:  We're being scanned by the %s",
	Snams[LINK->Sh]);
    SEndln();
  }
}

Local Void Dis2(LINK)
reg struct LOC_EStrat *LINK;
{
  int I, J;
  double Q;

  J = 0;
  for (I = 0; I < Tubes; I++) {
    if (Ships[LINK->Sh]->Tstat[I] >= 0 &&
	Ships[LINK->Sh]->Tengy[I] < 10 && Objects[LINK->Sh]->Fuel >= 30) {
      Q = Objects[LINK->Sh]->Fuel;
      if (Q > 10 - Ships[LINK->Sh]->Tengy[I])
	Q = 10.0 - Ships[LINK->Sh]->Tengy[I];
      Objects[LINK->Sh]->Fuel -= Q;
      if (!cheat)
        Ships[LINK->Sh]->Fcap -= (long)Q;
      Ships[LINK->Sh]->Tengy[I] += (long)Q;
      J++;
    }
  }
  if (J != 0)
    return;
  for (I = 0; I < Tubes; I++) {
    if (Ships[LINK->Sh]->Tstat[I] >= 0 &&
	Ships[LINK->Sh]->Tstat[I] != LINK->Targ) {
      Ships[LINK->Sh]->Tstat[I] = LINK->Targ;
      J++;
    }
  }
  if (J != 0)
    return;
  for (I = 0; I < Phasers; I++) {
    if (Ships[LINK->Sh]->Pstat[I] >= 0 &&
	Ships[LINK->Sh]->Pstat[I] != LINK->Targ) {
      Ships[LINK->Sh]->Pstat[I] = LINK->Targ;
      J++;
    }
  }
  if (J == 0)
    Scan(LINK);
}

Local Void Distant(LINK)
reg struct LOC_EStrat *LINK;
{
  reg Object *WITH;

  if (Objects[LINK->Sh]->Fuel < 10 && Ships[LINK->Sh]->Regen < 4.5) {
    Retreat(LINK);
    return;
  }
  if (Ships[LINK->Sh]->Dmge[(long)Warp]) {
    Retreat(LINK);
    return;
  }
  if (Objects[LINK->Targ - 1]->Tim_Del > 10000 - 10 / Iter) {
    Evade(LINK);
    return;
  }
  Objects[LINK->Sh]->Trgt = LINK->Targ;
  if (LINK->Bear >= 30 || LINK->Bear <= 330) {
    if (Objects[LINK->Sh]->Fuel < 30 || Objects[LINK->Sh]->Des_Spd >
	  fabs(Objects[LINK->Targ - 1]->Act_Spd) + 1.5) {
      Dis2(LINK);
      return;
    }
    Objects[LINK->Sh]->Des_Spd =
      fabs(Objects[LINK->Targ - 1]->Act_Spd) + _randint(2L) + 3;
    if (Objects[LINK->Sh]->Des_Spd > 11)
      Objects[LINK->Sh]->Des_Spd = 11.0;
    sprintf(scrstr,"Sulu: %s attacking.", Snams[LINK->Sh]);
    SEndln();
    return;
  }
  WITH = Objects[LINK->Sh];
  if (LINK->Rnge > 7500)
    WITH->Des_Spd = 0.5;
  if (WITH->Des_Spd > 4)
    WITH->Des_Spd += -_randint(2L) - 1;
  if (WITH->Fuel < 30 && WITH->Des_Spd > 6)
    WITH->Des_Spd += -_randint(4L) - 1;
  Dis2(LINK);
}

Local Void Med2(LINK)
reg struct LOC_EStrat *LINK;
{
  int I, J;
  boolean Test;
  double X;

  Test = true;
  for (I = 1; I < Enemies; I++) {
    if (LINK->Sh != I && Objects[I] != NULL) {
      if (Objects[I]->ObTyp < 10) {
	X = Bearing(LINK->Sh, I);
	X = RMod(X - Objects[LINK->Sh]->Act_Crs);
	if (X < 20 || X > 340 || Range(LINK->Sh, I) < 300)
	  Test = false;
      }
    }
  }
  J = 0;
  if (Test) {
    for (I = 0; I < Tubes; I++) {
      if (Ships[LINK->Sh]->Tstat[I] > 0 &&
	  Ships[LINK->Sh]->Tengy[I] > 0 && J <= _randint(3L) + 1) {
	Ships[LINK->Sh]->Tstat[I] += 100;
	J++;
      }
    }
  }
  if (J > 0) {
    Ships[LINK->Sh]->Tstat[Tubes] = 12;
    Ships[LINK->Sh]->Tstat[Tubes + 1] = 10;
    Ships[LINK->Sh]->Tstat[Tubes + 2] = _randint(200L) + 101;
    return;
  }
  if (Ships[LINK->Sh]->Dmge[(long)Warp] &&
      fabs(Objects[LINK->Targ - 1]->Act_Spd) >= 1) {
    Retreat(LINK);
    return;
  }
  Test = true;
  for (I = 0; I < Phasers; I++) {
    if (Ships[LINK->Sh]->Pstat[I] >= 0) {
      Test = false;
      break;
    }
  }
  for (I = 0; I < Tubes; I++) {
    if (Ships[LINK->Sh]->Tstat[I] >= 0) {
      Test = false;
      break;
    }
  }
  if (Test) {
    Retreat(LINK);
    return;
  }
  if (Objects[LINK->Sh]->Fuel <= 10 && Ships[LINK->Sh]->Regen <= 4 &&
      Objects[LINK->Sh]->Trgt * Objects[LINK->Sh]->Act_Spd > 0 &&
      Ships[LINK->Sh]->Dmge[(long)Prob] &&
      !Ships[LINK->Sh]->Dmge[(long)Engn]) {
    Jettison(LINK);
    return;
  }
  if (LINK->Bear >= 90 && LINK->Bear <= 270 &&
      Objects[LINK->Sh]->Trgt * Objects[LINK->Sh]->Act_Spd > 0 &&
      Objects[LINK->Sh]->Fuel > 10 && !Ships[LINK->Sh]->Dmge[(long)Prob]) {
    Prlnch(LINK);
    return;
  }
  if (LINK->Bear > 135 && LINK->Bear < 225) {
    Scan(LINK);
    return;
  }
  if (Objects[LINK->Targ-1]->Tim_Del > 10000 - 10 / Iter) {
    Evade(LINK);
    return;
  }
  if (Objects[LINK->Sh]->Des_Spd > fabs(Objects[LINK->Targ-1]->Act_Spd) + 1 &&
      Objects[LINK->Sh]->Trgt == LINK->Targ) {
    Distant(LINK);
    return;
  }
  Objects[LINK->Sh]->Des_Spd =
    fabs(Objects[LINK->Targ - 1]->Act_Spd) + _randint(2L) + 1;
  if (Objects[LINK->Sh]->Des_Spd > 11)
    Objects[LINK->Sh]->Des_Spd = 11.0;
  if (Ships[LINK->Sh]->Dmge[(long)Warp])
    Objects[LINK->Sh]->Des_Spd = 0.99;
  if (rand() & 8)
    Objects[LINK->Sh]->Trgt = LINK->Targ;
  else {
    Objects[LINK->Sh]->Des_Crs = Objects[LINK->Targ - 1]->Des_Crs;
    if (Objects[LINK->Targ - 1]->Des_Spd < 0.0)
    {
      Objects[LINK->Sh]->Des_Crs += 180.0;
      if (Objects[LINK->Sh]->Des_Crs > 360.0)
	Objects[LINK->Sh]->Des_Crs -= 360.0;
    }
    Objects[LINK->Sh]->Trgt = 0;
  }
  Dis2(LINK);
}

Local Void Normal(LINK)
reg struct LOC_EStrat *LINK;
{
  int I, J, K, Units;
  reg Srec *WITH;

  I = 1;
  if (LINK->Rnge > 1200)
    I = 2;
  if (LINK->Rnge > 4200)
    I = 3;
  switch (I) {

  case 1:
    WITH = Ships[LINK->Sh];
    K = 0;
    for (J = 0; J < Phasers; J++) {
      if (K <= _randint(4L) + 1 || Enemies == 1) {
	if (WITH->Pstat[J] >= 0 &&
	    PhHits(LINK->Sh, LINK->Targ-1, J + 1, &Units) >= 10) {
	  WITH->Pstat[J] += 100;
	  K++;
	}
      }
    }
    if (K > 0) {
      WITH->Pstat[Phasers] = 10;
      WITH->Pstat[Phasers + 1] = 100;
    } else if (LINK->Bear > 135 && LINK->Bear < 225)
      Evade(LINK);
    else {
      K = 0;
      for (J = 0; J < Phasers; J++) {
	if (WITH->Pstat[J] >= 0 && WITH->Pstat[J] != LINK->Targ) {
	  WITH->Pstat[J] = LINK->Targ;
	  K = 1;
	}
      }
      if (K == 0) {
	if (LINK->Bear >= 90 && LINK->Bear <= 270 &&
	    !WITH->Dmge[(long)Prob] && Objects[LINK->Sh]->Fuel > 10 &&
	    Objects[LINK->Sh]->Act_Spd <= 1)
	  Prlnch(LINK);
	else if (Objects[LINK->Sh]->Fuel < 10 && WITH->Regen < 4 &&
		 Objects[LINK->Sh]->Tim_Del < 10000 - 5 / Iter)
	  Destruct(LINK);
	else if (Objects[LINK->Sh]->Trgt == LINK->Targ &&
		 Objects[LINK->Sh]->Act_Spd + fabs(
		   Objects[LINK->Targ - 1]->Act_Spd) < 3)
	  Med2(LINK);
	else {
	  Objects[LINK->Sh]->Des_Spd = Objects[LINK->Sh]->Act_Spd;
	  Objects[LINK->Sh]->Trgt = LINK->Targ;
	  if (WITH->Dmge[(long)Warp]) {
	    if (fabs(Objects[LINK->Sh]->Des_Spd) < 1)
	      Med2(LINK);
	    else
	      Objects[LINK->Sh]->Des_Spd = -0.99;
	  }
	}
      }
    }
    break;

  case 2:
    K = 0;
    for (J = 2; J < ObjCount; J++) {
      if (Objects[J] != NULL) {
	if (Objects[J]->ObTyp >= 20 && Objects[J]->ObTyp < 30 &&
	    Range(LINK->Sh, J) < 3600) {
	  K = 1;
	  break;
	}
      }
    }
    if (K == 1)
      Evade(LINK);
    else
      Med2(LINK);
    break;

  case 3:
    Distant(LINK);
    break;
  }
}


void EStrat()
{
  struct LOC_EStrat V;
  int I;
  boolean Nrm;

  V.Targ = 1;
  V.Sh = TCount;
  if (Ships[0] != NULL) {
    V.Rnge = Range(V.Sh, V.Targ-1);
    V.Bear = Bearing(V.Sh, V.Targ-1);
    V.Bear = RMod(V.Bear - Objects[V.Sh]->Act_Crs);
  }
  if (Win > 0 && Win < 6) {
    switch (Win) {

    case 2:
    case 3:
      Retreat(&V);
      break;

    case 4:
    case 5:
      Desist(&V);
      break;
    }
    return;
  }
  for (I = 0; I < 4; I++) {
    if (Ct[I] > 0 && Ct[I] < 6) {
      switch (I) {

      case 0:
	Ruse(&V);
	break;

      case 1:
	Bluff(&V);
	break;

      case 2:
	Surr(&V);
	break;

      case 3:
	if (Ct[2]<3)
	  ESurr(&V);	/* No contemplation if waiting for your surrender */
	break;
      }
    }
  }
  Nrm = true;
  for (I = 0; I<4; I++)
    if (Ct[I] > 0 && Ct[I] < 6) {
      Nrm = false;
      break;
    }
  if (!Nrm) {	/* Check for unsportsmanlike firing... */
    for (I = 0; I < Phasers; I++)
      if (Ships[0]->Pstat[I] > 99) {
	Nrm = true;
	break;
      }
    if (!Nrm)
      for (I = 0; I < Tubes; I++)
	if (Ships[0]->Tstat[I] > 99) {
	  Nrm = true;
	  break;
	}
    if (Nrm) {
      sprintf(scrstr,"%s:  How dare you fire upon us!  We are resuming our",
	Capt);
      SEndln();
      waddstr(Cmdwin,"   attack!");
      Endln();
      Win = 0;
      for (I = 0; I<4; I++)
	if (Ct[I])
	  Ct[I] = 6;
    } else {
      Nrm = true;
      for (I = 0; I <= 3; I++)
        if (Ct[I] > 0 && Ct[I] < 6) {
          Nrm = false;
	  break;
        }
      if (Win == 4 || Win == 5)
        Nrm = false;
    }
  }
  if (Nrm)
    Normal(&V);
}


boolean Yes()
{
  Char Ch;

  waddstr(Cmdwin,"? [Y/N] ");
  wrefresh(Cmdwin);
  Ch=wgetch(Cmdwin);
  waddstr(Cmdwin,"\n");
  wclrtoeol(Cmdwin);
  wrefresh(Cmdwin);
  if (toupper(Ch) == 'Y')
    return true;
  else
    return false;
}


void BParam()
{
  Endln();
  waddstr(Cmdwin,"Spock: Bad parameters.");
  Endln();
}


void Whatt()
{
  Endln();
  waddstr(Cmdwin,"Error!!!");
  Endln();
}


void ChWhat()
{
  Endln();
  waddstr(Cmdwin,"Chekov: What? ");
  Endln();
}


void NoEngy()
{
  Endln();
  waddstr(Cmdwin,"Scott: We've not enough power.");
  Endln();
}


void NoProb()
{
  Endln();
  waddstr(Cmdwin,"Scott: Probe launcher destroyed, Captain.");
  Endln();
}


void NoSnsr()
{
  Endln();
  waddstr(Cmdwin,"Spock: Our sensors are damaged.");
  Endln();
}


void NoComp()
{
  Endln();
  sprintf(scrstr,"Spock: Quite impossible, %s, our computer is dead.", Title);
  SEndln();
}


boolean Getwarp(ok)
boolean ok;
{
  double temp;
  short code;

  waddstr(Cmdwin,"\n   Warp factor [-9 to 9] ");
  mygetstr(Cmdwin,scrstr,true);
  code = (sscanf(scrstr, "%lg", &temp) == 0);
  if (code > 0) {
    ok = false;
    return ok;
  }
  if (fabs(temp) > 9) {
    ok = false;
    ChWhat();
  } else if ((fabs(temp) >= 1) && Ships[0]->Dmge[(int)Warp]) {
    ok=false;
    NoEngy();
  } else if (ok)
    Objects[0]->Des_Spd = temp;
  return ok;
}


void Indigits(X)
int *X;
{
  reg int I, J;

  for (I = 0; I <= 9; I++)
    X[I] = 0;
  J = (-1);
  mygetstr(Cmdwin,scrstr,true);
  for (I = 0; scrstr[I]; I++) {
    if (isdigit(scrstr[I])) {
      J++;
      X[J] = scrstr[I]-'0';
    }
  }
}


void Scan_(I)
int I;
{
  int J;
  Damages D;
  Char Ch;
  reg Srec *WITH;

  werase(Scnwin);
  sprintf(scrstr,"Damages to %s:\n", Snams[I]);
  mvwaddstr(Scnwin, 0, 0, scrstr);
  WITH = Ships[I];
  for (D = Comp; (long)D <= (long)Warp; D = (Damages)((long)D + 1)) {
    if (WITH->Dmge[(long)D]) {
      waddstr(Scnwin,DText[(long)D - (long)Comp]);
      waddch(Scnwin,'\n');
    }
  }
  sprintf(scrstr,"Survivors: %d\n", WITH->Crew);
  waddstr(Scnwin,scrstr);
  waddstr(Scnwin,"Phasers:");
  for (J = 0; J <= 3; J++) {
    if (WITH->Pstat[J] < 0)
      waddstr(Scnwin,"  --");
    else {
      sprintf(scrstr,"%4.0f", WITH->PChrg[J]);
      waddstr(Scnwin,scrstr);
    }
  }
  waddstr(Scnwin,"\n  Tubes:");
  for (J = 0; J <= 5; J++) {
    if (WITH->Tstat[J] < 0)
      waddstr(Scnwin,"  --");
    else {
      sprintf(scrstr,"%4d", WITH->Tengy[J]);
      waddstr(Scnwin,scrstr);
    }
  }
  waddstr(Scnwin,"\nShields:");
  for (J = 0; J <= 3; J++) {
    sprintf(scrstr,"%5.0f", WITH->Seff[J]);
    waddstr(Scnwin,scrstr);
  }
  sprintf(scrstr,"\n  Efficiency: %4.2f\n", WITH->Weff);
  waddstr(Scnwin,scrstr);
  sprintf(scrstr,"Regeneration: %0.0f\n", WITH->Regen);
  waddstr(Scnwin,scrstr);
  sprintf(scrstr,"Fuel Cur/Max: %0.0f/%d\n", Objects[I]->Fuel, WITH->Fcap);
  waddstr(Scnwin,scrstr);
  waddstr(Scnwin,"Press a key...");
  wrefresh(Scnwin);
  wgetch(Scnwin);
}


int GetShp(Ok, Shp)
boolean *Ok, *Shp;
{
  int I, J, X, Y;

  getyx(Cmdwin, Y, X);
  *Shp = true;
  mygetstr(Cmdwin,scrstr,false);
  J = 0;
  if (scrstr[0] != '\0') {
    if (islower(scrstr[0]))
      scrstr[0]=toupper(scrstr[0]);
    for (I = 1; I <= Enemies; I++) {
      if (scrstr[0] == Snams[I][0]) {
	mvwaddstr(Cmdwin, Y, X, Snams[I]);
	waddstr(Cmdwin,"\n");
	wclrtoeol(Cmdwin);
	wrefresh(Cmdwin);
	*Ok = true;
	J = I;
	if (Ships[J] == NULL)
	  *Ok = false;
	break;
      }
    }
  }
  if (J != 0)
    return J;
  I = (sscanf(scrstr, "%d", &J) == 0);
  if (I == 0)
    *Shp = false;
  return J;
}


void PRStt()
{
  int I, J, X, Y;
  double Z;
  char *Junk;

  werase(Prbwin);
  mvwaddstr(Prbwin, 0, 0, "Num Dist Crse  Target  Range Pods Time Prox\n");
  for (I = 1; I < ObjCount; I++) {
    if (Objects[I] != NULL) {
      if (Objects[I]->ObTyp == 20) {
	sprintf(scrstr,"%2d%6.0f%4.0f", I, Range(0, I), Objects[I]->Act_Crs);
	waddstr(Prbwin,scrstr);
	if (Objects[I]->Trgt == 0)
	  waddstr(Prbwin,"   Manual ");
	else {
	  Junk = Snams[Objects[I]->Trgt - 1];
	  J = strlen(Junk) / 2 + 6;
	  sprintf(scrstr,"%*s%*s", J, Junk, 10 - J, "");
	  waddstr(Prbwin,scrstr);
	}
	sprintf(scrstr,"%6.0f%4.0f",
	       Range(I, Objects[I]->Trgt-1), Objects[I]->Fuel);
	waddstr(Prbwin,scrstr);
	Z = (10000 - Objects[I]->Tim_Del) * Iter;
	sprintf(scrstr,"%5.0f%5.0f\n", Z, Objects[I]->Prx_Del);
	waddstr(Prbwin,scrstr);
      }
    }
  }
  wrefresh(Prbwin);
}


void PFire()
{
  int I, Bad;
  Digs Temp;
  boolean Stat[4];
  boolean Ok;
  reg Srec *WITH;

  waddstr(Cmdwin,"   Fire Phasers [1-4] ");
  Indigits(Temp);
  Ok = true;
  Bad = 0;
  for (I = 0; I <= 3; I++)
    Stat[I] = true;
  waddstr(Cmdwin,"   Spread [10-45] ");
  mygetstr(Cmdwin,scrstr,true);
  Bad = (sscanf(scrstr, "%d", &I) == 0);
  if (Bad > 0)
    Ok = false;
  else
    Ships[0]->Pstat[4] = I;
  WITH = Ships[0];
  for (I = 0; I <= 3; I++) {
    if (Temp[I] > 0 && Temp[I] < 5) {
      if (WITH->Pstat[Temp[I] - 1] < 0 ||
	  WITH->Pdepl[Temp[I] - 1] > 125 && WITH->Pdepl[Temp[I] - 1] < 235 ||
	  WITH->PChrg[Temp[I] - 1] <= 0) {
	Bad++;
	Stat[Temp[I] - 1] = false;
      } else
	WITH->Pstat[Temp[I] - 1] += 100;
    } else if (Temp[I] > 4)
      Ok = false;
  }
  if (!Ok) {
    BParam();
    return;
  }
  if (Bad <= 0)
    return;
  waddstr(Cmdwin," Computer: Phaser");
  if (Bad > 1)
    waddch(Cmdwin,'s');
  for (I = 1; I <= 4; I++) {
    if (!Stat[I - 1]) {
      sprintf(scrstr,"%2d", I);
      waddstr(Cmdwin,scrstr);
    }
  }
  waddstr(Cmdwin," unable to fire.");
  Endln();
}


void TFire()
{
  int I, Bad;
  Digs Temp;
  boolean Stat[6];
  boolean Ok;
  reg Srec *WITH;

  waddstr(Cmdwin,"   Fire Tubes [1-6] ");
  Indigits(Temp);
  Ok = true;
  Bad = 0;
  for (I = 0; I <= 5; I++)
    Stat[I] = true;
  WITH = Ships[0];
  for (I = 0; I <= 5; I++) {
    if (Temp[I] > 0 && Temp[I] < 7) {
      if (WITH->Tstat[Temp[I] - 1] < 0 ||
	  WITH->TDepl[Temp[I] - 1] > 135 && WITH->TDepl[Temp[I] - 1] < 225 ||
	  WITH->Tengy[Temp[I] - 1] <= 0) {
	Bad++;
	Stat[Temp[I] - 1] = false;
      } else
	WITH->Tstat[Temp[I] - 1] += 100;
    } else if (Temp[I] > 6)
      Ok = false;
  }
  if (!Ok) {
    BParam();
    return;
  }
  if (Bad <= 0)
    return;
  waddstr(Cmdwin," Computer: Tube");
  if (Bad > 1)
    waddch(Cmdwin,'s');
  for (I = 1; I <= 6; I++) {
    if (!Stat[I - 1]) {
      sprintf(scrstr,"%2d", I);
      waddstr(Cmdwin,scrstr);
    }
  }
  waddstr(Cmdwin," unable to fire.");
  Endln();
}


void PCtrl()
{
  int I, Code, R, Shp;
  double TD, Pr, X;
  boolean Ok, Ship;
  Object *WITH;

  waddstr(Cmdwin,"\nSulu: Launch Probe");
  if (Yes()) {
    if (Ships[0]->Dmge[(long)Prob]) {
      NoProb();
      return;
    }
    waddstr(Cmdwin,"   Number of pods to launch: [10+] ");
    mygetstr(Cmdwin,scrstr,true);
    Code = (sscanf(scrstr, "%d", &I) == 0);
    if (Code != 0)
      return;
    if (I > Objects[0]->Fuel) {
      NoEngy();
      return;
    }
    waddstr(Cmdwin,"   Launch probe at: ");
    Shp = GetShp(&Ok, &Ship);
    waddstr(Cmdwin,"   Set time delay: [0-15] ");
    mygetstr(Cmdwin,scrstr,true);
    sscanf(scrstr,"%lg", &TD);
    waddstr(Cmdwin,"   Proximity fuse: [50-500] ");
    mygetstr(Cmdwin,scrstr,true);
    sscanf(scrstr,"%lg", &Pr);
    if (TD > 15 || Pr < 50 || Pr > 500 || I < 10) {
      BParam();
      return;
    }
    Launch(0, &R);
    sprintf(scrstr,"Sulu: Probe %d away!", R);
    SEndln();
    WITH = Objects[R];
    WITH->Act_Spd = 2.0;
    WITH->Des_Spd = WITH->Act_Spd;
    WITH->Fuel = I;
    Objects[0]->Fuel -= I;
    Ships[0]->Fcap -= I;
    WITH->ObTyp = 20;
    if (Ship && Ok) {
      WITH->Trgt = Shp+1;
      WITH->Des_Crs = Bearing(0, Shp);
    } else
      WITH->Des_Crs = Shp;
    WITH->Tim_Del = 10000 - TD * 4;
    WITH->Prx_Del = Pr;
    WITH->Act_Crs = WITH->Des_Crs;
    X = Radians(WITH->Act_Crs);
    WITH->XPos += WITH->Act_Spd * 100 * cos(X) * Iter;
    WITH->YPos -= WITH->Act_Spd * 100 * sin(X) * Iter;
    return;
  }
  PRStt();
  waddstr(Cmdwin,"   Detonate all probes");
  if (Yes()) {
    for (I = 1; I < ObjCount; I++) {
      if (Objects[I] && (Objects[I]->ObTyp == 20))
	Objects[I]->Tim_Del = 10000.0;
    }
    sprintf(scrstr,"   Aye, %s detonating all probes.", Title);
    SEndln();
    return;
  }
  sprintf(scrstr,"%s: Control probe #", Name);
  waddstr(Cmdwin,scrstr);
  mygetstr(Cmdwin,scrstr,true);
  I = atoi(scrstr);
  if (Objects[I] == NULL || Objects[I]->ObTyp != 20) {
    BParam();
    return;
  }
  waddstr(Cmdwin,"Sulu: Detonate it");
  if (Yes()) {
    sprintf(scrstr,"   Aye, %s detonating probe %d.", Title, I);
    SEndln();
    Objects[I]->Tim_Del = 10000.0;
    return;
  }
  waddstr(Cmdwin,"   Lock it onto ");
  Shp = GetShp(&Ok, &Ship);
  WITH = Objects[I];
  if (Ship && Ok) {
    WITH->Trgt = Shp+1;
    WITH->Des_Crs = Bearing(I, Shp);
  } else {
    WITH->Trgt = 0;
    WITH->Des_Crs = Shp;
  }
  waddstr(Cmdwin,"Sulu: Locking...");
  Endln();
}


void TLoad()
{
  int I, J, k, X, Y;
  Digs Temp;
  reg Srec *WITH;

  waddstr(Cmdwin,"   [Load or Unload] ");
  getyx(Cmdwin, Y, X);
  mygetstr(Cmdwin,scrstr,false);
  scrstr[0] = toupper(scrstr[0]);
  wmove(Cmdwin, Y, X);
  if (scrstr[0]=='L') {
	waddstr(Cmdwin,"Load");
	I = 0;
  } else if (scrstr[0]=='U') {
	waddstr(Cmdwin,"Unload");
	I = 1;
  } else {
	Whatt();
	return;
  }
  waddstr(Cmdwin,"\n   Tubes [1-6] ");
  Indigits(Temp);
  WITH = Ships[0];
  if (!I) {
    for (I = 0; I <= 5; I++) {
      if (Temp[I] > 0 && Temp[I] < 7) {
	if (WITH->Tstat[Temp[I] - 1] >= 0) {
	  J = WITH->Tstat[7];
	  k = 10 - WITH->Tengy[Temp[I] - 1];
	  if (J > k)
	    J = k;
	  if (J > Objects[0]->Fuel)
	    J = (long)Objects[0]->Fuel;
	  WITH->Tengy[Temp[I] - 1] += J;
	  Objects[0]->Fuel -= J;
	  if (!cheat)
	    WITH->Fcap -= J;
	}
      }
    }
  } else {
    for (I = 0; I <= 5; I++) {
      J = WITH->Tengy[Temp[I] - 1];
      Objects[0]->Fuel += J;
      if (!cheat)
      	WITH->Fcap += J;
      else
	if (Objects[0]->Fuel > WITH->Fcap)
	  Objects[0]->Fuel = WITH->Fcap;
      WITH->Tengy[Temp[I] - 1] = 0;
    }
  }
  Endln();
  waddstr(Cmdwin,"Scott: Tube units now ");
  for (I = 0; I <= 5; I++) {
    if (WITH->Tstat[I] < 0)
      waddstr(Cmdwin,"  --");
    else {
      sprintf(scrstr,"%4d", WITH->Tengy[I]);
      waddstr(Cmdwin,scrstr);
    }
  }
  sprintf(scrstr,"  Fuel @ %0.0f", Objects[0]->Fuel);
  SEndln();
}


void MNav()
{
  double Temp;
  int Code;
  boolean Ok;

  Ok = true;
  waddstr(Cmdwin,"   Helmsman, come to course [0-360] ");
  mygetstr(Cmdwin,scrstr,true);
  Code = (sscanf(scrstr, "%lg", &Temp) == 0);
  if (Code > 0)
    Ok = false;
  else
    Objects[0]->Des_Crs = Temp;
  Ok = Getwarp(Ok);
  if (Ok) {
    Objects[0]->Trgt = 0;
    sprintf(scrstr,"Sulu: Aye, %s.",Title);
    SEndln();
  }
}


void PNav()
{
  double Temp;
  int Shp;
  boolean Ok, Ship;

  if (Ships[0]->Dmge[(long)Comp]) {
    NoComp();
    return;
  }
  Ok = false;
  waddstr(Cmdwin,"   Chekov, pursue the ");
  Shp = GetShp(&Ok, &Ship);
  if (!(Ship && Ok)) {
    ChWhat();
    return;
  }
  Ok = Getwarp(Ok);
  if (!Ok) {
    return;
  }
  Objects[0]->Trgt = Shp+1;
  Temp = Bearing(0, Shp);
  Endln();
  sprintf(scrstr,"Chekov: Aye, %s, coming to course %3.0f", Title, Temp);
  SEndln();
}


void SScan()
{
  TCount = (-1);
  Scan_(0);
}


void SDest()
{
  if (Objects[0]->Tim_Del < 1000) {
    sprintf(scrstr,"%s: Lieutenant Uhura, tie in the bridge to the master",
      Name);
    SEndln();
    waddstr(Cmdwin,"   computer.");
    Endln();
    if (Ships[0]->Dmge[(int)Comp]) {
      NoComp();
      return;
    }
    sprintf(scrstr,"Uhura: Aye, %s",Title);
    SEndln();
    sprintf(scrstr,"%s: Computer. Destruct sequence. Are you ready to copy?",
      Name);
    SEndln();
    waddstr(Cmdwin,"Computer: Working.");
    Endln();
    sprintf(scrstr,"%s: Computer, this is Captain %s of the U.S.S. %s.",
      Name, Name, Snams[0]);
    SEndln();
    waddstr(Cmdwin,"   Destruct sequence one, code 1-1A.");
    Endln();
    waddstr(Cmdwin,"Computer: Voice and code verified and correct.");
    Endln();
    waddstr(Cmdwin,"   Sequence one complete.");
    Endln();
    waddstr(Cmdwin,"Spock: This is Commander Spock, Science Officer. Destruct");
    Endln();
    waddstr(Cmdwin,"   sequence two, code 1-1A-2B.");
    Endln();
    waddstr(Cmdwin,"Computer: Voice and code verified and correct.  Sequence");
    Endln();
    waddstr(Cmdwin,"   two complete.");
    Endln();
    waddstr(Cmdwin,"Scott: This is Lieutenant Commander Scott, Chief Engineering");
    Endln();
    sprintf(scrstr,"   Officer of the U.S.S. %s. Destruct sequence", Snams[0]);
    SEndln();
    waddstr(Cmdwin,"   number three, code 1B-2B-3.");
    Endln();
    waddstr(Cmdwin,"Computer: Voice and code verified and correct.");
    Endln();
    waddstr(Cmdwin,"   Destruct sequence complete and engaged. Awaiting final");
    Endln();
    waddstr(Cmdwin,"   code for twenty second countdown.");
    Endln();
    sprintf(scrstr,"%s: Computer, this is Captain %s of the U.S.S. %s.",
      Name, Name, Snams[0]);
    SEndln();
    waddstr(Cmdwin,"   begin countdown, code 0-0-0, destruct 0.");
    Endln();
    Objects[0]->Tim_Del = 10000 - 20 / Iter;
  } else {
    sprintf(scrstr,"%s: Computer, this is Captain %s of the U.S.S. %s.",
      Name, Name, Snams[0]);
    SEndln();
    waddstr(Cmdwin,"   Code 1-2-3 continuity abort destruct order, repeat:");
    Endln();
    waddstr(Cmdwin,"   Code 1-2-3 continuity abort destruct order!");
    Endln();
    if (Ships[0]->Dmge[(int)Comp]) {
      NoComp();
      return;
    }
    waddstr(Cmdwin,"Computer: Self-destruct order ... ");
    wrefresh(Cmdwin);
    Delay(2000);
    if ((10000 - Objects[0]->Tim_Del) * Iter >= 5) {
      waddstr(Cmdwin,"aborted. Destruct order aborted.");
      Objects[0]->Tim_Del = 1.0;
    } else
       waddstr(Cmdwin,"cannot be aborted.");
    Endln();
  }
}


void ASrndr()
{
  sprintf(scrstr,"%s: Uhura, open a hailing frequency to the %ss.",
    Name, Enemy);
  SEndln();
  sprintf(scrstr,"Uhura: Aye, %s.", Title);
  SEndln();
  sprintf(scrstr,"%s: This is Captain %s of the U.S.S. %s. I give you",
    Name, Name, Snams[0]);
  SEndln();
  waddstr(Cmdwin,"   one last chance to surrender before we resume our attack.");
  Endln();
  if (Win == 5) {
    sprintf(scrstr,"%s: %s, we are already complying with your previous request!",
      Capt, Name);
    SEndln();
    return;
  }
  if (Ct[3]) {
    sprintf(scrstr,"Spock: %s, our offer has already been refused.", Title);
    SEndln();
  } else
    Ct[3]=1;
}


void Druse()
{
  int i, charge;

  sprintf(scrstr,"%s: Chekov, drop shields ...", Name);
  SEndln();
  if (Ct[0]) {
    sprintf(scrstr,"Spock: %s, the %ss are not that stupid.",
      Title, Enemy);
    SEndln();
    return;
  }
  waddstr(Cmdwin,"   Transfer power to [engines or phasers]: ");
  mygetstr(Cmdwin,scrstr,true);
  if (!scrstr[0]) {
    ChWhat();
    return;
  }
  charge = (-10);
  if (scrstr[0] == 'e' || scrstr[0] == 'E') {
    Objects[0]->Fuel+=10;
    if (Objects[0]->Fuel > Ships[0]->Fcap)
      Objects[0]->Fuel = Ships[0]->Fcap;
  } else if (scrstr[0] == 'p' || scrstr[0] == 'P')
    charge = 10;
  else {
    ChWhat();
    return;
  }
  for (i=0; i<4; i++)
    Ships[0]->Sdrns[i] = 0;
  for (i=0; i<Phasers; i++)
    Ships[0]->PCrts[i] = charge;
  Ct[0] = 1;
}


void PCmd()
{
  boolean X;
  Char Junk;

  werase(Scnwin);
  mvwaddstr(Scnwin, 0, 0, " Code     Command");
  TCount = (-1);
  X = true;
  mvwaddstr(Scnwin, 12, HelpX-13,"<Space Bar> for other page");
  mvwaddstr(Scnwin, 13, HelpX-10,"<Return> to continue");
  do {
    wmove(Scnwin, 1, 0);
    if (X) {
      waddstr(Scnwin, "  1  Fire Phasers\n");
      waddstr(Scnwin, "  2  Fire Photon Torpedoes\n");
      waddstr(Scnwin, "  3  Anti-Matter Probe Control\n");
      waddstr(Scnwin, "  4  Load/Unload Torpedoes\n");
      waddstr(Scnwin, "  5  Change Course & Speed\n");
      waddstr(Scnwin, "  6  Pursue Enemy Vessel\n");
      waddstr(Scnwin, "  7  Damage Report\n");
      waddstr(Scnwin, "  8  Self-Destruction\n");
      waddstr(Scnwin, "  9  Ask Enemy to Surrender\n");
      waddstr(Scnwin, "  0  Attempt Defenseless Ruse\n");
      waddstr(Scnwin, "  -  List Commands\n");
    } else {
      waddstr(Scnwin, "  q  Lock Phasers\n");
      waddstr(Scnwin, "  w  Lock Tubes\n");
      waddstr(Scnwin, "  e  Alter Power Distribution\n");
      waddstr(Scnwin, "  r  Change Firing Parameters\n");
      waddstr(Scnwin, "  t  Alter Display Radius\n");
      waddstr(Scnwin, "  y  Run from Enemy Vessel\n");
      waddstr(Scnwin, "  u  Scan Enemy Vessel\n");
      waddstr(Scnwin, "  i  Engineering Separation\n");
      waddstr(Scnwin, "  o  Surrender\n");
      waddstr(Scnwin, "  p  Attempt Corbomite Bluff\n");
      waddstr(Scnwin, "  [  Quit Game\n");
    }
    wrefresh(Scnwin);
    Junk = wgetch(Scnwin);
    if (Junk == ' ')
      X = !X;
  } while ((Junk != '\n') && (Junk != '\r'));
}


void Pstts()
{
  Digs temp;
  int i, shp, bad;
  boolean ok, Ship;
  Char *Junk;
  double B;
  boolean Stat[4];
  reg Srec *WITH;

  TCount = (-1);
  werase(Scnwin);
  mvwaddstr(Scnwin, 0, 0, "Phasers  Control  Turned  Levels");
  WITH = Ships[0];
  for (i = 0; i < Phasers; i++) {
    sprintf(scrstr,"%4d", i+1);
    mvwaddstr(Scnwin,i+1,0,scrstr);
    if (WITH->Pstat[i] < 0)
      waddstr(Scnwin,"     Damaged  ");
    else if (WITH->Pstat[i] == 0)
      waddstr(Scnwin,"      Manual  ");
    else {
      Junk = Snams[WITH->Pstat[i] - 1];
      shp = strlen(Junk) / 2 + 9;
      sprintf(scrstr,"%*s%*s", shp, Junk, 14 - shp, "");
      waddstr(Scnwin,scrstr);
    }
    if (WITH->Pstat[i] < 0)
      waddstr(Scnwin,"  --      --");
    else {
      sprintf(scrstr,"%4ld%8ld",
	     (long)floor(WITH->Pdepl[i] + 0.5),
	     (long)floor(WITH->PChrg[i] + 0.5));
      waddstr(Scnwin,scrstr);
    }
  }
  mvwaddstr(Scnwin,Phasers+2,0,"Charge rates: ");
  for (i = 0; i <= 3; i++) {
    sprintf(scrstr,"%4.0f", Ships[0]->PCrts[i]);
    waddstr(Scnwin,scrstr);
  }
  sprintf(scrstr,"Firing percentage: %d", Ships[0]->Pstat[5]);
  mvwaddstr(Scnwin,Phasers+3,0,scrstr);
  wrefresh(Scnwin);
  waddstr(Cmdwin,"   Lock Phasers [1-4] ");
  Indigits(temp);
  bad = 0;
  for (i = 0; i <= 3; i++)
    Stat[i] = false;
  if (temp[0] > 0) {
    TCount++;
    waddstr(Cmdwin,"   on ");
    shp = GetShp(&ok, &Ship);
    WITH = Ships[0];
    if (Ship) {
      B = Bearing(0, shp) - Objects[0]->Act_Crs;
      B = RMod(B);
      for (i = 0; i <= 3; i++) {
	if (temp[i] > 0 && temp[i] < 5) {
	  if (WITH->Pstat[temp[i] - 1] < 0 || !ok) {
	    bad++;
	    Stat[temp[i] - 1] = true;
	  } else {
	    WITH->Pstat[temp[i] - 1] = shp+1;
	    WITH->Pdepl[temp[i] - 1] = B;
	  }
	}
      }
    } else {
      for (i = 0; i <= 3; i++) {
	if (temp[i] > 0 && temp[i] < 5) {
	  if (WITH->Pstat[temp[i] - 1] >= 0) {
	    WITH->Pdepl[temp[i] - 1] = shp % 360;
	    WITH->Pstat[temp[i] - 1] = 0;
	  }
	}
      }
    }
  }
  if (bad <= 0)
    return;
  waddstr(Cmdwin,"Computer: Phaser");
  if (bad > 1)
    waddch(Cmdwin,'s');
  for (i = 0; i < Phasers; i++) {
    if (Stat[i]) {
      sprintf(scrstr,"%2d", i+1);
      waddstr(Cmdwin,scrstr);
    }
  }
  waddstr(Cmdwin," unable to turn.");
  Endln();
}


void Tstts()
{
  Digs temp;
  int i, shp, bad;
  boolean ok, Ship;
  Char *Junk;
  double B;
  boolean Stat[6];
  reg Srec *WITH;

  TCount = (-1);
  werase(Scnwin);
  mvwaddstr(Scnwin, 0, 0, " Tubes   Control  Turned  Levels");
  WITH = Ships[0];
  for (i = 0; i < Tubes; i++) {
    sprintf(scrstr,"%4d", i+1);
    mvwaddstr(Scnwin,i+1,0,scrstr);
    if (WITH->Tstat[i] < 0)
      waddstr(Scnwin,"     Damaged  ");
    else if (WITH->Tstat[i] < 1)
      waddstr(Scnwin,"      Manual  ");
    else {
      Junk = Snams[WITH->Tstat[i] - 1];
      shp = strlen(Junk) / 2 + 9;
      sprintf(scrstr,"%*s%*s", shp, Junk, 14 - shp, "");
      waddstr(Scnwin,scrstr);
    }
    if (WITH->Tstat[i] < 0)
      waddstr(Scnwin,"  --      --");
    else {
      sprintf(scrstr,"%4ld%8d",
	     (long)floor(WITH->TDepl[i] + 0.5), WITH->Tengy[i]);
      waddstr(Scnwin,scrstr);
    }
  }
  sprintf(scrstr,"Launch speed: %d", Ships[0]->Tstat[6]);
  mvwaddstr(Scnwin,Tubes+2,0,scrstr);
  sprintf(scrstr,"  Time delay: %d", Ships[0]->Tstat[7]);
  mvwaddstr(Scnwin,Tubes+3,0,scrstr);
  sprintf(scrstr,"  Prox delay: %d", Ships[0]->Tstat[8]);
  mvwaddstr(Scnwin,Tubes+4,0,scrstr);
  wrefresh(Scnwin);
  waddstr(Cmdwin,"   Lock Tubes [1-6] ");
  Indigits(temp);
  bad = 0;
  for (i = 0; i <= 5; i++)
    Stat[i] = false;
  if (temp[0] > 0) {
    TCount++;
    waddstr(Cmdwin,"   on ");
    shp = GetShp(&ok, &Ship);
    WITH = Ships[0];
    if (Ship) {
      B = Bearing(0, shp) - Objects[0]->Act_Crs;
      B = RMod(B);
      for (i = 0; i <= 5; i++) {
	if (temp[i] > 0 && temp[i] < 7) {
	  if (WITH->Tstat[temp[i] - 1] < 0 || !ok) {
	    bad++;
	    Stat[temp[i] - 1] = true;
	  } else {
	    WITH->Tstat[temp[i] - 1] = shp + 1;
	    WITH->TDepl[temp[i] - 1] = B;
	  }
	}
      }
    } else {
      for (i = 0; i <= 5; i++) {
	if (temp[i] > 0 && temp[i] < 7) {
	  if (WITH->Tstat[temp[i] - 1] >= 0) {
	    WITH->TDepl[temp[i] - 1] = shp % 360;
	    WITH->Tstat[temp[i] - 1] = 0;
	  }
	}
      }
    }
  }
  if (bad <= 0)
    return;
  waddstr(Cmdwin,"Computer: Tube");
  if (bad > 1)
    waddch(Cmdwin,'s');
  for (i = 0; i < Tubes; i++) {
    if (Stat[i]) {
      sprintf(scrstr,"%2d", i+1);
      waddstr(Cmdwin,scrstr);
    }
  }
  waddstr(Cmdwin," unable to turn.");
  Endln();
}


void AtPwr()
{
  double inbuf[4];
  int i;
  boolean Ok;

  sprintf(scrstr,"Scott: Regeneration rate is %0.2f", Ships[0]->Regen);
  SEndln();
  waddstr(Cmdwin,"   Designate shield drains (X,X,X,X).");
  Endln();
  sprintf(scrstr,"   Current drains are: %0.2f %0.2f %0.2f %0.2f", Ships[0]->Sdrns[0],
    Ships[0]->Sdrns[1], Ships[0]->Sdrns[2], Ships[0]->Sdrns[3]);
  SEndln();
  do {
  do {
    Endln();
    sprintf(scrstr,"%s: Shield drains are [0-1] ", Name);
    waddstr(Cmdwin,scrstr);
    mygetstr(Cmdwin,scrstr,true);
    i=sscanf(scrstr," %lg%*[, ]%lg%*[, ]%lg%*[, ]%lg",&inbuf[0],&inbuf[1],
      &inbuf[2], &inbuf[3]);
    if (i<4) {
      sprintf(scrstr,"Scott: A value is needed for each of the 4 shields, %s.",
	Title);
      SEndln();
    } else
      break;
  } while (i<4);
  Ok = true;
  for (i=0; i<4; i++) {
    if (inbuf[i]<0 || inbuf[i]>1) {
      Ok = false;
      break;
    }
    Ships[0]->Sdrns[i] = inbuf[i];
  }
  if (!Ok) {
    sprintf(scrstr,"Scott: Minimum drain for shields is zero, maximum is one, %s.",
      Title);
    SEndln();
    sprintf(scrstr,"%s: Son-of-a-gun, you're right.", Name);
    SEndln();
  } else
    break;
  } while (!Ok);

  waddstr(Cmdwin,"Scott: Designate phaser charge/discharge (X,X,X,X).");
  Endln();
  sprintf(scrstr,"   current rates are: %0.2f %0.2f %0.2f %0.2f",Ships[0]->PCrts[0],
    Ships[0]->PCrts[1], Ships[0]->PCrts[2], Ships[0]->PCrts[3]);
  SEndln();
  do {
  do {
    Endln();
    sprintf(scrstr,"%s: Phaser C/D rates [-10 to 10] ", Name);
    waddstr(Cmdwin,scrstr);
    mygetstr(Cmdwin,scrstr,true);
    Endln();
    i=sscanf(scrstr," %lg%*[, ]%lg%*[, ]%lg%*[, ]%lg",&inbuf[0],&inbuf[1],
      &inbuf[2], &inbuf[3]);
    if (i<4) {
      sprintf(scrstr,"Scott: A value is needed for each of the 4 phasers, %s.",
	Title);
      SEndln();
    } else
      break;
  } while (i<4);
  Ok = true;
  for (i=0; i<4; i++) {
    if (inbuf[i]<-10 || inbuf[i]>10) {
      Ok = false;
      break;
    }
    Ships[0]->PCrts[i] = inbuf[i];
  }
  if (!Ok) {
    sprintf(scrstr,"Scott: I remind you, %s, maximum discharge rate is -10 and",
      Title);
    SEndln();
    waddstr(Cmdwin,"   maximum charge rate is 10.");
    Endln();
  } else
    break;
  } while (!Ok);
}


void ChPrm()
{
  int i;
  boolean Bad;

  Endln();
  sprintf(scrstr,"Chekov:  Reset tubes, %s", Title);
  waddstr(Cmdwin,scrstr);
  if (Yes()) {
    Bad = true;
    do {
    sprintf(scrstr,"   Set launch speed to (0-12, currently %0d) ",
      Ships[0]->Tstat[Tubes]);
    waddstr(Cmdwin,scrstr);
    mygetstr(Cmdwin,scrstr,true);
    i=atoi(scrstr);
    if (i>=0 && i<=12)
      Bad = false;
    } while (Bad);
    Ships[0]->Tstat[Tubes] = i;
    Bad = true;
    do {
      sprintf(scrstr,"   . . . time delay to (0-10, currently %0d) ",
	Ships[0]->Tstat[Tubes+1]);
      waddstr(Cmdwin,scrstr);
      mygetstr(Cmdwin,scrstr,true);
      i=atoi(scrstr);
      if (i>=0 && i<=10)
	Bad = false;
    } while (Bad);
    Ships[0]->Tstat[Tubes+1] = i;
    Bad = true;
    do {
      sprintf(scrstr,"   . . . proximity delay to (0-500, currently %0d) ",
	Ships[0]->Tstat[Tubes+2]);
      waddstr(Cmdwin,scrstr);
      mygetstr(Cmdwin,scrstr,true);
      i=atoi(scrstr);
      if (i>=0 && i<=500)
	Bad = false;
    } while (Bad);
    Ships[0]->Tstat[Tubes+2] = i;
  }
  sprintf(scrstr,"Chekov:  Reset phasers, %s", Title);
  waddstr(Cmdwin,scrstr);
  if (Yes()) {
    Bad = true;
    do {
      sprintf(scrstr,"   Reset firing percentage to (0-100, currently %0d) ",
        Ships[0]->Pstat[Phasers+1]);
      waddstr(Cmdwin,scrstr);
      mygetstr(Cmdwin,scrstr,true);
      i=atoi(scrstr);
      if (i>=0 && i<=100)
	Bad = false;
    } while (Bad);
    Ships[0]->Pstat[Phasers+1] = i;
  }
}


void AtRad()
{
  int Junk;
  boolean Ok;

  TCount = (-1);
  if (Ships[0]->Dmge[(long)Snsr]) {
    NoSnsr();
    return;
  }
  do {
    waddstr(Cmdwin,"   Display radius: [50-10000] ");
    mygetstr(Cmdwin,scrstr,true);
    Junk = atoi(scrstr);
    if (Junk >= 50 && Junk <= 10000) {
      Radius = (long)Junk;
      Ok = true;
    } else {
      waddstr(Cmdwin,"\nSpock:  Designate between 50 and 10000.");
      wrefresh(Cmdwin);
      Ok = false;
    }
  } while (!Ok);
}


void ENav()
{
  double Temp;
  int Shp;
  boolean Ok, Ship;

  if (Ships[0]->Dmge[(long)Comp]) {
    NoComp();
    return;
  }
  Ok = false;
  waddstr(Cmdwin,"   Chekov, elude the ");
  Shp = GetShp(&Ok, &Ship);
  if (!(Ship && Ok)) {
    ChWhat();
    return;
  }
  Ok = Getwarp(Ok);
  if (!Ok) {
    return;
  }
  Objects[0]->Trgt = -(Shp+1);
  Temp = RMod(Bearing(0, Shp) + 180);
  sprintf(scrstr,"Chekov: Aye, %s, coming to course %3.0f", Title, Temp);
  mvwaddstr(Cmdwin,3,0,scrstr);
  Endln();
}


void EScan()
{
  int Shp;
  boolean Ok, Ship;

  Ok = true;
  waddstr(Cmdwin,"   Spock, scan the ");
  Shp = GetShp(&Ok, &Ship);
  if (Ships[0]->Dmge[(long)Snsr]) {
    NoSnsr();
    return;
  }
  if (Ship && Ok)
    Scan_(Shp);
  else
    BParam();
}


void Engin()
{
  short I, FORLIM;

  if (Ships[0]->Dmge[(long)Engn]) {
    waddstr(Cmdwin,"   Detonate the Engineering section, Scotty!");
    Endln();
    Endln();
    waddstr(Cmdwin,"Scott:  ");
    if (Bombed) {
      sprintf(scrstr,"What section? It's already gone, %s!", Title);
      waddstr(Cmdwin,scrstr);
    }
    else {
      sprintf(scrstr,"Aye, %s! Detonating Engineering now!", Title);
      waddstr(Cmdwin,scrstr);
      for (I = 0; I < ObjCount; I++) {
	if (Objects[I] != NULL) {
	  if (Objects[I]->ObTyp == 10)
	    Objects[I]->Tim_Del = 10000.0;
	}
      }
      Bombed = true;
    }
    Endln();
    return;
  }
  waddstr(Cmdwin,"   Scotty, jettison the Engineering section!!");
  Endln();
  Endln();
  waddstr(Cmdwin,"Scott:  Aye, Captain (oh, me poor engines!)");
  Endln();
  Endln();
  Detach(0);
}


void Srndr()
{
  sprintf(scrstr,"%s:  Uhura, open a channel to the %ss.", Name, Enemy);
  SEndln();
  sprintf(scrstr,"Uhura:  Aye, %s.", Title);
  SEndln();
  sprintf(scrstr,"%s:  This is Captain %s of the U.S.S. %s. Will",
    Name, Name, Snams[0]);
  SEndln();
  waddstr(Cmdwin,"   you accept my unconditional surrender?");
  Endln();
  if (Win == 4) {
    sprintf(scrstr,"Spock:  %s, we have already surrendered.", Title);
    SEndln();
    return;
  }
  if (Ct[2]) {
    sprintf(scrstr,"Spock:  The %ss have already refused.", Enemy);
    SEndln();
  } else {
    if (Enemy == Romulan) {
      waddstr(Cmdwin,"Spock:  The Romulans have not been known to have taken");
      Endln();
      waddstr(Cmdwin,"   prisoners");
      Endln();
    }
    Ct[2]=1;
  }
}


void DoBluff()
{
  if (_randint(2) == 1) {
    waddstr(Cmdwin,"   Lieutenant Uhura, open a hailing frequency, ship-to-ship.");
    Endln();
    sprintf(scrstr,"Uhura:  Hailing frequency open, %s.", Title);
    SEndln();
    sprintf(scrstr,"%s:  This is the Captain of the %s.  Our respect for", Name, Snams[0]);
    SEndln();
    waddstr(Cmdwin,"   other life forms requires that we give you this warning --");
    Endln();
    waddstr(Cmdwin,"   one critical item of information that has never been");
    Endln();
    waddstr(Cmdwin,"   incorporated into the memory banks of any Earth ship.");
    Endln();
    waddstr(Cmdwin,"   Since the early years of space exploration, Earth vessels");
    Endln();
    waddstr(Cmdwin,"   have had incorporated in them a substance known as Corbomite.");
    Endln();
    if (!Ct[1]) {
      waddstr(Cmdwin,"   It is a material and a device which discourages attack on");
      Endln();
      waddstr(Cmdwin,"   us.  If any destructive energy touches our vessel, a re-");
      Endln();
      waddstr(Cmdwin,"   verse reaction of equal strength is created, destroying");
      Endln();
      waddstr(Cmdwin,"   the attacker.  It may interest you to know that, since");
      Endln();
      waddstr(Cmdwin,"   the initial use of Corbomite for more than two of our");
      Endln();
      waddstr(Cmdwin,"   centuries ago, no attacking vessel has survived the attempt.");
      Endln();
      waddstr(Cmdwin,"   Death has little meaning to us.  If it has none to you,");
      Endln();
      waddstr(Cmdwin,"   then attack us now.  We grow annoyed at your foolishness.");
      Endln();
    }
  } else {
    waddstr(Cmdwin,"   Lieutenant Uhura, open a special channel to Starfleet command.");
    Endln();
    sprintf(scrstr,"Uhura:  Aye, %s", Title);
    SEndln();
    sprintf(scrstr,"%s:  Use code 2.", Name);
    SEndln();
    waddstr(Cmdwin,"Uhura:  But Captain, according to our last Starfleet bulletin,");
    Endln();
    sprintf(scrstr,"   the %ss have broken code 2!", Enemy);
    SEndln();
    sprintf(scrstr,"%s:  That's an order, Lieutenant! Code 2!!", Name);
    SEndln();
    waddstr(Cmdwin,"Uhura:  Yes, Captain.  Code 2.");
    Endln();
    sprintf(scrstr,"%s:  Message from %s to Starfleet Command, this sector.",
      Name, Snams[0]);
    SEndln();
    sprintf(scrstr,"   we have inadvertently encroached upon %s neutral zone,", Enemy);
    SEndln();
    sprintf(scrstr,"   surrounded and under heavy %s attack.  Escape", Enemy);
    SEndln();
    waddstr(Cmdwin,"   impossible.  Shields failing.  Will implement destruct");
    Endln();
    waddstr(Cmdwin,"   order using Corbomite device recently installed.  Since");
    Endln();
    if (!Ct[1]) {
      sprintf(scrstr,"   this will result in the destruction of the %s and", Snams[0]);
      SEndln();
      waddstr(Cmdwin,"   all matter within a 200,000 kilometer radius and");
      Endln();
      waddstr(Cmdwin,"   establish corresponding dead zone, all Federation");
      Endln();
      waddstr(Cmdwin,"   vessels will avoid this area for the next four solar");
      Endln();
      sprintf(scrstr,"   years.  Explosion will take place in one minute.  %s,", Name);
      SEndln();
      sprintf(scrstr,"   commanding %s, out.", Snams[0]);
      SEndln();
      waddstr(Cmdwin,"   Mr. Sulu, stand by.");
      Endln();
      waddstr(Cmdwin,"Sulu:  Standing by.");
      Endln();
    }
  }
  if (!Ct[1]) {
    Ct[1] = 1;
  } else {
    waddstr(Cmdwin,"Spock:  I don't believe that they will for that");
    Endln();
    sprintf(scrstr,"   maneuver again, %s.", Title);
    SEndln();
  }
}


void EStop()
{
  waddstr(Cmdwin,"Quit game");
  if (Yes())
    Stop = true;
  TCount = (-1);
}

void Command()
{
  Char Ch;
  int I, Y, X;
  boolean Looped;
#ifdef	FD_SET
  fd_set ReadFd;
#else
  int ReadFd;
#endif
  struct timeval Timeout;

  if (Redo) {
    DrawRep();
    Redo = false;
  }
  Status();
  LKnt = 0;
  waddstr(Cmdwin,Name);
  waddstr(Cmdwin,": Code ");
  wrefresh(Cmdwin);
  Looped = false;
  do {
#ifdef	FD_ZERO
    FD_ZERO(&ReadFd);
    FD_SET(0, &ReadFd);
#else
    ReadFd = 1;
#endif
    Timeout.tv_sec = Timer;
    Timeout.tv_usec = 0;
    if (!ps2_character_available()) {
      Looped = true;
      werase(Cmdwin);
    } else {
      Ch = wgetch(Cmdwin);
      Looped = true;
      waddstr(Cmdwin,"\n");
      wclrtoeol(Cmdwin);
      wrefresh(Cmdwin);
      if (isupper(Ch))
	Ch = tolower(Ch);
      switch (Ch) {
	  case '1':
	    PFire();
	    break;

	  case '2':
	    TFire();
	    break;

	  case '3':
	    PCtrl();
	    break;

	  case '4':
	    TLoad();
	    break;

	  case '5':
	    MNav();
	    break;

	  case '6':
	    PNav();
	    break;

	  case '7':
	    SScan();
	    break;

	  case '8':
	    SDest();
	    break;

	  case '9':
	    ASrndr();
	    break;

	  case '0':
	    Druse();
	    break;

	  case '-':
	    PCmd();
	    break;

	  case 'q':
	    Pstts();
	    break;

	  case 'w':
	    Tstts();
	    break;

	  case 'e':
	    AtPwr();
	    break;

	  case 'r':
	    ChPrm();
	    break;

	  case 't':
	    AtRad();
	    break;

	  case 'y':
	    ENav();
	    break;

	  case 'u':
	    EScan();
	    break;

	  case 'i':
	    Engin();
	    break;

	  case 'o':
	    Srndr();
	    break;

	  case 'p':
	    DoBluff();
	    break;

	  case '[':
	    EStop();
	    break;

	  case '':
	    wrefresh(curscr);

	  default:
	    Looped = false;
	    break;
      }
    }
  } while (!Looped);

}

int main(argc, argv)
int argc;
Char *argv[];
{
  int I;

  SysInit();
again:
  TrekInit();
  Radius = 5000;
  TCount = (-1);
  Win = 0;
  Win2 = 0;
  LKnt = 0;
  DrawScr();
  Stop = false;
  Redo = false;
  do {
    TCount++;
    if (TCount > Enemies)
      Execute();
    else if (Objects[TCount] != NULL) {
      I = Objects[TCount]->ObTyp;
      if (I < 10 && Ships[TCount] != NULL) {
	if (Ships[TCount]->Crew != 0) {
	  if (!I)
	    Command();
	  else
	    EStrat();
	}
      }
    }
  } while (!Stop);

  waddstr(Cmdwin,"Computer:  Do you desire another battle");
  if (Yes())
  {
    delwin(Prbwin);
    delwin(Scnwin);
    delwin(Diswin);
    delwin(Cmdwin);
    delwin(Repwin);
    delwin(Radwin);
    for (I = ObjCount; I >= 0; I--)
    {
      if (Objects[I])
      {
	free(Objects[I]);
	Objects[I] = NULL;
      }
    }
    for (I = 0; I<10; I++)
    {
      if (Ships[I])
      {
      	free(Ships[I]);
	Ships[I] = NULL;
      }
    }
    goto again;
  }
  endwin();
  exit(0);
}
/* End. */
