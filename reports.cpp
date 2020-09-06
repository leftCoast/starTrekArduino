#include <Arduino.h>
#include "sst.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void attakreport(void) 
{
	if (future[FCDBAS] < 1e30) {
		proutn((char*)"Starbase in ");
		cramlc(1, batx, baty);
		prout((char*)" is currently under attack.");
		proutn((char*)"It can hold out until Stardate ");
		cramf(future[FCDBAS], 0,1);
		prout((char*)".");
	}
	if (isatb == 1) {
		proutn((char*)"Starbase in ");
		cramlc(1, d.isx, d.isy);
		prout((char*)" is under Super-commander attack.");
		proutn((char*)"It can hold out until Stardate ");
		cramf(future[FSCDBAS], 0, 1);
		prout((char*)".");
	}
}
	

void report(int f) {

	char*	s1;
	char*	s2;
	char*	s3;
	char	buf[128];

	chew();
	if(thawed) {
		s1 = (char*)"thawed ";
	} else {
		s1=(char*)"";
	}
	switch (length) {
		case 1: s2=(char*)"short"; break;
		case 2: s2=(char*)"medium"; break;
		case 4: s2=(char*)"long"; break;
		default: s2=(char*)"unknown length"; break;
	}
	switch (skill) {
		case SNOVICE: s3=(char*)"novice"; break;
		case SFAIR: s3=(char*)"fair"; break;
		case SGOOD: s3=(char*)"good"; break;
		case SEXPERT: s3=(char*)"expert"; break;
		case SEMERITUS: s3=(char*)"emeritus"; break;
		default: s3=(char*)"skilled"; break;
	}
	sprintf(buf,"\nYou %s playing a %s%s %s game.\n",alldone? " were": "are", s1, s2, s3);
	proutn(buf);
	if (skill>SGOOD && thawed && !alldone) prout((char*)"No plaque is allowed.");
	if (tourn) {
		sprintf(buf,"This is tournament game %d.\n", tourn);
		proutn(buf);
	}
	if (f) {
		sprintf(buf,"Your secret password is \"%s\"\n",passwd);
		proutn(buf);
	}
	sprintf(buf,"%d of %d Klingon ships have been destroyed",d.killk+d.killc+d.nsckill, inkling);
	proutn(buf);
	if (d.killc) {
		sprintf(buf,", including %d Commander%s.\n", d.killc, d.killc==1?"":"s");
		proutn(buf);
	} else if (d.killk+d.nsckill > 0) {
		prout((char*)", but no Commanders.");
	} else {
		prout((char*)".");
	}
	if (skill > SFAIR) {
		sprintf(buf,"The Super Commander has %sbeen destroyed.\n",d.nscrem?"not ":"");
		proutn(buf);
	}
	if (d.rembase != inbase) {
		proutn((char*)"There ");
		if (inbase-d.rembase==1) {
			proutn((char*)"has been 1 base");
		} else {
			proutn((char*)"have been ");
			crami(inbase-d.rembase, 1);
			proutn((char*)" bases");
		}
		proutn((char*)" destroyed, ");
		crami(d.rembase, 1);
		prout((char*)" remaining.");
	} else {
		sprintf(buf,"There are %d bases.\n", inbase);
		proutn(buf);
	}
	if (REPORTS || iseenit) {
		/* Don't report this if not seen and
			either the radio is dead or not at base! */
		attakreport();
		iseenit = 1;
	}
	if (casual) {
		sprintf(buf,"%d casualt%s suffered so far.\n",casual, casual==1? "y" : "ies");
		proutn(buf);
	}
#ifdef CAPTURE
	if (brigcapacity != brigfree) {
		sprintf(buf,"%d Klingon%s in brig.\n",brigcapacity-brigfree, brigcapacity-brigfree>1 ? "s" : "");
    	proutn(buf);
	}
	if (kcaptured > 0) {
		sprintf(buf,"%d captured Klingon%s turned in to Star Fleet.\n",kcaptured, kcaptured>1 ? "s" : "");
		proutn(buf);
	}
#endif
	if (nhelp) {
		sprintf(buf,"There %s %d call%s for help.\n",nhelp==1 ? "was" : "were", nhelp, nhelp==1 ? "" : "s");
		proutn(buf);
	}
	if (ship == IHE) {
		proutn((char*)(char*)"You have ");
		if (nprobes) {
			crami(nprobes,1);
		} else {
			proutn((char*)"no");
		}
		proutn((char*)" deep space probe");
		if (nprobes!=1) {
			proutn((char*)"s");
		}
		prout((char*)".");
	}
	if (REPORTS && future[FDSPROB] != 1e30) {
		if (isarmed) 
			proutn((char*)"An armed deep space probe is in");
		else
			proutn((char*)"A deep space probe is in");
		cramlc(1, probecx, probecy);
		prout((char*)".");
	}
	if (icrystl) {
		if (cryprob <= .05)
			prout((char*)"Dilithium crystals aboard ship...not yet used.");
		else {
			int i=0;
			double ai = 0.05;
			while (cryprob > ai) {
				ai *= 2.0;
				i++;
			}
			sprintf(buf,"Dilithium crystals have been used %d time%s.\n",i, i==1? "" : "s");
			proutn(buf);
		}
	}
	skip(1);
}

	
void lrscan(void) {
	
	int	x;
	int	y;
	char	buf[128];
	
	chew();
	if (damage[DLRSENS] != 0.0) {
		/* Now allow base's sensors if docked */
		if (condit != IHDOCKED) {
			prout((char*)"LONG-RANGE SENSORS DAMAGED.");
			return;
		}
		skip(1);
		proutn((char*)"Starbase's long-range scan for");
	}
	else {
		skip(1);
		proutn((char*)"Long-range scan for");
	}
	cramlc(1, quadx, quady);
	skip(1);
	if (coordfixed) {
		for (y = quady+1; y >= quady-1; y--) {
			for (x = quadx-1; x <= quadx+1; x++) {
				if (x == 0 || x > 8 || y == 0 || y > 8)
					MyPuts("   -1");
				else {
					sprintf(buf,"%5d", d.galaxy[x][y]);
					proutn(buf);
					// If radio works, mark star chart so
					// it will show current information.
					// Otherwise mark with current
					// value which is fixed. 
					starch[x][y] = damage[DRADIO] > 0 ? d.galaxy[x][y]+1000 :1;
				}
			}
			MyPutChar('\n');
		}
	} else {
		for (x = quadx-1; x <= quadx+1; x++) {
			for (y = quady-1; y <= quady+1; y++) {
				if (x == 0 || x > 8 || y == 0 || y > 8)
					MyPuts("   -1");
				else {
					sprintf(buf,"%5d", d.galaxy[x][y]);
					proutn(buf);
					// If radio works, mark star chart so
					// it will show current information.
					// Otherwise mark with current
					// value which is fixed. 
					starch[x][y] = damage[DRADIO] > 0 ? d.galaxy[x][y]+1000 :1;
				}
			}
			MyPutChar('\n');
		}
	}
}


