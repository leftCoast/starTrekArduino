#include "sst.h"
#include <string.h>


void dstrct() {
	/* Finish with a BANG! */
	chew();
	if (damage[DCOMPTR] != 0.0) {
		prout((char*)"Computer damaged; cannot execute destruct sequence.");
		return;
	}
	skip(1);
	prouts((char*)"---WORKING---"); skip(1);
	prout((char*)"SELF-DESTRUCT-SEQUENCE-ACTIVATED");
	prouts((char*)"   10"); skip(1);
	prouts((char*)"       9"); skip(1);
	prouts((char*)"          8"); skip(1);
	prouts((char*)"             7"); skip(1);
	prouts((char*)"                6"); skip(1);
	prout((char*)"ENTER-CORRECT-PASSWORD-TO-CONTINUE-");
	prout((char*)"SELF-DESTRUCT-SEQUENCE-OTHERWISE-");
	prout((char*)"SELF-DESTRUCT-SEQUENCE-WILL-BE-ABORTED");
	scan();
	chew();
	if (strcmp(passwd, citem) != 0) {
		prouts((char*)"PASSWORD-REJECTED;"); skip(1);
		prout((char*)"CONTINUITY-EFFECTED");
		skip(1);
		return;
	}
	prouts((char*)"PASSWORD-ACCEPTED"); skip(1);
	prouts((char*)"                   5"); skip(1);
	prouts((char*)"                      4"); skip(1);
	prouts((char*)"                         3"); skip(1);
	prouts((char*)"                            2"); skip(1);
	prouts((char*)"                              1"); skip(1);
	if (Rand() < 0.15) {
		prouts((char*)"GOODBYE-CRUEL-WORLD");
		skip(1);
	}
	skip(2);
	kaboom();
}

void kaboom(void) {
	stars();
	if (ship==IHE) prouts((char*)"***");
	prouts((char*)"********* Entropy of ");
	crmshp();
	prouts((char*)" maximized *********");
	skip(1);
	stars();
	skip(1);
	if (nenhere != 0) {
		double whammo = 25.0 * energy;
		int l=1;
		while (l <= nenhere) {
			if (kpower[l]*kdist[l] <= whammo) 
				deadkl(kx[l],ky[l], quad[kx[l]][ky[l]], kx[l], ky[l]);
			l++;
		}
	}
	finish(FDILITHIUM);
}



// *****************************************************
// Message blocks
// *****************************************************



// Promotion thing
int doPromotion(void) {

	switch (skill) {
		case SNOVICE:
			prout((char*)"promotes you from \"Novice\" to \"Fair\".");
		break;
		case SFAIR:
			prout((char*)"promotes you from \"Fair\" to \"Good\".");
		break;
		case SGOOD:
			prout((char*)"promotes you from \"Good\" to \"Expert\".");
		break;
		case SEXPERT:
			prout((char*)"promotes you to Commodore Emeritus.");
			skip(1);
			prout((char*)"Now that you think you're really good, try playing");
			prout((char*)"the \"Emeritus\" game. It will splatter your ego.");
		break;
		case SEMERITUS:
			skip(1);
			prout((char*)"Computer-  ERROR-ERROR-ERROR-ERROR");
			skip(1);
			prout((char*)"  YOUR-SKILL-HAS-EXCEEDED-THE-CAPACITY-OF-THIS-PROGRAM");
			prout((char*)"  THIS-PROGRAM-MUST-SURVIVE");
			prout((char*)"  THIS-PROGRAM-MUST-SURVIVE");
			prout((char*)"  THIS-PROGRAM-MUST-SURVIVE");
			prouts((char*)"  THIS-PROGRAM-MUST?- MUST ? - SUR? ? -?  VI");
			skip(1);
			prout((char*)"Now you can retire and write your own Star Trek game!");
			skip(1);
		break;
	}
	if (skill > SGOOD) {
		if (thawed
#ifdef DEBUG
		&& !idebug
#endif
			) {
			prout((char*)"You cannot get a citation, so...");
		} else {
			prout((char*)"Do you want your Commodore Emeritus Citation printed?");
			proutn((char*)"(You need a 132 column printer.)");
			chew();
			if (ja()) {
				return 1;
			}
		}
	}
	return 0;
}


