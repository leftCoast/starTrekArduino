#include "sst.h"

static char classes[4][2]={"","M","N","O"};
static int height;

static int consumeTime(void) {
/* I think most of this avoidance was caused by overlay scheme.
   Let's see what happens if all events can occur here */

//	double asave;
	ididit = 1;
#if 0
	/* Don't wory about this */
	if (future[FTBEAM] <= d.date+Time && d.remcom != 0 && condit != IHDOCKED) {
		/* We are about to be tractor beamed -- operation fails */
		return 1;
	}
#endif
//	asave = future[FSNOVA];
//	future[FSNOVA] = 1e30; /* defer supernovas */
	events();	/* Used to avoid if future[FSCMOVE] within time */
//	future[FSNOVA] = asave;
	/*fails if game over, quadrant super-novas or we've moved to new quadrant*/
	if (alldone || d.galaxy[quadx][quady] == 1000 || justin != 0) return 1;
	return 0;
}

void planetreport(void) 
{
	int iknow = 0, i;
	skip(1);
	chew();
	prout((char*)"Spock-  \"Planet report follows, Captain.\"");
	skip(1);
	for (i = 1; i <= inplan; i++) {
		if (d.plnets[i].known
#ifdef DEBUG
			|| ( idebug && d.plnets[i].x !=0)
#endif
			) {
			iknow = 1;
#ifdef DEBUG
			if (idebug && d.plnets[i].known==0) proutn("(Unknown) ");
#endif
			cramlc(1, d.plnets[i].x, d.plnets[i].y);
			proutn((char*)"   class ");
			proutn(classes[d.plnets[i].pclass]);
			proutn((char*)"   ");
			if (d.plnets[i].crystals == 0) proutn((char*)"no ");
			prout((char*)"dilithium crystals present.");
			if (d.plnets[i].known==2) 
				prout((char*)"    Shuttle Craft Galileo on surface.");
		}
	}
	if (iknow==0) prout((char*)"No information available.");
}

void orbit(void) 
{
	skip(1);
	chew();
	ididit=0;
	if (inorbit!=0) {
		prout((char*)"Already in standard orbit.");
		return;
	}
	if (damage[DWARPEN] != 0 && damage[DIMPULS] != 0) {
		prout((char*)"Both warp and impulse engines damaged.");
		return;
	}
	if (plnetx == 0 || abs(sectx-plnetx) > 1 || abs(secty-plnety) > 1) {
		crmshp();
		prout((char*)" not adjacent to planet.\n");
		return;
	}
	Time = 0.02+0.03*Rand();
	prout((char*)"Helmsman Sulu-  \"Entering standard orbit, Sir.\"");
	newcnd();
	if (consumeTime()) return;
	proutn((char*)"Sulu-  \"Entered orbit at altitude ");
	cramf(height = (1400.+7200.*Rand()), 0, 2);
	prout((char*)" kilometers.\"");
	inorbit = 1;
	return;
}

void sensor(void) 
{
	skip(1);
	chew();
	if (damage[DSRSENS] != 0.0) {
		prout((char*)"Short range sensors damaged.");
		return;
	}
	if (plnetx == 0) {
		prout((char*)"No planet in this quadrant.");
		return;
	}
	proutn((char*)"Spock-  \"Sensor scan for");
	cramlc(1, quadx, quady);
	prout((char*)"-");
	skip(1);
	proutn((char*)"         Planet at");
	cramlc(2, plnetx, plnety);
	proutn((char*)" is of class ");
	proutn(classes[d.plnets[iplnet].pclass]);
	prout((char*)".");
	if (d.plnets[iplnet].known==2) 
		prout((char*)"         Sensors show Galileo still on surface.");
	proutn((char*)"         Readings indicate");
	if (d.plnets[iplnet].crystals == 0) proutn((char*)" no");
	prout((char*)" dilithium crystals present.\"");
	if (d.plnets[iplnet].known == 0) d.plnets[iplnet].known = 1;
	return;
}

