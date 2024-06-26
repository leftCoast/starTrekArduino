#define INCLUDED	// Define externs here
#include <Arduino.h>
#include "sst.h"
#include <ctype.h>
#ifdef MSDOS
#include <dos.h>
#endif


static char line[128], *linep = line;
static int linecount;	/* for paging */

//static void clearscreen(void);

/* Compared to original version, I've changed the "help" command to
   "call" and the "terminate" command to "quit" to better match
   user expectations. The DECUS version apparently made those changes
   as well as changing "freeze" to "save". However I like "freeze".

   When I got a later version of Super Star Trek that I was converting
   from, I added the emexit command.

   That later version also mentions srscan and lrscan working when
   docked (using the starbase's scanners), so I made some changes here
   to do this (and indicating that fact to the player), and then realized
   the base would have a subspace radio as well -- doing a Chart when docked
   updates the star chart, and all radio reports will be heard. The Dock
   command will also give a report if a base is under attack.

   Movecom no longer reports movement if sensors are damaged so you wouldn't
   otherwise know it.

   Also added:

   1. Better base positioning at startup

   2. deathray improvement (but keeping original failure alternatives)

   3. Tholian Web

   4. Enemies can ram the Enterprise. Regular Klingons and Romulans can
      move in Expert and Emeritus games. This code could use improvement.

   5. The deep space probe looks interesting! DECUS version

   6. Cloaking (with contributions from Erik Olofsen) and Capturing (BSD version).

*/

// I don't like the way this is done, relying on an index. But I don't
// want to invest the time to make this nice and table driven.

static char *commands[] = {
  (char*)"srscan",
  (char*)"lrscan",
  (char*)"phasers",
  (char*)"photons",
  (char*)"move",
  (char*)"shields",
  (char*)"dock",
  (char*)"damages",
  (char*)"chart",
  (char*)"impulse",
  (char*)"rest",
  (char*)"warp",
  (char*)"status",
  (char*)"sensors",
  (char*)"orbit",
  (char*)"transport",
  (char*)"mine",
  (char*)"crystals",
  (char*)"shuttle",
  (char*)"planets",
  (char*)"request",
  (char*)"report",
  (char*)"computer",
  (char*)"commands",
  (char*)"emexit",
  (char*)"probe",
  (char*)"cloak",
  (char*)"capture",
  (char*)"score",
  (char*)"abandon",
  (char*)"destruct",
  (char*)"freeze",
  (char*)"deathray",
  (char*)"debug",
  (char*)"call",
  (char*)"quit",
  (char*)"help"

};

const int NUMCOMMANDS = (sizeof(commands)/sizeof(char *));

static void listCommands(int x) {
  prout((char*)"SRSCAN    MOVE    PHASERS  CALL\n"
        "STATUS    IMPULSE PHOTONS  ABANDON\n"
        "LRSCAN    WARP    SHIELDS  DESTRUCT\n"
        "CHART     REST    DOCK     QUIT\n"
        "DAMAGES   REPORT  SENSORS  ORBIT\n"
        "TRANSPORT MINE    CRYSTALS SHUTTLE\n"
        "PLANETS   REQUEST DEATHRAY FREEZE\n"
        "COMPUTER  EMEXIT  PROBE    COMMANDS");
#ifdef SCORE
  proutn((char*)"SCORE     ");
#endif
#ifdef CLOAKING
  proutn((char*)"CLOAK     ");
#endif
#ifdef CAPTURE
  proutn((char*)"CAPTURE   ");
#endif
  if (x) proutn((char*)"HELP     ");
  prout((char*)"");
}


static void helpme(void) {

  prout((char*)"Spock-  \"Captain, that information is missing from the");
  prout((char*)"   computer. You need to find SST.DOC and put it in the");
  prout((char*)"   current directory.\"");
}