// Federation Resources Depleted
void resourceDepleted(void) {
			
	prout((char*)"Your time has run out and the Federation has been");
	prout((char*)"conquered.  Your starship is now Klingon property,");
	prout((char*)"and you are put on trial as a war criminal.  On the");
	proutn((char*)"basis of your record, you are ");
	if (d.remkl*3.0 > inkling) {
		prout((char*)"aquitted.");
		skip(1);
		prout((char*)"LIVE LONG AND PROSPER.");
	}
	else {
		prout((char*)"found guilty and");
		prout((char*)"sentenced to death by slow torture.");
		alive = 0;
	}
	score(0);
}


// Life support has run out, you died.
void noLifeSupport(void) {

	prout((char*)"Your life support reserves have run out, and");
	prout((char*)"you die of thirst, starvation, and asphyxiation.");
	prout((char*)"Your starship is a derelict in space.");
}


//energy has run out, you died.
void noEnergy(void) {

	prout((char*)"Your energy supply is exhausted.");
	skip(1);
	prout((char*)"Your starship is a derelict in space.");
}


// As the name says, you died in battle.
void battleDeath(void) {

	proutn((char*)"The ");
	crmshp();
	prout((char*)" has been destroyed in battle.");
	skip(1);
	prout((char*)"Dulce et decorum est pro patria mori.");
}


// Repeated off map death.
void boundryDeath(void) {

	prout((char*)"You have made three attempts to cross the negative energy");
	prout((char*)"barrier which surrounds the galaxy.");
	skip(1);
	prout((char*)"Your navigation is abominable.");
	score(0);
}			
			

// You blew up a star, dummy!
void novaDeath(void) {

	prout((char*)"Your starship has been destroyed by a nova.");
	prout((char*)"That was a great shot.");
	skip(1);
}


// You were fried by a supernova.			
void superNovaDeath(void) {

	proutn((char*)"The ");
	crmshp();
	prout((char*)" has been fried by a supernova.");
	prout((char*)"...Not even cinders remain...");
}


// Captured death. So many ways to di huh?
void capturedDeath(void) {

	prout((char*)"You have been captured by the Klingons. If you still");
	prout((char*)"had a starbase to be returned to, you would have been");
	prout((char*)"repatriated and given another chance. Since you have");
	prout((char*)"no starbases, you will be mercilessly tortured to death.");
}	


// Not sure what this one is..
void vaporizeDeath(void) { prout((char*)"Your starship is now an expanding cloud of subatomic particles"); }


	
void rematerializeError(void) {

	prout((char*)"Starbase was unable to re-materialize your starship.");
	prout((char*)"Sic transit gloria muntdi");
}	


// I guess you shot yourself.
void phaserDeath(void) {

	proutn((char*)"The ");
	crmshp();
	prout((char*)" has been cremated by its own phasers.");
}

void vaporPartyDeath(void) {

	prout((char*)"You and your landing party have been");
	prout((char*)"converted to energy, dissipating through space.");
}


void	strandedDeath(void) {

	prout((char*)"You are left with your landing party on");
	prout((char*)"a wild jungle planet inhabited by primitive cannibals.");
	skip(1);
	prout((char*)"They are very fond of \"Captain Kirk\" soup.");
	skip(1);
	proutn((char*)"Without your leadership, the ");
	crmshp();
	prout((char*)" is destroyed.");
}			
	

void blowUpPlanetDeath(void) {
	
	prout((char*)"You and your mining party perish.");
	skip(1);
	prout((char*)"That was a great shot.");
	skip(1);
}