void beam(void) {
	chew();
	skip(1);
	if (damage[DTRANSP] != 0) {
		prout((char*)"Transporter damaged.");
		if (damage[DSHUTTL]==0 && (d.plnets[iplnet].known==2 || iscraft == 1)) {
			skip(1);
			prout((char*)"Spock-  \"May I suggest the shuttle craft, Sir?\" ");
			if (ja() != 0) shuttle();
		}
		return;
	}
	if (inorbit==0) {
		crmshp();
		prout((char*)" not in standard orbit.");
		return;
	}
	if (shldup!=0) {
		prout((char*)"Impossible to transport through shields.");
		return;
	}
	if (d.plnets[iplnet].known==0) {
		prout((char*)"Spock-  \"Captain, we have no information on this planet");
		prout((char*)"  and Starfleet Regulations clearly state that in this situation");
		prout((char*)"  you may not go down.\"");
		return;
	}
	if (landed==1) {
		/* Coming from planet */
		if (d.plnets[iplnet].known==2) {
			proutn((char*)"Spock-  \"Wouldn't you rather take the Galileo?\" ");
			if (ja() != 0) {
				chew();
				return;
			}
			prout((char*)"Your crew hides the Galileo to prevent capture by aliens.");
		}
		prout((char*)"Landing party assembled, ready to beam up.");
		skip(1);
		prout((char*)"Kirk whips out communicator...");
		prouts((char*)"BEEP  BEEP  BEEP");
		skip(2);
		prout((char*)"\"Kirk to enterprise-  Lock on coordinates...energize.\"");
	}
	else {
		/* Going to planet */
		if (d.plnets[iplnet].crystals==0) {
			prout((char*)"Spock-  \"Captain, I fail to see the logic in");
			prout((char*)"  exploring a planet with no dilithium crystals.");
			proutn((char*)"  Are you sure this is wise?\" ");
			if (ja()==0) {
				chew();
				return;
			}
		}
		prout((char*)"Scotty-  \"Transporter room ready, Sir.\"");
		skip(1);
		prout((char*)"Kirk, and landing party prepare to beam down to planet surface.");
		skip(1);
		prout((char*)"Kirk-  \"Energize.\"");
	}
	skip(1);
	prouts((char*)"WWHOOOIIIIIRRRRREEEE.E.E.  .  .  .  .   .    .");
	skip(2);
	if (Rand() > 0.98) {
		prouts((char*)"BOOOIIIOOOIIOOOOIIIOIING . . .");
		skip(2);
		prout((char*)"Scotty-  \"Oh my God!  I've lost them.\"");
		finish(FLOST);
		return;
	}
	prouts((char*)".    .   .  .  .  .  .E.E.EEEERRRRRIIIIIOOOHWW");
	skip(2);
	prout((char*)"Transport complete.");
	landed = -landed;
	if (landed==1 && d.plnets[iplnet].known==2) {
		prout((char*)"The shuttle craft Galileo is here!");
	}
	if (landed!=1 && imine==1) {
		icrystl = 1;
		cryprob = 0.05;
	}
	imine = 0;
	return;
}

void mine(void) {

	ididit = 0;
	skip(1);
	chew();
	if (landed!= 1) {
		prout((char*)"Mining party not on planet.");
		return;
	}
	if (d.plnets[iplnet].crystals == 0) {
		prout((char*)"No dilithium crystals on this planet.");
		return;
	}
	if (imine == 1) {
		prout((char*)"You've already mined enough crystals for this trip.");
		return;
	}
	if (icrystl == 1 && cryprob == 0.05) {
		proutn((char*)"With all those fresh crystals aboard the ");
		crmshp();
		skip(1);
		prout((char*)"there's no reason to mine more at this time.");
		return;
	}
	Time = (0.1+0.2*Rand())*d.plnets[iplnet].pclass;
	if (consumeTime()) return;
	prout((char*)"Mining operation complete.");
	imine = 1;
	return;
}

void usecrystals(void) {

	skip(1);
	chew();
	if (icrystl!=1) {
		prout((char*)"No dilithium crystals available.");
		return;
	}
	if (energy >= 1000) {
		prout((char*)"Spock-  \"Captain, Starfleet Regulations prohibit such an operation");
		prout((char*)"  except when condition Yellow exists.");
		return;
	}
	prout((char*)"Spock- \"Captain, I must warn you that loading");
	prout((char*)"  raw dilithium crystals into the ship's power");
	prout((char*)"  system may risk a severe explosion.");
	proutn((char*)"  Are you sure this is wise?\" ");
	if (ja()==0) {
		chew();
		return;
	}
	skip(1);
	prout((char*)"Engineering Officer Scott-  \"(GULP) Aye Sir.");
	prout((char*)"  Mr. Spock and I will try it.\"");
	skip(1);
	prout((char*)"Spock-  \"Crystals in place, Sir.");
	prout((char*)"  Ready to activate circuit.\"");
	skip(1);
	prouts((char*)"Scotty-  \"Keep your fingers crossed, Sir!\"");
	skip(1);
	if (Rand() <= cryprob) {
		prouts((char*)"  \"Activating now! - - No good!  It's***");
		skip(2);
		prouts((char*)"***RED ALERT!  RED A*L********************************");
		skip(1);
		stars();
		prouts((char*)"******************   KA-BOOM!!!!   *******************");
		skip(1);
		kaboom();
		return;
	}
	energy += 5000.0*(1.0 + 0.9*Rand());
	prouts((char*)"  \"Activating now! - - ");
	prout((char*)"The instruments");
	prout((char*)"   are going crazy, but I think it's");
	prout((char*)"   going to work!!  Congratulations, Sir!\"");
	cryprob *= 2.0;
	return;
}