static void makemoves(void) {

  int i, hitme;
  
  while (TRUE) { /* command loop */
    hitme = FALSE;
    justin = 0;
    Time = 0.0;
    i = -1;
    while (TRUE) { /* get a command */
      int matched = 0;
      chew();
      skip(1);
      proutn((char*)"COMMAND> ");
      if (scan() == IHEOL) {
      	if (quickExit) {
      		return;
      	}
      	continue;
      }
      for (i = 0; i < 29; i++) // Abbreviations allowed for the first 29 commands, only.
        if (isit(commands[i]))
          break;
      if (i < 29) {
        matched = 1;
      } else {
        for (; i < NUMCOMMANDS; i++)
          if (strcmp(commands[i], citem) == 0) {
            matched = 1;
            break;
          }
      }
      if (matched
#ifndef CLOAKING
          && i != 26 // ignore the CLOAK command
#endif
#ifndef CAPTURE
          && i != 27 // ignore the CAPTURE command
#endif
#ifndef SCORE
          && i != 28 // ignore the SCORE command
#endif
#ifndef DEBUG
          && i != 33 // ignore the DEBUG command
#endif
         ) break;

      if (skill <= SFAIR) {
        prout((char*)"UNKNOWN COMMAND. LEGAL COMMANDS ARE:\n");
        listCommands(TRUE);
      } else prout((char*)"UNKNOWN COMMAND.");
    }
    switch (i) { /* command switch */
      case 0: // srscan
        srscan(1);
        break;
      case 1: // lrscan
        lrscan();
        break;
      case 2: // phasers
        phasers();
        if (ididit) {
#ifdef CLOAKING
          if (irhere && d.date >= ALGERON && !isviolreported && iscloaked) {
            prout((char*)"The Romulan ship discovers you are breaking the Treaty of Algeron!");
            ncviol++;
            isviolreported = TRUE;
          }
#endif
          hitme = TRUE;
        }
        break;
      case 3: // photons
        photon();
        if (ididit) {
#ifdef CLOAKING
          if (irhere && d.date >= ALGERON && !isviolreported && iscloaked) {
            prout((char*)"The Romulan ship discovers you are breaking the Treaty of Algeron!");
            ncviol++;
            isviolreported = TRUE;
          }
#endif
          hitme = TRUE;
        }
        break;
      case 4: // move
        warp(1);
        break;
      case 5: // shields
        sheild(1);
        if (ididit) {
          attack(2);
          shldchg = 0;
        }
        break;
      case 6: // dock
        dock();
        break;
      case 7: // damages
        dreprt();
        break;
      case 8: // chart
        chart(0);
        break;
      case 9: // impulse
        impuls();
        break;
      case 10: // rest
        waiting();
        if (ididit) hitme = TRUE;
        break;
      case 11: // warp
        setwarp();
        break;
      case 12: // status
        srscan(3);
        break;
      case 13: // sensors
        sensor();
        break;
      case 14: // orbit
        orbit();
        if (ididit) hitme = TRUE;
        break;
      case 15: // transport "beam"
        beam();
        break;
      case 16: // mine
        mine();
        if (ididit) hitme = TRUE;
        break;
      case 17: // crystals
        usecrystals();
        break;
      case 18: // shuttle
        shuttle();
        if (ididit) hitme = TRUE;
        break;
      case 19: // Planet list
        planetreport();
        break;
      case 20: // Status information
        srscan(2);
        break;
      case 21: // Game Report
        report(0);
        break;
      case 22: // use COMPUTER!
        eta();
        break;
      case 23:
        listCommands(TRUE);
        break;
      case 24: // Emergency exit
        clearscreen(); // Hide screen
        freeze(TRUE); // forced save
        alldone = 1; // quit the game.
        quickExit = true;
        return;
      case 25:
        probe(); // Launch probe
        break;
#ifdef CLOAKING
      case 26:
        cloak(); // turn on/off cloaking
        if (iscloaking) {
          attack(2); // We will be seen while we cloak
          iscloaking = FALSE;
          if (damage[DCLOAK] == 0) // don't cloak if we got damaged while cloaking!
            iscloaked = TRUE;
        }
        break;
#endif
#ifdef CAPTURE
      case 27:
        capture(); // Attempt to get Klingon ship to surrender
        if (ididit) hitme = TRUE;
        break;
#endif
#ifdef SCORE
      case 28:
        score(1); // get the score
        break;
#endif
      case 29: // Abandon Ship
        abandon();
        break;
      case 30: // Self Destruct
        dstrct();
        break;
      case 31: // Save Game
        freeze(FALSE);
        if (skill > SGOOD)
          prout((char*)"WARNING--Frozen games produce no plaques!");
        break;
      case 32: // Try a desparation measure
        deathray();
        if (ididit) hitme = TRUE;
        break;
#ifdef DEBUG
      case 33: // What do we want for debug???
        debugme();
        break;
#endif
      case 34: // Call for help
        help();
        break;
      case 35:
        alldone = 1; // quit the game
#ifdef DEBUG
        if (idebug) score(0);
#endif
        break;
      case 36:
        helpme(); // get help
        break;
    }
    for (;;) {
      if (alldone) break; // Game has ended
#ifdef DEBUG
      if (idebug) prout("2500");
#endif
      if (Time != 0.0) {
        events();
        if (alldone) break; // Events did us in
      }
      if (d.galaxy[quadx][quady] == 1000) { // Galaxy went Nova!
        atover(0);
        continue;
      }
      if (nenhere == 0) movetho();
      if (hitme && justin == 0) {
        attack(2);
        if (alldone) break;
        if (d.galaxy[quadx][quady] == 1000) { // went NOVA!
          atover(0);
          hitme = TRUE;
          continue;
        }
      }
      break;
    }
    if (alldone) break;
  }
}