void shuttleCookedDeath(void) {

	prout((char*)"You and your mining party are atomized.");
	skip(1);
	proutn((char*)"Mr. Spock takes command of the ");
	crmshp();
	prout((char*)" and");
	prout((char*)"joins the Romulans, reigning terror on the Federation.");
}	


void shuttleBreakupDeath(void) {

	prout((char*)"The shuttle craft Galileo is also caught,");
	prout((char*)"and breaks up under the strain.");
	skip(1);
	prout((char*)"Your debris is scattered for millions of miles.");
	proutn((char*)"Without your leadership, the ");
	crmshp();
	prout((char*)" is destroyed.");
}	


void mutantDeath(void) {

	prout((char*)"The mutants attack and kill Spock.");
	prout((char*)"Your ship is captured by Klingons, and");
	prout((char*)"your crew is put on display in a Klingon zoo.");
}	


void tribbleDeath(void) {

	prout((char*)"Tribbles consume all remaining water,");
	prout((char*)"food, and oxygen on your ship.");
	skip(1);
	prout((char*)"You die of thirst, starvation, and asphyxiation.");
	prout((char*)"Your starship is a derelict in space.");
}


void blackHoleDeath(void) {

	prout((char*)"Your ship is drawn to the center of the black hole.");
	prout((char*)"You are crushed into extremely dense matter.");
}

#ifdef CLOAKING
void caughtCloaked(void) {

	prout((char*)"You have violated the Treaty of Algeron.");
	prout((char*)"The Romulan Empire can never trust you again.");
}


void uncloakWarning(void) {

	prout((char*)"Your ship was cloaked so your subspace radio did not receive anything.");
	prout((char*)"You may have missed some warning messages.");
	skip(1);
}
#endif


void fuzzyEnding(void) {

	if (ship==IHF) {
		ship= 0;
	} else if (ship == IHE) {
		ship = IHF;
	}
	alive = 0;
	if (d.remkl != 0) {
		double goodies = d.remres/inresor;
		double baddies = (d.remkl + 2.0*d.remcom)/(inkling+2.0*incom);
		if (goodies/baddies >= 1.0+0.5*Rand()) {
			prout((char*)"As a result of your actions, a treaty with the Klingon");
			prout((char*)"Empire has been signed. The terms of the treaty are");
			if (goodies/baddies >= 3.0+Rand()) {
				prout((char*)"favorable to the Federation.");
				skip(1);
				prout((char*)"Congratulations!");
			}
			else
				prout((char*)"highly unfavorable to the Federation.");
		}
		else
			prout((char*)"The Federation will be destroyed.");
	} else {
		prout((char*)"Since you took the last Klingon with you, you are a");
		prout((char*)"martyr and a hero. Someday maybe they'll erect a");
		prout((char*)"statue in your memory. Rest in peace, and try not");
		prout((char*)"to think about pigeons.");
		gamewon = 1;
	}
}



// *****************************************************
// End message blocks.
// *****************************************************



