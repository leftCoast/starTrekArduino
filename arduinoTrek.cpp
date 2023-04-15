#include "arduinoTrek.h"
#include "idlers.h"
#include "timeObj.h"
#include "resizeBuff.h"
#include "sst.h"

#define	UNCONNECTED_ANALOG_PIN	A1		// Used for getting really random seed numbers.


// You can clear this after successfully calling SD.begin().
extern bool SDDriveReady = false;

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



// save game, start from saved..



// ************************* Directory object *************************


dirItem::dirItem(char* inName)
	: linkListObj() {
	
	int numBytes;
	
	mName = NULL;
	numBytes = strlen(inName)+1;
	if(numBytes) {
		if (resizeBuff(numBytes,&mName)) {
			strcpy(mName,inName);
		}
	}
}
	
	
dirItem::~dirItem(void) { resizeBuff(0,&mName); }

	
char* dirItem::getName(void) { return mName; }
				

dirList::dirList(void)
	: linkList() {  }
	
	
dirList::~dirList(void) {  }
	
	
void dirList::readDir(char* path) {

	File  	dir;																		// File handle used for the current directory.
	File  	entry;                                                   // File handle used for the different entries of the current directory.
	char		fileName[15];
	dirItem*	newItem;
	bool  	done;                                                    // A boolean used to track when to stop all this nonsense.

	dumpList();																			// Clear the list for a new set.
	dir = SD.open(path);																// Try opening the directory.
	if (dir) {																			// If we were able to open the working directory..
		dir.rewindDirectory();														// Rewind it to the first entry.
		done = false;																	// We ain't done yet.
		do {																				// Start looping through the entries.
			entry = dir.openNextFile();											// Grab an entry.
			if (entry) {																// If we got an entry..
				strcpy(fileName,entry.name());									// Grab the name.
				if (entry.isDirectory()) {											// If the entry is a directory..
					strcat(fileName,"/");											// Toss on a slash.
				}
				if (!strstr(fileName,"~1.")) {
					newItem = new dirItem(fileName);									// Create the list item.
					addToTop(newItem);													// Pop it in the list.
					entry.close();                                            // And we close the entry.
				}
			} else {                                                    // Else, we didn't get an entry from above.
				done = true;                                              // No entry means, we are done here.
			}
		} while (!done);                                              	// And we do this loop, over and over, while we are not done.
		dir.close();                                                  	// Looping through entries is done, close up the original file.
	} else {                                                        	// If this worked correctly, we'd know there was an error at this point.                                        
		Serial.println("Fail to open file.");                         	// Sadly, instead of returning a NULL, it just crashes.
	}
}


char* dirList::getDirItem(int index) {

	dirItem* anItem;
	
	anItem = (dirItem*)getByIndex(index);
	if (anItem) {
		return anItem->getName();
	}
	return NULL;
}


// ************************* file code *************************

dirList	ourDirList;
File		fp;

int readDir(char* path) {
	
	if (SDDriveReady) {
		ourDirList.readDir(path);
		return ourDirList.getCount();
	}
	return 0;
}


int numDirItems() {

	if (SDDriveReady) {
		return ourDirList.getCount();
	}
	return 0;
}


char* getDirItem(int index) { 

	char*	dirItem;
	
	if (SDDriveReady) {
		dirItem = ourDirList.getDirItem(index);
		return dirItem;
	}
	return NULL;
}

			
bool openForSave(char* fullpath) {
	
	if (SDDriveReady) {
		fp = SD.open(fullpath, FILE_WRITE);
		if (fp) {
			fp.seek(0);
			return true;
		}
	}
	return false;
}


void writeData(char* data,int numBytes) {
	
	if (SDDriveReady) {
		if (fp) {
			fp.write(data,numBytes);
		}
	}
}


bool openForRead(char* fullPath) {
	
	if (SDDriveReady) {
		fp = SD.open(fullPath, FILE_READ);
		return fp != 0;
	}
	return false;
}


void readData(char* data,int numBytes) {
	
	if (SDDriveReady) {
		if (fp) {
			fp.read((uint8_t*)data,numBytes);
		}
	}
}


void closeFile(void) {

	if (SDDriveReady) {
		if (fp) {
			fp.close();
		}
		ourDirList.dumpList();
	}
}

// **********************  End file code *************************



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
	
	
	
	