void shuttle(void) {

	chew();
	skip(1);
	ididit = 0;
	if(damage[DSHUTTL] != 0.0) {
		if (damage[DSHUTTL] == -1.0) {
			if (inorbit && d.plnets[iplnet].known == 2)
				prout((char*)"Ye Faerie Queene has no shuttle craft bay to dock it at.");
			else
				prout((char*)"Ye Faerie Queene had no shuttle craft.");
		}
		else if (damage[DSHUTTL] > 0)
			prout((char*)"The Galileo is damaged.");
		else prout((char*)"Shuttle craft is now serving Big Mac's.");
		return;
	}
	if (inorbit==0) {
		crmshp();
		prout((char*)" not in standard orbit.");
		return;
	}
	if ((d.plnets[iplnet].known != 2) && iscraft != 1) {
		prout((char*)"Shuttle craft not currently available.");
		return;
	}
	if (landed==-1 && d.plnets[iplnet].known==2) {
		prout((char*)"You will have to beam down to retrieve the shuttle craft.");
		return;
	}
	if (shldup!=0 || condit == IHDOCKED) {
		prout((char*)"Shuttle craft cannot pass through shields.");
		return;
	}
	if (d.plnets[iplnet].known==0) {
		prout((char*)"Spock-  \"Captain, we have no information on this planet");
		prout((char*)"  and Starfleet Regulations clearly state that in this situation");
		prout((char*)"  you may not fly down.\"");
		return;
	}
	Time = 3.0e-5*height;
	if (Time >= 0.8*d.remtime) {
		prout((char*)"First Officer Spock-  \"Captain, I compute that such");
		prout((char*)"  a maneuver would require approximately ");
		cramf(100*Time/d.remtime,0,4);
		prout((char*)"% of our");
		prout((char*)"remaining time.");
		prout((char*)"Are you sure this is wise?\" ");
		if (ja()==0) {
			Time = 0.0;
			return;
		}
	}
	if (landed == 1) {
		/* Kirk on planet */
		if (iscraft==1) {
			/* Galileo on ship! */
			if (damage[DTRANSP]==0) {
				proutn((char*)"Spock-  \"Would you rather use the transporter?\" ");
				if (ja() != 0) {
					beam();
					return;
				}
				proutn((char*)"Shuttle crew");
			}
			else
				proutn((char*)"Rescue party");
			prout((char*)" boards Galileo and swoops toward planet surface.");
			iscraft = 0;
			skip(1);
			if (consumeTime()) return;
			d.plnets[iplnet].known=2;
			prout((char*)"Trip complete.");
			return;
		}
		else {
			/* Ready to go back to ship */
			prout((char*)"You and your mining party board the");
			prout((char*)"shuttle craft for the trip back to the Enterprise.");
			skip(1);
			prout((char*)"The short hop begins . . .");
			d.plnets[iplnet].known=1;
			icraft = 1;
			skip(1);
			landed = -1;
			if (consumeTime()) return;
			iscraft = 1;
			icraft = 0;
			if (imine!=0) {
				icrystl = 1;
				cryprob = 0.05;
			}
			imine = 0;
			prout((char*)"Trip complete.");
			return;
		}
	}
	else {
		/* Kirk on ship */
		/* and so is Galileo */
		prout((char*)"Mining party assembles in the hangar deck,");
		prout((char*)"ready to board the shuttle craft \"Galileo\".");
		skip(1);
		prouts((char*)"The hangar doors open; the trip begins.");
		skip(1);
		icraft = 1;
		iscraft = 0;
		if (consumeTime()) return;
		d.plnets[iplnet].known = 2;
		landed = 1;
		icraft = 0;
		prout((char*)"Trip complete");
		return;
	}
}
		