void finish(FINTYPE ifin) {
  
	char buf[128]; 
	int igotit;
	double badpt;
	
	alldone = 1;
	skip(3);
	sprintf(buf,"It is stardate %.1f.\n\n", d.date);
	proutn(buf);
	switch (ifin) {
		case FWON: // Game has been won
			prout((char*)"You have smashed the Klingon invasion fleet and saved the Federation.");
			if (d.nromrem != 0) {
				sprintf(buf,"The remaining %d Romulan ships surrender to Starfleet Command.",d.nromrem);
        		prout(buf);
			}   
#ifdef CAPTURE
			if (alive && brigcapacity-brigfree > 0) { // captured Klingon crew will get transfered to starbase
				kcaptured += brigcapacity-brigfree;
				sprintf(buf,"The %d captured Klingons are transferred to Star Fleet Command.\n",brigcapacity-brigfree);
				proutn(buf);
			}
#endif
			gamewon=1;
			if (alive) {
				badpt = 5.*d.starkl + casual + 10.*d.nplankl + 45.*nhelp+100.*d.basekl;
				if (ship == IHF) {
					badpt += 100.0;
				} else if (ship == 0) {
					 badpt += 200.0;
				}
				if (badpt < 100.0) {
					badpt = 0.0;	// Close enough!
				}
				if (d.date-indate < 5.0 
					|| (d.killk+d.killc+d.nsckill)/(d.date-indate)
					>= 0.1*skill*(skill+1.0) + 0.1 + 0.008*badpt) {
					skip(1);
					proutn((char*)"In fact, you have done so well that Starfleet Command ");
					igotit = doPromotion();
				}
				//skip(1);
				prout((char*)"       LIVE LONG AND PROSPER."); // Only grant long life if alive. (original didn't!)
			}
			score(0);
			if (igotit != 0) {
				plaque();
			}
		return;
		
		// from here on down its not a total victory, death, destruction. Battle to a draw..
		case FDEPLETE:			resourceDepleted();		return;
		case FLIFESUP:			noLifeSupport();			break;
		case FNRG:				noEnergy();					break;
		case FBATTLE:			battleDeath();				break;
		case FNEG3:				boundryDeath();			return;
		case FNOVA:				novaDeath();				break;
		case FSNOVAED:			superNovaDeath();			break;
		case FABANDN:			capturedDeath();			break;
		case FDILITHIUM: 		vaporizeDeath();			break;
		case FMATERIALIZE:	rematerializeError();	break;
		case FPHASER:			phaserDeath();				break;
		case FLOST:				vaporPartyDeath();		break;
		case FMINING:			strandedDeath();			break;
		case FDPLANET:			blowUpPlanetDeath();		break;
		case FSSC: prout((char*)"The Galileo is instantly annihilated by the supernova."); // no break;
		case FPNOVA:			shuttleCookedDeath();	break;
		case FSTRACTOR:		shuttleBreakupDeath();	break;
		case FDRAY:				mutantDeath();				break;
		case FTRIBBLE:			tribbleDeath();			break;
		case FHOLE:				blackHoleDeath();			break;
#ifdef CLOAKING
		case FCLOAK:			
			ncviol++;
			caughtCloaked();
		break;
#endif
	}
#ifdef CLOAKING
	if (ifin!=FWON && ifin!=FCLOAK && iscloaked!=0) {
		uncloakWarning();
	}
#endif
	fuzzyEnding();	// Some sort of "sue for peace" thing.
	score(0);
}