int runsst (int argc, char **argv) 
{
  prelim();

  if (argc > 1) { // look for -f option
    if (strcmp(argv[1], "-f") == 0) {
      coordfixed = 1;
      argc--;
      argv++;
    }
  }

  if (argc > 1) {
    fromcommandline = 1;
    line[0] = '\0';
    while (--argc > 0) {
      strcat(line, *(++argv));
      strcat(line, " ");
    }
  }
  else fromcommandline = 0;

  while (TRUE) { /* Play a game */
    setupsst();
     if (quickExit) return 0;
    if (alldone) {
      score(0);
      alldone = 0;
    }
    else makemoves();
    if (quickExit) return 0;
    skip(2);
    stars();
    skip(1);

    if (tourn && alldone) {
      prout((char*)"Do you want your score recorded?");
      if (ja()) {
        chew2();
        freeze(FALSE);
      }
    }
    proutn((char*)"Do you want to play again?");
    if (!ja()) break;
  }
  skip(1);
  prout((char*)"May the Great Bird of the Galaxy roost upon your home planet.");
  return 0;
}


void doMakeMoves(void) { makemoves(); } // Hook for Arduino to call makemoves();


void cramen(int i) 
{
  /* return an enemy */
  char *s;

  switch (i) 
  {
    case IHR: s = (char*)"Romulan"; break;
    case IHK: s = (char*)"Klingon"; break;
    case IHC: s = (char*)"Commander"; break;
    case IHS: s = (char*)"Super-commander"; break;
    case IHSTAR: s = (char*)"Star"; break;
    case IHP: s = (char*)"Planet"; break;
    case IHB: s = (char*)"Starbase"; break;
    case IHBLANK: s = (char*)"Black hole"; break;
    case IHT: s = (char*)"Tholian"; break;
    case IHWEB: s = (char*)"Tholian web"; break;
    default: s = (char*)"Unknown??"; break;
  }
  proutn(s);
}


void cramlc(int key, int x, int y) {
  
	if (key == 1) {
		proutn((char*)" Quadrant");
	} else if (key == 2) {
		proutn((char*)" Sector");
	}
	proutn((char*)" ");
	crami(x, 1);
	proutn((char*)",");
	crami(y, 1);
}


void crmena(int i, int enemy, int key, int x, int y) {
  
  if (i == 1) {
  	proutn((char*)"***");
  }
  cramen(enemy);
  proutn((char*)" at");
  cramlc(key, x, y);
}


void crmshp(void) {

  char* s;
  
  switch (ship) {
    case IHE: s = (char*)"Enterprise"; break;
    case IHF: s = (char*)"Faerie Queene"; break;
    default:  s = (char*)"Ship???"; break;
  }
  proutn(s);
}


void stars(void) {
	
	proutn((char*)"          ");
  	prouts((char*)"* * * * * * * * *");
  	skip(1);
}


// I really wonder what this does. -jim lee
double expran(double avrage) 
{ return -avrage * log(1e-7 + Rand()); }


// Now uses the Arduino random seed thing. (See arduinoTrek.cpp) - jim lee
double Rand(void) { return random(0,RAND_MAX) / (1.0 + (double)RAND_MAX); }


// Also now uses the Arduino random seed thing. - jim lee
void iran8(int *i, int *j) {

  *i = random(1,9);
  *j = random(1,9);
}

// And this as well now uses the random seed thing. - jim lee
void iran10(int *i, int *j) {

  *i = random(1,11);
  *j = random(1,11); 
}


void chew(void) {

	linecount = 0;
	linep = line;
	line[0] = '\0';
}

void chew2(void) {

	linecount = 0;
	linep = &(line[1]);
	line[1] = '\0';
}