void dreprt(void) {
	
	int	i;
	int	jdam;
	char	buf[128];
	
	jdam = FALSE;
	chew();
	for (i = 1; i <= ndevice; i++) {
		if (damage[i] > 0.0) {
			if (!jdam) {
				skip(1);
				prout((char*)"DEVICE            -REPAIR TIMES-");
				prout((char*)"                IN FLIGHT   DOCKED");
				jdam = TRUE;
			}
			sprintf(buf,"  %16s ", device[i]);
			proutn(buf);
			if (i == DDRAY) { // Deathray is special case
				proutn((char*)"           ");
				cramf(damage[i]+0.005, 8, 2);
			} else {
				cramf(damage[i]+0.05, 8, 2);
				proutn((char*)"  ");
				cramf(docfac*damage[i]+0.005, 8, 2);
			}
			skip(1);
		}
	}
	if (!jdam) prout((char*)"All devices functional.");
}


/*
void chart(int nn) {

	int	i;
	int	j;
	char	buf[128];

	chew();
	skip(1);
	if (stdamtim != 1e30 && stdamtim != d.date && condit == IHDOCKED) {
		prout((char*)"Spock-  \"I revised the Star Chart from the");
		prout((char*)"  starbase's records.\"");
		skip(1);
	}
	if (nn == 0) prout((char*)"STAR CHART FOR THE KNOWN GALAXY");
	if (stdamtim != 1e30) {
		if (condit == IHDOCKED) {
			// We are docked, so restore chart from base information -- these values won't update! 
			stdamtim = d.date;
			for (i=1; i <= 8 ; i++)
				for (j=1; j <= 8; j++)
					if (starch[i][j] == 1) starch[i][j] = d.galaxy[i][j]+1000;
		}
		else {
			proutn((char*)"(Last surveillance update ");
			cramf(d.date-stdamtim, 0, 1);
			prout((char*)" stardates ago.)");
		}
	}
	if (nn ==0) skip(1);

	prout((char*)"      1    2    3    4    5    6    7    8");
	prout((char*)"    ----------------------------------------");
	if (nn==0) prout((char*)"  -");
	if (coordfixed) {
		for (j = 8; j >= 1; j--) {
			sprintf(buf,"%d -", j);
			proutn(buf);
			for (i = 1; i <= 8; i++) {
				if (starch[i][j] < 0) // We know only about the bases
					MyPuts("  .1.");
				else if (starch[i][j] == 0) // Unknown
					MyPuts("  ...");
				else if (starch[i][j] > 999) { 				// Memorized value
					sprintf(buf,"%5d", starch[i][j]-1000);
					proutn(buf);
				} else {
					sprintf(buf,"%5d", d.galaxy[i][j]); 				// What is actually there (happens when value is 1)
					proutn(buf);
				}
			}
			prout((char*)"  -");
		}
	}	else {
		for (i = 1; i <= 8; i++) {
			sprintf("%d -", i);
			proutn(buf);
			for (j = 1; j <= 8; j++) {
				if (starch[i][j] < 0) 											// We know only about the bases
					MyPuts("  .1.");
				else if (starch[i][j] == 0)									// Unknown
					MyPuts("  ...");
				else if (starch[i][j] > 999) {									// Memorized value
					sprintf(buf,"%5d", starch[i][j]-1000);
					proutn(buf);
				} else {
					sprintf(buf,"%5d", d.galaxy[i][j]); 							// What is actually there (happens when value is 1)
					proutn(buf);
				}
			}
			prout((char*)"  -");
		}
	}
	if (nn == 0) {
		skip(1);
		crmshp();
		proutn((char*)" is currently in");
		cramlc(1, quadx, quady);
		skip(1);
	}
}
		
		
void srscan(int l) 
{
	static char requests[][3] =
		{"","da","co","po","ls","wa","en","to","sh","kl","ti"};
	char *cp;
	int leftside=TRUE, rightside=TRUE, i, j, k=0, nn=FALSE;
	int goodScan=TRUE;
  char buf[128];
  
	switch (l) {
		case 1: // SRSCAN
			if (damage[DSRSENS] != 0) {
				// Allow base's sensors if docked 
				if (condit != IHDOCKED) {
					prout((char*)"SHORT-RANGE SENSORS DAMAGED");
					goodScan=FALSE;
				}
				else
					prout((char*)"[Using starbase's sensors]");
			}
			if (goodScan)
				starch[quadx][quady] = damage[DRADIO]>0.0 ?
									   d.galaxy[quadx][quady]+1000:1;
			scan();
			if (isit((char*)"chart")) nn = TRUE;
			if (isit((char*)"no")) rightside = FALSE;
			chew();
			prout((char*)"\n    1 2 3 4 5 6 7 8 9 10");
			break;
		case 2: // REQUEST
			while (scan() == IHEOL)
				MyPuts("Information desired? ");
			chew();
			for (k = 1; k <= 10; k++)
				if (strncmp(citem,requests[k],min(2,strlen(citem)))==0)
					break;
			if (k > 10) {
				prout((char*)"UNRECOGNIZED REQUEST. Legal requests are:\n"
					 "  date, condition, position, lsupport, warpfactor,\n"
					 "  energy, torpedoes, shields, klingons, time.");
				return;
			}
			// no "break"
		case 3: // STATUS
			chew();
			leftside = FALSE;
			skip(1);
	}
	for (i = 1; i <= 10; i++) {
		int jj = (k!=0 ? k : i);
		if (leftside) {
			if (coordfixed) {
				sprintf(buf,"%2d  ", 11-i);
        MyPuts(buf);
				for (j = 1; j <= 10; j++) {
					if (goodScan || (abs((11-i)-secty)<= 1 && abs(j-sectx) <= 1))
            {
						sprintf(buf,"%c ",quad[j][11-i]);
            MyPuts(buf);
            }
					else
						MyPuts("- ");
				}
			} else {
				sprintf(buf,"%2d  ", i);
        MyPuts(buf);
        
				for (j = 1; j <= 10; j++) {
					if (goodScan || (abs(i-sectx)<= 1 && abs(j-secty) <= 1))
            {
						sprintf(buf,"%c ",quad[i][j]);
            MyPuts(buf);
            }
					else
						MyPuts("- ");
				}
			}
		}
		
		if (rightside) {
			switch (jj) {
				case 1:
					sprintf(buf," Stardate      %.1f", d.date);
          MyPuts(buf);
					break;
				case 2:
					if (condit != IHDOCKED) newcnd();
					switch (condit) {
						case IHRED: cp = (char*)"RED"; break;
						case IHGREEN: cp = (char*)"GREEN"; break;
						case IHYELLOW: cp = (char*)"YELLOW"; break;
						case IHDOCKED: cp = (char*)"DOCKED"; break;
					}
					sprintf(buf," Condition     %s", cp);
          MyPuts(buf);
#ifdef CLOAKING
				    if (iscloaked) MyPuts(", CLOAKED");
#endif
					break;
				case 3:
					MyPuts(" Position     ");
					cramlc(0, quadx, quady);
					MyPutChar(',');
					cramlc(0, sectx, secty);
					break;
				case 4:
					MyPuts(" Life Support  ");
					if (damage[DLIFSUP] != 0.0) {
						if (condit == IHDOCKED)
							MyPuts("DAMAGED, supported by starbase");
						else
              {
							sprintf(buf,"DAMAGED, reserves=%4.2f", lsupres);
              MyPuts(buf);
              }
					}
					else
						MyPuts("ACTIVE");
					break;
				case 5:
					sprintf(buf," Warp Factor   %.1f", warpfac);
          MyPuts(buf);
					break;
				case 6:
					sprintf(buf," Energy        %.2f", energy);
          MyPuts(buf);
					break;
				case 7:
					sprintf(buf," Torpedoes     %d", torps);
          MyPuts(buf);
					break;
				case 8:
					MyPuts(" Shields       ");
					if (damage[DSHIELD] != 0)
						MyPuts("DAMAGED,");
					else if (shldup)
						MyPuts("UP,");
					else
						MyPuts("DOWN,");
					sprintf(buf," %d%% %.1f units",(int)((100.0*shield)/inshld + 0.5), shield);
          MyPuts(buf);
					break;
				case 9:
					sprintf(buf," Klingons Left %d", d.remkl);
          MyPuts(buf);
					break;
				case 10:
					sprintf(buf," Time Left     %.2f", d.remtime);
          MyPuts(buf);
					break;
			}
					
		}
		skip(1);
		if (k!=0) return;
	}
	if (nn) chart(1);
}
*/	