void score(int inGame) {
	
	double	timused;
	int		ithperd; 
	int		iwon;
	int		klship;
	int		dnromrem;
	char 		buf[128];
	
	timused = d.date - indate;
	dnromrem = d.nromrem; 			// Leave global value alone
	
	if (!inGame) {
		pause(0);
	}
	iskill = skill;
	if ((timused == 0 || d.remkl != 0) && timused < 5.0) { 
		timused = 5.0;
	}
	perdate = (d.killc + d.killk + d.nsckill)/timused;
	ithperd = 500*perdate + 0.5;
	iwon = 0;
	if (gamewon) {
		iwon = 100*skill;
	}
	if (ship == IHE) {
		klship = 0;
	} else if (ship == IHF) {
		klship = 1;
	} else {
		klship = 2;
	}
	if (gamewon == 0 || inGame) {	// None captured if no win or if still in the game
		dnromrem = 0; 
	}
	iscore = 10*d.killk + 50*d.killc + ithperd + iwon
			 - 100*d.basekl - 100*klship - 45*nhelp -5*d.starkl - casual
		 + 20*d.nromkl + 200*d.nsckill - 10*d.nplankl + dnromrem;
#ifdef CLOAKING
	iscore -= 100*ncviol;
#endif
#ifdef CAPTURE
	iscore += 3*kcaptured;
#endif
	if (alive == 0) iscore -= 200;
	skip(2);
	if (inGame) {
		prout((char*)"Your score so far --");
	} else {
		prout((char*)"Your score --");
	}
	
	if (d.nromkl) {
		sprintf(buf,d.nromkl> 1 ? "%4d Romulans destroyed %5d\n" : "%4d Romulan destroyed %5d\n",d.nromkl, 20*d.nromkl);
		MyPuts(buf);
	}
	if (dnromrem) {
		sprintf(buf,dnromrem > 1 ? "%4d Romulans surrendered %5d\n" : "%4d Romulan surrendered %5d\n",dnromrem, dnromrem);
		MyPuts(buf);
	}
	if (d.killk) {
		sprintf(buf,d.killk > 1 ? "%4d Klingons destroyed %5d\n" : "%4d Klingon ship destroyed %5d\n",d.killk,  10*d.killk);
		MyPuts(buf);
	}
	if (d.killc) {
		sprintf(buf,d.killc > 1 ? "%4d Commanders destroyed %5d\n" : "%4d Commander destroyed %5d\n",d.killc, 50*d.killc);
		MyPuts(buf);
	}
	if (d.nsckill) {
		sprintf(buf,"%4d Super-Commander destroyed %5d\n",d.nsckill, 200*d.nsckill);
		MyPuts(buf);
	}
	if (ithperd) {
		sprintf(buf,"%6.2f Klingons per s.date %5d\n",perdate, ithperd);
		MyPuts(buf);
	}
#ifdef CAPTURE
	if (kcaptured) {
		sprintf(buf,kcaptured > 1 ? "%4d Klingons captured %5d\n" : "%4d Klingon captured %5d\n",kcaptured, 3*kcaptured);
		MyPuts(buf);
	}
#endif
	if (d.starkl) {
		sprintf(buf,d.starkl > 1 ? "%4d stars destroyed %5d\n" : "%4d star destroyed %5d\n",d.starkl, -5*d.starkl);
		MyPuts(buf);
	}
	if (d.nplankl) {
		sprintf(buf,d.nplankl > 1 ? "%4d planets destroyed %5d\n" : "%4d planet destroyed %5d\n",d.nplankl, -10*d.nplankl);
		MyPuts(buf);
	}
	if (d.basekl) {
		sprintf(buf,d.basekl > 1 ? "%4d bases destroyed %5d\n" : "%4d base destroyed %5d\n",d.basekl, -100*d.basekl);
		MyPuts(buf);
	}
	if (nhelp) {
		sprintf(buf,nhelp > 1 ? "%4d calls for help %5d\n" : "%4d call for help %5d\n",nhelp, -45*nhelp);
		MyPuts(buf);
	}
	if (casual) {
		sprintf(buf,casual > 1 ? "%4d casualties %5d\n" : "%4d casualty %5d\n",casual, -casual);
		MyPuts(buf);
	}
	if (klship) {
		sprintf(buf,klship > 1 ? "%4d ships lost %5d\n" : "%4d ship lost %5d\n",klship, -100*klship);
		MyPuts(buf);
	}
#ifdef CLOAKING
	if (ncviol>0) {
		sprintf(buf,ncviol > 1 ? "%4d Treaty violations %5d\n" : "%4d Treaty violation %5d\n",ncviol, -100*ncviol);
		MyPuts(buf);
	}
#endif
	if (alive==0)
		prout((char*)"Penalty for getting killed -200");
	if (gamewon) {
		skip(1);
		proutn((char*)"Winning bonus, ");
		switch (skill) {
			case SNOVICE: proutn((char*)"Novice game"); break;
			case SFAIR: proutn((char*)"Fair game"); break;
			case SGOOD: proutn((char*)"Good game"); break;
			case SEXPERT: proutn((char*)"Expert game"); break;
			case SEMERITUS: proutn((char*)"Emeritus game"); break;
		}
		sprintf(buf,"%4d\n",iwon);
		proutn(buf);
	}
	skip(1);
	sprintf(buf,"TOTAL SCORE %4d\n",iscore);
	proutn(buf);
	if (inGame && skill < SGOOD) 
		MyPuts("REMEMBER--The score doesn't really matter until the mission is accomplished!\n");
}

void plaque(void) 
{
}