// Get a string from the user. Re-written for Arduino. -jim lee
void gets(char* str,int len) {

	char	ch;
	int	count=0;

	while(count<len-1) {					// While we have not run out of buffer..
		ch = (char)getch();				// Grab a char.
		if (quickExit) return;			// If the close has been called.. Bolt!
		if(ch=='\n' || ch=='\0') {		// If its either a '\n' or '\0'..
			str[count]='\0';				// Stuff in a '\0'.
			return;							// And we are done!
		} else {								// Else it wasn't.
			str[count]=ch;					// So we save it in the buffer at index count.
		}
		count++;								// Bump up count.
		str[count]= '\0';					// Just in case we pre-load the buffer at index count with a '\0'.
	}
}


// We must scan the incoming stream of characters for a value token or a text
// token. Where we are in the input string is saved by the gloabal linep.
// I wrote the comments working out what this was doing - jim lee
int scan(void) {

	int	i;
	char*	cp;

	if (quickExit) return 0;																	// If the user has called it quits in the UI.. Cut & run.
	linecount = 0;																					// The global linecount gets defaulted to zero.
	aaitem = 0.0;																					// The global aaitem gets defaulted to a zero.
	*citem = 0;																						// The global citem gets defaulted to empty string.
	if (*linep == 0) {																			// If linep is pointing at \0..
		if (linep != line) {																		// If the line ptr is not pointing at the beginning of the line buff..
			chew();																					// chew() makes linecont = 0 and clears the line buff.
			return IHEOL;																			// return IHEOL.. Need to look that up. (I think it means "I Have End Of Line" but I'm not positive.)
		}
		gets(line, sizeof(line));																// gets() will spin on getch() until it gets a '\n'. Does not copy the '\n';
		if (quickExit) return 0;
		linep = line;																				// A fresh bit of text means we need to reset the linep pointer to the beginning of the text.
	}
	while (*linep == ' ') linep++;															// Skip leading white space..
	if (*linep == 0) {																			// If we are now pointing at '\0'..
		chew();																						// chew() makes the global linecont = 0 and points the linep at the beginning of the global line buff. 
		return IHEOL;																				// return IHEOL.. Again, need to look that up.
	}																									// At this point, linep is pointing at first non white space in the non empty global line.
	if (isdigit(*linep) || *linep == '+' || *linep == '-' || *linep == '.') {	// If its a diget, plus, minus or period.. ( Treat it as a number. )
		if (sscanf(linep, "%lf%n", &aaitem, &i) < 1) {									// If we can NOT find a double in this mess.. (i gets the count of chars it read.)
			linep = line; 																			// Reset linep to point at the beginning of line.
			*linep = 0;																				// Stamp a '\0' at the head of line. (Effectively clearing the input.)
			return IHEOL;																			// Return.. YOu guessed it, IHEOL.
		} else {																						// Else we DID find a number in line it got written into the global aaitem.
			linep += i;																				// It says "skip to end". It adds the number chars to the pointer value, upping its index.
			return IHREAL;																			// Return IHREAL. This means, we parsed a real into the global aaitem.
		}
	}																									// At this point line is treated as alpha. (linep is pointing to the first nonwhite char.)
	cp = citem;																						// cp now points at the beginning of citem. citem is a global cstr of 24 bytes.
	while (*linep && *linep != ' ') {														// While what linep is pointing at is not '\0' or a blank..
		if ((cp - citem) < 9) {																	// If.. ( This mess means cp is pointing to array index 0..8 of the global citem )
			*cp++ = tolower(*linep);															// Stuff lowercase version of what linep is currently pointing at. Into global citem at the location cp is pointing at. Then, increment the cp pointer.							 
		}																								// Oh, so clever that last line..
		linep++;																						// In any case, update linep to the next character.
	}																	
	*cp = 0;																							// Loop exited, stomp a '\0' into global citem at the location cp is pointing to. 
	return IHALPHA;																				// And I assume this means we tell the world we just parsed an alpha token.
}



int ja(void) {
  
  chew();																	// Clear incoming stream.
  while (TRUE) {															// While forever do..
    scan();																	// Grab a token from the incoming stream.
    if (quickExit) return 0;											// If the user has called it quits in the UI.. Cut & run.
    chew();																	// Clear incoming stream.
    if (*citem == 'y') return TRUE;									// Looks like a yes, we'll go with that.
    if (*citem == 'n') return FALSE;								// Looks like a no, that's fine too.
    proutn((char*)"Please answer with \"Y\" or \"N\":");		// We're only looking for a yes / no here.
  }
}