void chart(int nn) {

	int	i;
	int	j;
	char	buf[128];

	chew();
	skip(1);
	if (stdamtim != 1e30 && stdamtim != d.date && condit == IHDOCKED) {
		prout((char*)"Spock-  \"I revised the Star Chart from the");
		prout((char*)"  starbase's records.\"");
		prouts("            ");	// wait for a bit..
		skip(1);
	}
	clearscreen();
	if (nn == 0) {
		prout((char*)"           THE KNOWN GALAXY");
		proutn((char*)"  ");
		crmshp();
		proutn((char*)" located in");
		cramlc(1, quadx, quady);
	}
	if (stdamtim != 1e30) {
		if (condit == IHDOCKED) { // We are docked, so restore chart from base information -- these values won't update! 
			stdamtim = d.date;
			for (i=1; i <= 8 ; i++)
				for (j=1; j <= 8; j++)
					if (starch[i][j] == 1) starch[i][j] = d.galaxy[i][j]+1000;
		} else {
			proutn((char*)"(Last surveillance update ");
			cramf(d.date-stdamtim, 0, 1);
			prout((char*)" stardates ago.)");
		}
	}
	if (nn ==0) skip(1);

	prout((char*)"     1 | 2 | 3 | 4 | 5 | 6 | 7 | 8");
	if (coordfixed) {
		for (j = 8; j >= 1; j--) {
			sprintf(buf,"%d", j);
			proutn(buf);
			for (i = 1; i <= 8; i++) {
				if (starch[i][j] < 0) // We know only about the bases
					MyPuts(" .1.");
				else if (starch[i][j] == 0) // Unknown
					MyPuts(" ...");
				else if (starch[i][j] > 999) { 				// Memorized value
					sprintf(buf,"%4d", starch[i][j]-1000);
					proutn(buf);
				} else {
					sprintf(buf,"%4d", d.galaxy[i][j]); 				// What is actually there (happens when value is 1)
					proutn(buf);
				}
			}
			//prout((char*)"  -");
		}
	}	else {
		for (i = 1; i <= 8; i++) {
			sprintf(buf,"%d :", i);
			proutn(buf);
			for (j = 1; j <= 8; j++) {
				if (starch[i][j] < 0) 											// We know only about the bases
					MyPuts(" .1.");
				else if (starch[i][j] == 0)									// Unknown
					MyPuts(" ...");
				else if (starch[i][j] > 999) {									// Memorized value
					sprintf(buf,"%4d", starch[i][j]-1000);
					proutn(buf);
				} else {
					sprintf(buf,"%4d", d.galaxy[i][j]); 							// What is actually there (happens when value is 1)
					proutn(buf);
				}
			}
			proutn("\n");
		}
	}
	/*
	if (nn == 0) {
		crmshp();
		proutn((char*)" is in");
		cramlc(1, quadx, quady);
		skip(1);
	}
	*/
}


