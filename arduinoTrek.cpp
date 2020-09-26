#include "arduinoTrek.h"
#include "idlers.h"
#include "timeObj.h"
#include "sst.h"

#define	UNCONNECTED_ANALOG_PIN	A20
// We need to shut this nonsense off NOW!
bool	quickExit = false;

// Our output buffer that the ported code will see.
textBuff*	trekComBuffer;

// Their reply buffer.
textBuff*	trekReplyBuffer;


// Block 'till we get a char. I guess this is the waiting room.
int getch() {

	char theChar;
	
	while (trekComBuffer->empty()) {
		sleep(10);
		if (quickExit) {
			out("getch() got quick");
			return ((int)'\n');
		}
	}
	return (int)trekComBuffer->readChar();
}


// And where it all goes out.. Well for now, we'll stuff it in here.
void proutn(char* s) { trekReplyBuffer->addStr(s); }


// Sometimes they jjust want a char sent out.
void proutCh(char c) {  trekReplyBuffer->addChar(c); }


// Ok, they want this string to be printed s-l-o-w-l-y This'll do that.
void prouts(char* s) {

	timeObj	charDelay(1);//timeObj	charDelay(125); 
	int		i;
	
	i = 0;
	while(s[i]!='\0') {
		trekReplyBuffer->addChar(s[i]);
		i++;
		charDelay.start();
		while (!charDelay.ding()) sleep(10);
	}
}

#ifndef min
// It needs this.
int min(int a, int b) {
	if (a < b) return a;
	return b;
}
#endif

// For debugging.
void out(char* str) { Serial.print(str);Serial.flush(); }
void outln(char* str) { Serial.println(str);Serial.flush(); }


// Setup stuff for game, allocate buffers.
void arduinoTrekSetup(char* comList) {
			
	trekComBuffer = new textBuff(COMBUFF_BYTES);			// Allocate your command buffer.
	trekReplyBuffer = new textBuff(REPLYBUFF_BYTES);	// Allocate the reply buffer.
	fromcommandline = 0;											// This basically says, ask all the questions.
	prelim();														// Prints out the initial game header, -STAR TREK- bla bla bla.
	if (strlen(comList)) {										// If they passed in a string of commands..
		trekComBuffer->addStr(comList,false);				// You can preload startup commands.
		fromcommandline = 1;										// This is saying, see what they sent before asking.
	}		
}  


// Loop stuff. This actually blocks the loop for the entire game. Win, loose, or exit is
// what drops you out of the this call. There needs to be an idler function that feeds in
// command chars and route's the output to wherever it goes.i
void arduinoTrekLoop(void) {
	
	long int	seed;
	
	seed = analogRead(UNCONNECTED_ANALOG_PIN);				// Tired of playing the same old game every time?
	randomSeed(seed);													// Reading an unconnected analog port for a random seed will mix it up
	setupsst();															
	if (quickExit) return;
	if (alldone) {
		score(0);
		alldone = 0;
	} else {
		doMakeMoves();
		if (quickExit) return;
	}
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
}	
	
	
	
	