void cramf(double x, int w, int d) {
  char buf[64];
  sprintf(buf, "%*.*f", w, d, x);
  proutn(buf);
}


void crami(int i, int w) {
  char buf[16];
  sprintf(buf, "%*d", w, i);
  proutn(buf);
}

// Hack to make this work with Arduino Mega on Wokwi. Didn't work, did it?
// void crami(int i, int w) {
//   char buf[16];
//   for(int j=0;j<16;j++) buf[j] = ' ';
//   itoa(i,buf,10);
//   if (w<16) {
//    while(strlen(buf)<w) strcat(buf," ");
//   }
//   proutn(buf);
// }


double sqr(double i) {
  return i * i;
}

// Somehow we need to clear the screen. Hope for an ANSI display
void clearscreen(void) { proutn((char*)"\e[2J"); }


/* We will pull these out in case we want to do something special later */

void pause(int i) {

#ifdef CLOAKING
  if (iscloaked) return;
#endif
  proutCh('\n');
  if (i == 1) {
    if (skill > SFAIR)
      prout((char*)"[ ANNOUNCEMENT ARRIVING... ]");
    else
      prout((char*)"[ IMPORTANT ANNOUNCEMENT ARRIVING -- HIT ENTER TO CONTINUE ]");
    getch();
    if (quickExit) return;
  }
  else {
    if (skill > SFAIR)
      proutn((char*)"[ CONTINUE? ]");
    else
      proutn((char*)"[ HIT ENTER TO CONTINUE ]");
    getch();
    if (quickExit) return;
    proutn((char*)"\r                           \r");
  }
  if (i != 0) {
    clearscreen();
  }
  linecount = 0;
}


void skip(int i) {
  while (i-- > 0) {
    linecount++;
    if (linecount >= 23)
      pause(0);
    else
      proutCh('\n');
  }
}


void prout(char *s) {
  proutn(s);
  skip(1);
}


void huh(void) 
{
  chew();
  skip(1);
  prout((char*)"Beg your pardon, Captain?");
}


int isit(char *s) {
  /* New function -- compares s to scaned citem and returns true if it
     matches to the length of s */

  return strncmp(s, citem, max(1, strlen(citem))) == 0;

}

#ifdef DEBUG
void debugme(void) {
  proutn("Reset levels? ");
  if (ja() != 0) {
    if (energy < inenrg) energy = inenrg;
    shield = inshld;
    torps = intorps;
    lsupres = inlsr;
  }
  proutn("Reset damage? ");
  if (ja() != 0) {
    int i;
    for (i = 0; i <= ndevice; i++) if (damage[i] > 0.0) damage[i] = 0.0;
    stdamtim = 1e30;
  }
  proutn("Toggle idebug? ");
  if (ja() != 0) {
    idebug = !idebug;
    if (idebug) prout("Debug output ON");
    else prout("Debug output OFF");
  }
  proutn("Cause selective damage? ");
  if (ja() != 0) {
    int i, key;
    for (i = 1; i <= ndevice; i++) {
      proutn("Kill ");
      proutn(device[i]);
      proutn("? ");
      chew();
      key = scan();
      if (quickExit) return;
      if (key == IHALPHA &&  isit("y")) {
        damage[i] = 10.0;
        if (i == DRADIO) stdamtim = d.date;
      }
    }
  }
  proutn("Examine/change events? ");
  if (ja() != 0) {
    int i;
    for (i = 1; i < NEVENTS; i++) {
      int key;
      if (future[i] == 1e30) continue;
      switch (i) {
        case FSNOVA:  proutn("Supernova       "); break;
        case FTBEAM:  proutn("T Beam          "); break;
        case FSNAP:   proutn("Snapshot        "); break;
        case FBATTAK: proutn("Base Attack     "); break;
        case FCDBAS:  proutn("Base Destroy    "); break;
        case FSCMOVE: proutn("SC Move         "); break;
        case FSCDBAS: proutn("SC Base Destroy "); break;
      }
      cramf(future[i] - d.date, 8, 2);
      chew();
      proutn("  ?");
      key = scan();
      if (quickExit) return;
      if (key == IHREAL) {
        future[i] = d.date + aaitem;
      }
    }
    chew();
  }
  proutn("Make universe visible? ");
  if (ja() != 0) {
    int i, j;
    for (i = 1; i < 9; i++)
    {
      for (j = 1; j < 9; j++)
      {
        starch[i][j] = 1;
      }
    }
  }
}
#endif