// Short range scan. But what is this "l" we are passing in? It is used in a giant switch
// statment to select who knows what?
void srscan(int l) {
	
	static char requests[][3] = {"","da","co","po","ls","wa","en","to","sh","kl","ti"};
	char*	cp;
	bool	leftside;
	bool	rightside;
	int	i; 
	int	j;
	int	k;
	bool	nn;
	bool	goodScan;
  	char	buf[128];
  
  	leftside		= true;
	rightside	= true;
	k				= 0;
	nn				= false;
	goodScan		= true;
	switch (l) {
		case 1: // SRSCAN
			MyPuts("   Quadrant : ");											   // Lets put out our position as the Title line.
			cramlc(0, quadx, quady);												// Format and output quadrant.
			MyPuts("   Sector : ");													// Ouput Sector label..
			cramlc(0, sectx, secty);												// And lets see the sector.
			
			if (damage[DSRSENS] != 0) {											// If sensors are damaged..
				if (condit != IHDOCKED) {											// Allow base's sensors if docked.
					prout((char*)"SHORT-RANGE SENSORS DAMAGED");				// Tell 'em we got no sensors.
					goodScan = false;													// Note we didn't do a scan.
				} else {																	// Else, we are docked..
					prout((char*)"[Using starbase's sensors]");				// Tell 'em we're using the starbase sensor array.
				}
			}
			if (goodScan) {															// If we had a good scan..
				if (damage[DRADIO]>0.0) {											// If the radio is damaged?
					starch[quadx][quady] = d.galaxy[quadx][quady]+1000;	// We put something into the star chart, not sure what. But its 1000 more than the galaxy spot.
				} else {																	// Else, no radio damage?
					starch[quadx][quady] = 1;										// We put a 1 into the star chart. Very confusing..
				}
			}
			scan();
			if (isit((char*)"chart")) nn = TRUE;
			if (isit((char*)"no")) rightside = FALSE;
			chew();
			prout((char*)"\n    1 2 3 4 5 6 7 8 9 10");
		break;
		case 2: // REQUEST
			while (scan() == IHEOL)
				MyPuts("Information desired? ");
			chew();
			for (k = 1; k <= 10; k++)
				if (strncmp(citem,requests[k],min(2,strlen(citem)))==0)
					break;
			if (k > 10) {
				prout((char*)"UNRECOGNIZED REQUEST. Legal requests are:\n"
					 "  date, condition, position, lsupport, warpfactor,\n"
					 "  energy, torpedoes, shields, klingons, time.");
				return;
			}
		// no "break"
		case 3: // STATUS
			chew();
			leftside = FALSE;
			skip(1);
	}
	for (i = 1; i <= 10; i++) {
		int jj = (k!=0 ? k : i);
		if (leftside) {
			if (coordfixed) {
				sprintf(buf,"%2d  ", 11-i);
        		MyPuts(buf);
				for (j = 1; j <= 10; j++) {
					if (goodScan || (abs((11-i)-secty)<= 1 && abs(j-sectx) <= 1)) {
						sprintf(buf,"%c ",quad[j][11-i]);
						MyPuts(buf);
            	} else {
						MyPuts("- ");
					}
				}
			} else {
				sprintf(buf,"%2d  ", i);
        		MyPuts(buf);
				for (j = 1; j <= 10; j++) {
					if (goodScan || (abs(i-sectx)<= 1 && abs(j-secty) <= 1)) {
						sprintf(buf,"%c ",quad[i][j]);
            		MyPuts(buf);
            	} else {
						MyPuts("- ");
					}
				}
			}
		}
		if (rightside) {
			switch (jj) {
				case 1:
					sprintf(buf," Std %.1f", d.date);
          		MyPuts(buf);
				break;
				case 2:
					if (condit != IHDOCKED) newcnd();
					switch (condit) {
						case IHRED: cp = (char*)"RED"; break;
						case IHGREEN: cp = (char*)"GREEN"; break;
						case IHYELLOW: cp = (char*)"YELLOW"; break;
						case IHDOCKED: cp = (char*)"DOCKED"; break;
					}
					sprintf(buf," Cond %s", cp);
          		MyPuts(buf);
#ifdef CLOAKING
					if (iscloaked) MyPuts(", CLOAKED");
#endif
				break;
				case 3:
					MyPuts(" Life Spt");
					if (damage[DLIFSUP] != 0.0) {
						if (condit == IHDOCKED) {
							MyPuts(" On");
						} else {
							sprintf(buf," Res. =%4.1f", lsupres);
              			MyPuts(buf);
              		}
					} else
						MyPuts(" On");
				break;
				case 4:
					sprintf(buf," Warp %.1f", warpfac);
          		MyPuts(buf);
				break;
				case 5:
					sprintf(buf," E %.1f", energy);
         		 MyPuts(buf);
				break;
				case 6:
					sprintf(buf," Torp %d", torps);
          		MyPuts(buf);
				break;
				case 7:
					MyPuts(" Shld ");
					if (damage[DSHIELD] != 0)
						MyPuts("Dmg");
					else if (shldup)
						MyPuts("Up");
					else
						MyPuts("Off");
				break;
				case 8:
					MyPuts(" Shld ");
					sprintf(buf," %d%%",(int)((100.0*shield)/inshld + 0.5));
					 MyPuts(buf);
				break;	
				case 9:
					sprintf(buf," Klingons %d", d.remkl);
          		MyPuts(buf);
					break;
				case 10:
					sprintf(buf," T left %.1f", d.remtime);
          		MyPuts(buf);
				break;
			}	
		}
		skip(1);
		if (k!=0) return;
	}
	if (nn) {
		chart(1);
	}
}