void deathray(void) {
	double r = Rand();
	
	ididit = 0;
	skip(1);
	chew();
	if (ship != IHE) {
		prout((char*)"Ye Faerie Queene has no death ray.");
		return;
	}
	if (nenhere==0) {
		prout((char*)"Sulu-  \"But Sir, there are no enemies in this quadrant.\"");
		return;
	}
	if (damage[DDRAY] > 0.0) {
		prout((char*)"Death Ray is damaged.");
		return;
	}
	prout((char*)"Spock-  \"Captain, the 'Experimental Death Ray'");
	prout((char*)"  is highly unpredictable.  Considering the alternatives,");
	prout((char*)"  are you sure this is wise?\" ");
	if (ja()==0) return;
	prout((char*)"Spock-  \"Acknowledged.\"");
	skip(1);
	ididit=1;
	prouts((char*)"WHOOEE ... WHOOEE ... WHOOEE ... WHOOEE");
	skip(1);
	prout((char*)"Crew scrambles in emergency preparation.");
	prout((char*)"Spock and Scotty ready the death ray and");
	prout((char*)"prepare to channel all ship's power to the device.");
	skip(1);
	prout((char*)"Spock-  \"Preparations complete, sir.\"");
	prout((char*)"Kirk-  \"Engage!\"");
	skip(1);
	prouts((char*)"WHIRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
	skip(1);
	if (r > .30) {
		prouts((char*)"Sulu- \"Captain!  It's working!\"");
		skip(2);
		while (nenhere > 0) {
			deadkl(kx[1],ky[1],quad[kx[1]][ky[1]],kx[1],ky[1]);
		}
		prout((char*)"Ensign Chekov-  \"Congratulations, Captain!\"");
		if (d.remkl == 0) finish(FWON);
		prout((char*)"Spock-  \"Captain, I believe the `Experimental Death Ray'");
		if (Rand() <= 0.05) {
			prout((char*)"   is still operational.\"");
		}
		else {
			prout((char*)"   has been rendered dysfunctional.\"");
			damage[DDRAY] = 39.95;
		}
		return;
	}
	r = Rand();	// Pick failure method 
	if (r <= .30) {
		prouts((char*)"Sulu- \"Captain!  It's working!\"");
		skip(1);
		prouts((char*)"***RED ALERT!  RED ALERT!");
		skip(1);
		prout((char*)"***MATTER-ANTIMATTER IMPLOSION IMMINENT!");
		skip(1);
		prouts((char*)"***RED ALERT!  RED A*L********************************");
		skip(1);
		stars();
		prouts((char*)"******************   KA-BOOM!!!!   *******************");
		skip(1);
		kaboom();
		return;
	}
	if (r <= .55) {
		prouts((char*)"Sulu- \"Captain!  Yagabandaghangrapl, brachriigringlanbla!\"");
		skip(1);
		prout((char*)"Lt. Uhura-  \"Graaeek!  Graaeek!\"");
		skip(1);
		prout((char*)"Spock-  \"Fascinating!  . . . All humans aboard");
		prout((char*)"  have apparently been transformed into strange mutations.");
		prout((char*)"  Vulcans do not seem to be affected.");
		skip(1);
		prout((char*)"Kirk-  \"Raauch!  Raauch!\"");
		finish(FDRAY);
		return;
	}
	if (r <= 0.75) {
		int i,j;
		prouts((char*)"Sulu- \"Captain!  It's   --WHAT?!?!\"");
		skip(2);
		proutn((char*)"Spock-  \"I believe the word is");
		prouts((char*)" *ASTONISHING*");
		prout((char*)" Mr. Sulu.");
		for (i=1; i<=10; i++)
			for (j=1; j<=10; j++)
				if (quad[i][j] == IHDOT) quad[i][j] = IHQUEST;
		prout((char*)"  Captain, our quadrant is now infested with");
		prouts((char*)" - - - - - -  *THINGS*.");
		skip(1);
		prout((char*)"  I have no logical explanation.\"");
		return;
	}
	prouts((char*)"Sulu- \"Captain!  The Death Ray is creating tribbles!\"");
	skip(1);
	prout((char*)"Scotty-  \"There are so many tribbles down here");
	prout((char*)"  in Engineering, we can't move for 'em, Captain.\"");
	finish(FTRIBBLE);
	return;
}