void eta(void) {
	int ix1, ix2, iy1, iy2, prompt=FALSE;
	int wfl;
	double ttime, twarp, tpower;
	if (damage[DCOMPTR] != 0.0) {
		prout((char*)"COMPUTER DAMAGED, USE A POCKET CALCULATOR.");
		skip(1);
		return;
	}
	if (scan() != IHREAL) {
		prompt = TRUE;
		chew();
		proutn((char*)"Destination quadrant and/or sector? ");
		if (scan()!=IHREAL) {
			huh();
			return;
		}
	}
	iy1 = aaitem +0.5;
	if (scan() != IHREAL) {
		huh();
		return;
	}
	ix1 = aaitem + 0.5;
	if (scan() == IHREAL) {
		iy2 = aaitem + 0.5;
		if (scan() != IHREAL) {
			huh();
			return;
		}
		ix2 = aaitem + 0.5;
	}
	else {	// same quadrant
		ix2 = ix1;
		iy2 = iy1;
		ix1 = quady;	// ya got me why x and y are reversed!
		iy1 = quadx;
	}

	if (ix1 > 8 || ix1 < 1 || iy1 > 8 || iy1 < 1 ||
		ix2 > 10 || ix2 < 1 || iy2 > 10 || iy2 < 1) {
		huh();
		return;
	}
	dist = sqrt(square(iy1-quadx+0.1*(iy2-sectx))+
				square(ix1-quady+0.1*(ix2-secty)));
	wfl = FALSE;

	if (prompt) prout((char*)"Answer \"no\" if you don't know the value:");
	while (TRUE) {
		chew();
		proutn((char*)"Time or arrival date? ");
		if (scan()==IHREAL) {
			ttime = aaitem;
			if (ttime > d.date) ttime -= d.date; // Actually a star date
			if (ttime <= 1e-10 ||
				(twarp=(floor(sqrt((10.0*dist)/ttime)*10.0)+1.0)/10.0) > 10) {
				prout((char*)"We'll never make it, sir.");
				chew();
				return;
			}
			if (twarp < 1.0) twarp = 1.0;
			break;
		}
		chew();
		proutn((char*)"Warp factor? ");
		if (scan()== IHREAL) {
			wfl = TRUE;
			twarp = aaitem;
			if (twarp<1.0 || twarp > 10.0) {
				huh();
				return;
			}
			break;
		}
		prout((char*)"Captain, certainly you can give me one of these.");
	}
	while (TRUE) {
		chew();
		ttime = (10.0*dist)/square(twarp);
		tpower = dist*twarp*twarp*twarp*(shldup+1);
		if (tpower >= energy) { // Suggestion from Ethan Staffin -- give amount needed
			prout((char*)"Insufficient energy, sir: we would need ");
			cramf(tpower, 1, 1);
			proutn ((char*)" units.");
			if (shldup==0 || tpower > energy*2.0) {
				if (!wfl) return;
				proutn((char*)"New warp factor to try? ");
				if (scan() == IHREAL) {
					wfl = TRUE;
					twarp = aaitem;
					if (twarp<1.0 || twarp > 10.0) {
						huh();
						return;
					}
					continue;
				}
				else {
					chew();
					skip(1);
					return;
				}
			}
			prout((char*)"But if you lower your shields,");
			proutn((char*)"remaining");
			tpower /= 2;
		}
		else
			proutn((char*)"Remaining");
		proutn((char*)" energy will be ");
		cramf(energy-tpower, 1, 1);
		prout((char*)".");
		if (wfl) {
			proutn((char*)"And we will arrive at stardate ");
			cramf(d.date+ttime, 1, 1);
			prout((char*)".");
		}
		else if (twarp==1.0)
			prout((char*)"Any warp speed is adequate.");
		else {
			proutn((char*)"Minimum warp needed is ");
			cramf(twarp, 1, 2);
			skip(1);
			proutn((char*)"and we will arrive at stardate ");
			cramf(d.date+ttime, 1, 2);
			prout((char*)".");
		}
		if (d.remtime < ttime)
			prout((char*)"Unfortunately, the Federation will be destroyed by then.");
		if (twarp > 6.0)
			prout((char*)"You'll be taking risks at that speed, Captain");
		if ((isatb==1 && d.isy == ix1 && d.isx == iy1 &&
			 future[FSCDBAS]< ttime+d.date)||
			(future[FCDBAS]<ttime+d.date && baty==ix1 && batx == iy1))
			prout((char*)"The starbase there will be destroyed by then.");
		proutn((char*)"New warp factor to try? ");
		if (scan() == IHREAL) {
			wfl = TRUE;
			twarp = aaitem;
			if (twarp<1.0 || twarp > 10.0) {
				huh();
				return;
			}
		}
		else {
			chew();
			skip(1);
			return;
		}
	}
}
